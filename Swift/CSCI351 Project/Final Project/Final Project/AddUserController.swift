//
//  ViewController.swift
//  Final Project
//
//  Created by Josh Bosley, Stefan Hurthibise on 11/14/14.
//  Copyright (c) 2014 Josh Bosley. All rights reserved.
//

import UIKit
import CoreData

class AddUserController: UIViewController, UITextFieldDelegate
{
    
    @IBOutlet weak var firstNameBox: UITextField!
    
    @IBOutlet weak var lastNameBox: UITextField!
    
    @IBOutlet weak var departmentNameBox: UITextField!
    
    @IBOutlet weak var lssuIdBox: UITextField!
    
    @IBAction func doneButton(sender: AnyObject)
    {
        // Add Data To Core Data
        var givenName = firstNameBox.text
        var birthName = lastNameBox.text
        var department = departmentNameBox.text
        var aNumber = lssuIdBox.text
        
        //Add information given to database
        let Entity = "UserInfo"
        let ManagedContext = (UIApplication.sharedApplication().delegate as AppDelegate).managedObjectContext
        
        var newOrder = NSEntityDescription.insertNewObjectForEntityForName( Entity,
            inManagedObjectContext: ManagedContext!) as NSManagedObject
        
        newOrder.setValue(givenName, forKey: "firstName")
        newOrder.setValue(birthName, forKey: "lastName")
        newOrder.setValue(department, forKey: "departmentName")
        newOrder.setValue(aNumber, forKey: "idNumber")
        ManagedContext?.save( nil )
        
        inventoryManager.inventory = [inventoryStruct]()
        reservationManger.reservations = [reservationStruct]()
        
        
        // Re-run server call for populating user stuff
        sockItToMe.ranServerCall = false
    }
    
    // Touch delegate functions override
    func textFieldShouldReturn(textField: UITextField) -> Bool
    {
        textField.resignFirstResponder()
        return true
    }
    
    override func touchesBegan(touches: NSSet, withEvent event: UIEvent)
    {
        self.view.endEditing(true)
    }
    // End touch delegate functions
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
    }
    
    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
    }
}

