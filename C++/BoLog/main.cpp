
/*
 
    Josh A. Bosley 2017
    Demonstration of Bolog
 
 */

#include <iostream>

// Include bolog
#include "engine/bologlib.hpp"

using namespace std;

// The bolog object
blg::lib bol;

// Demonstration of true/false response query
void loadSubmition(string query){
    if(bol.submit(query)){
        cout << "Query -> " << " : [ valid ] (" << query  << ")" << endl;
    } else {
        cout << "Query -> " << " : [invalid] (" << query  << ")" << endl;
    }
}

// Demonstration of returning data from a query
void getResponse(string query){
    std::cout << query << "\n\t -> " << bol.query(query) << endl;
}

// Entry
int main(int argc, const char * argv[]) {
    
    // Show the syntax
    bol.userHelp();
    std::cout << "\n\n-------[END HELP]-------\n" << endl;
    
    // Setting 'facts' a.k.a associations
    loadSubmition("- fruit apple bananas pear grapes pineapple cherry");
    loadSubmition("- bird chicken turkey duck");
    loadSubmition("- meat birds burger pork bacon");
    loadSubmition("- dairy cheese milk");
    loadSubmition("- food meat dairy fruit");
    loadSubmition("- person josh jill jack james johonas");
    loadSubmition("- planet mercury venus earth mars jupiter saturn neptune uranus");
    loadSubmition(". josh has shoes pants socks shirts jackets dog car");
    loadSubmition(". josh can program speak_english walk run jump");
    
    // An invalid query
    cout << "\n\nIs there a moot in the fruit? (Shouldn't be!)" << endl;
    loadSubmition("? moot fruit");
    
    // Valid queries
    cout << "\n\nWhat about apple and pear?" << endl;
    loadSubmition("? apple fruit");
    loadSubmition("? pear fruit");
    
    // Show inhereted properties
    // [ chicken is a bird and birds are meat, and meat is food therefor chicken is food]
    cout << "\n\nIs chicken food ?" << endl;
    loadSubmition("? chicken meat");
    
    // Show the associations connected to the 'josh' rule
    cout << "\n\nShow tokens belonging to josh, and the members of a token" << endl;
    getResponse("= josh");
    
    // Show what is associated via the token 'has'
    getResponse("~ josh has");
    
    // Demonstration of a deletion
    cout << "---- " << endl;
    getResponse("$ fruit");
    cout << "---- " << endl;
    loadSubmition("! - fruit pear");
    cout << "---- " << endl;
    getResponse("$ fruit");
    cout << "---- " << endl;
    
    // Start user session
    cout << "\n\n ---- [ Starting user session ] ---- " << endl;
    cout << "\n -> use 'q' to quit, or 'help' for help\n" << endl;
    
    bol.userSession();
    return 0;
}
