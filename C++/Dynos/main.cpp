/*
 * The MIT License
 *
 * Copyright 2018 Josh A. Bosley.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* 
 * File:   main.cpp
 * Author: Josh A. Bosley
 *
 * Created on March 29, 2018, 10:23 PM
 */

#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "core.h"
#include "host/helpers.h"
#include "host/DydnoDB.h"
#include "ckey/ckey.h"
#include "cmac/cmac.h"
#include "blocker/Blocker.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

void waitForShutdown()
{
    while(!dcore::CheckForShutdown())
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
}

int App(int argc, char *argv[])
{
    // Handle input arguments
    po::options_description desc("Program options");
    desc.add_options()
        ("help", "show help message")
        ("daemon", "Enable daemon")
        ("recycle", "Clear the existing FS")
        ("create", "Setup the dynos system")
        ("light", "Set operating mode to 'light'")
        ("full", "Set operating mode to 'full'")
        ("location", po::value<std::string>(), "Set install location")
    ;

    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    }
    catch (po::error const& e) 
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    po::notify(vm);    

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }
    
    if(vm.count("recycle"))
    {
        dhost::DynosDB db_man;
        fs::path srat("./local_config");
        if(!fs::is_directory(srat))
        {
            std::cout << "Nothing to recycle.\n";
            return 0;
        }
        
        char buffer[DYNOS_STANDARD_BUFFER];
        
        if(!db_man.getLocalConfigData("install_location", buffer))
        {
            std::cout << "Unable to load install location!";
            return 1;
        }
        
        std::string location(buffer);
        std::cout << "Attempting to purge : " << location << std::endl;
        
        try
        {
            fs::remove_all(fs::path(location));
            fs::remove_all(srat);
        } catch(const fs::filesystem_error& e)
        {
            std::cout << "Failure removing " << location << "\n" <<
                    e.code().message() << "\n";
            return 1;
        }
        std::cout << "Complete\n";
        return 0;
    }
    else if(vm.count("create"))
    {
        dhost::DynosDB db_man;
        fs::path srat("./local_config");
        
        if(fs::is_directory(srat))
        {
            std::cout << "A setup directory was discovered. Run --recycle\n";
            return 0;
        } else {
            fs::create_directory(srat);
        }
        
        if(vm.count("location"))
        {
            fs::path p(vm["location"].as<std::string>());
            try {
               if(!fs::is_directory(p))
               {
                   std::cout << "Location " << vm["location"].as<std::string>() <<
                           " is not a directory!\n";
                   return 1;
               }
            } catch(const fs::filesystem_error& e)
            {
                std::cout << "Failure checking location\n" <<
                        e.code().message() << "\n";
                return 1;
            }
            
            p /= "dynosd";
            try{
                fs::create_directory(p);
            } catch(const fs::filesystem_error& e)
            {
                std::cout << "Failure creating " << p.string()  << "\n" <<
                        e.code().message() << "\n";
                return 1;
            }
        }
        else
        {
            std::cout << "An install location must be specified!\n";
            return 1;
        }
        
        if(vm.count("full") && vm.count("light"))
        {
            std::cout << "Error, must pick either --light, or --full. Not both\n";
            return 1;
        }
        if(!vm.count("full") && !vm.count("light"))
        {
            std::cout << "Error, must pick either --light, or --full. Not both\n";
            return 1;
        }
        
        // Create the file system as-per specification
        fs::path store(vm["location"].as<std::string>()); store /= "dynosd";
       
        std::string op_mode;        
        (vm.count("light")) ? op_mode = "light" : op_mode = "full";
        
        keyman::ckey key_man(store.string());
   
        // Create an address for the system based on primary MAC
        std::string address;
        char primaryMac[MAC_ADDRESS_LENGTH];
        if(macman::GetPrimaryMac(primaryMac))
        {
            address = dhost::GenAddress(std::string(primaryMac));
            if(key_man.cnewKey(address, false))
            {
                std::cout << "Created new address for host\n";
            }
        } 
        
        std::cout << "Writing out data stores\n";
        
        db_man.writePrimaryConfig(store.string(), op_mode, address);
        db_man.initDB("seeds.db", store.string());
        db_man.initDB("chain.db", store.string());
        
        std::cout << "Generating genesis..\n";
        
        if(!blocks::CreateGenesis(address))
        {
            std::cout << "Unable to create genesis!\n";
            return 1;
        }
        std::cout << "Setup Completed\n";
        return 1;
    }
    
    if (vm.count("daemon")) {
        std::cout << ">> Enable daemon [NOT YET DONE]\n";
    }

    if(!dcore::Start())
    {
        std::cout << "Unable to start dynos core...\n";
        return 1;
    }
    
    waitForShutdown();

    return dcore::PerformShutdown();
}

int main(int argc, char *argv[])
{
    return App(argc, argv);
}