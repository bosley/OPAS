//
//  ViewController.swift
//  Final Project
//
//  Created by Josh Bosley, Stefan Hurthibise on 11/14/14.
//  Copyright (c) 2014 Josh Bosley. All rights reserved.
//

import UIKit
import CoreData

// Set by the program as users A# on load

var currentUser : String = ""

class ViewController: UIViewController
{
    // Get data from server everytime for up to date scheduling
    // will not load into core data, so they do not over-book.
    // Instead, the only thing to load into core data will be 
    // whether or not someone has setup an account
    
    // Buttons on root view
    @IBOutlet weak var newReservation: UIButton!
    @IBOutlet weak var checkReservation: UIButton!
    @IBOutlet weak var setupUser: UIButton!
    @IBOutlet weak var userLabel: UILabel!
    
    // Load check variables
    var devicesLoaded : Bool = false
    var updatesLoaded : Bool = false
    var firstTimeUsed : Bool = true
    
    // --------[ SOCKET STUFF ]--------
    var serverWaitTimer = NSTimer()
    var dataFromServer : AnyObject!
    
    func getSocketData()
    {
        sockItToMe.setDataRetrieved()
        dataFromServer = sockItToMe.allDataFromSocket[0];
        serverWaitTimer.invalidate()
        hackAndSlashDatData()
    }
    
    func hackAndSlashDatData()
    {
        var tempArray = dataFromServer.componentsSeparatedByString("~")
        var deviceArray = tempArray[0].componentsSeparatedByString("#")
        var updateArray = tempArray[1].componentsSeparatedByString("#")
        
        checkFirstTimeUsed()
        loadServerDataIntoCoreData(deviceArray, updates: updateArray)
    }
    // --------[END SOCKET STUFF]--------
    
    func loadServerDataIntoCoreData(devices: AnyObject, updates: AnyObject)
    {
        if devices.count > 0
        {
            println(devices)
            
            if ( sockItToMe.ranServerCall )
            {
                inventoryManager.populateInventory(devices)
                devicesLoaded = true
            
                // Nested because updates is dependant on inventory manager
                if updates.count > 0
                {
                    println(updates)
                    reservationManger.populateReservations(updates)
                    updatesLoaded = true
                }
            }
        }
        
        if updatesLoaded && devicesLoaded
        {
            // Check core data to see if this is the first time
            buttonAvailabilityCheck()
        }
        else
        {
            showAlert("Error", message: "Please connect to a network and restart the App.")
        }
    }
    
    func checkFirstTimeUsed()
    {
        let Entity = "UserInfo"
        let ManagedContext = (UIApplication.sharedApplication().delegate as AppDelegate).managedObjectContext
        
        let check = NSFetchRequest(entityName: Entity)
        
        var results: NSArray = ManagedContext!.executeFetchRequest(check, error: nil)!
        
        if( results.count == 0 )
        {
            firstTimeUsed = true
            
            userLabel.text = "Please setup your account."
        }
        else
        {
            var res = results[0] as NSManagedObject
            var user : AnyObject! = res.valueForKey("idNumber")
            currentUser = user! as String
            firstTimeUsed = false
            
            userLabel.text = "\(currentUser)"
            
         //   println(" USER : \(user)")
            println(" CURRUSR: \(currentUser)")
            
        }
    }
    
    func buttonAvailabilityCheck()
    {
        if firstTimeUsed
        {
            setupUser.hidden = false
        }
        else
        {
            newReservation.enabled = true
            checkReservation.enabled = true
            setupUser.hidden = true
        }
    }
    
    func showAlert(title: String, message: String)
    {
        let allertContoller = UIAlertController(title: title, message: message, preferredStyle: .Alert)
        let defaultAction = UIAlertAction(title: "OK", style: .Default, handler: nil)
        allertContoller.addAction(defaultAction)
        presentViewController(allertContoller, animated: true, completion: nil)
    }
    
    override func viewDidLoad()
    {
        setupUser.hidden = true
        newReservation.enabled = false
        checkReservation.enabled = false
        
        // ----------[SOCKET INIT STUFF]----------
        if ( !sockItToMe.ranServerCall )
        {
            sockItToMe.makeRequestForDevices()
            serverWaitTimer = NSTimer.scheduledTimerWithTimeInterval(1, target: self, selector: Selector("getSocketData"), userInfo: nil, repeats: true)
        }
        else
        {
            newReservation.enabled = true
            checkReservation.enabled = true
            setupUser.hidden = true
        }
        // --------[END SOCKET INIT STUFF]--------
        
        super.viewDidLoad()
    }

    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
    }
}

