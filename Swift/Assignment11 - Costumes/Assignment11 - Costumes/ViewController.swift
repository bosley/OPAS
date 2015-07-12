//
//  ViewController.swift
//  Assignment11 - Costumes
//
//  Created by Josh Allen Bosley on 10/3/14.
//  Copyright (c) 2014 Lssu. All rights reserved.
//

import UIKit
import CoreData

class ViewController: UIViewController, UIPickerViewDelegate,
                      UIPickerViewDataSource, UITextFieldDelegate
{
    
    var totalOrdersInDatabase : Int = 0
    
    // Outlets
    @IBOutlet var firstNameLabel: UILabel!
    
    @IBOutlet var lastNameLabel: UILabel!
    
    @IBOutlet var firstName: UITextField!
    
    @IBOutlet var lastName: UITextField!
    
    @IBOutlet var thePicker: UIPickerView!
    
    @IBOutlet var quantityField: UITextField!
    
    @IBOutlet var totalValueLabel: UILabel!
    
    @IBOutlet var currentUserLabel: UILabel!
    
    @IBOutlet var cancelButton: UIButton!
    
    // Picker Information / Setup
    
    let costume = 0, size = 1
    var costumes = ["Batman", "Ironman", "Ghost", "Schemm", "Wraith", "Teacher"]
    var sizes = ["Small - 4.99", "Medium - 8.99", "Large - 10.47", "Adult - 20.00"]
    var nameList = ["Small", "Medium", "Large", "Adult"]
    let costsDictionary = [
        0 : 4.99,
        1 : 8.99,
        2 : 10.47,
        3 : 20.00
    ]
    
    
    func numberOfComponentsInPickerView(pickerView: UIPickerView) -> Int
    {
        return 2
    }

    func pickerView(pickerView: UIPickerView, numberOfRowsInComponent component: Int) -> Int
    {
        if( component == costume )
        {
            return costumes.count
        }
        else if( component == size )
        {
            return sizes.count
        }
        else
        {
            // Should never reach here
            return 0
        }
    }
    
    func pickerView(pickerView: UIPickerView, titleForRow row: Int, forComponent component: Int) -> String
    {
        if( component == costume )
        {
            return costumes[row]
        }
        else
        {
            return sizes[row]
        }
    }
    
    // Add Item to order
    
    @IBAction func addItemToOrder(sender: AnyObject)
    {
        if ( acceptForm() )
        {
            let ci = thePicker.selectedRowInComponent(0)
            let si = thePicker.selectedRowInComponent(1)
            let theCost : Double = costsDictionary[si]!
            var x = nameList[si], y = costumes[ci], z = quantityField.text
        
            if ( !theUsersOrder.isSet )
            {
                switchUserOrdserIsSet()
            }
        
            if (( z.toInt() ) != nil)
            {
                // The cost is *= with count in the addItem function.
            
                theUsersOrder.addItem(x, theirCostume: y, count: z.toInt()!, cost: theCost)
                totalValueLabel.text = NSString(format: "%.2f", theUsersOrder.totalOrderCost)
            
                // Show the information
                
                var tc : Double = theCost * Double(z.toInt()!)
                var showCost = NSString(format: "%.2f", tc)
                showAlert("Added to order : ", theMessage: "\(z) \(nameList[si]) \(y)(s) for \(showCost)")
                quantityField.text = ""
            }
            else
            {
                showAlert("ERROR", theMessage: "Invalid Quantitiy")
            }
        }
    }

    // Place Order Button - Saves to the entity Orders
    
    @IBAction func placeOrder(sender: AnyObject)
    {
        if ( theUsersOrder.isSet  )
        {
            let ci = thePicker.selectedRowInComponent(0)
            let si = thePicker.selectedRowInComponent(1)
        
            // Declair which db is to be used (Entity), and set its context
        
            let Entity = "OrdersDB"
            let ManagedContext = (UIApplication.sharedApplication().delegate as AppDelegate).managedObjectContext
        
            // New Object for into Entity, the first reason ManagedContext is needed
        
            var newOrder = NSEntityDescription.insertNewObjectForEntityForName( Entity,
                inManagedObjectContext: ManagedContext!) as NSManagedObject
        
            // Set columns of object in db.

            var costume = costumes[ci]
            var costSize = sizes[si]
        
            newOrder.setValue( String(totalOrdersInDatabase), forKey: "orderID")
            newOrder.setValue( theUsersOrder.name, forKey: "name" )
            newOrder.setValue( theUsersOrder.totalOrderCost, forKey: "totalCost" )
            newOrder.setValue( theUsersOrder.totalCostumesOrdered, forKey: "totalCostumeCount" )
        
            var costumeList : String = ""
            for element in theUsersOrder.items
            {
                costumeList += (String(element.quantity) + " " + element.costumeSize
                + " " + element.costumeOrdered + ", ")
            }
        
            newOrder.setValue(costumeList, forKey: "costumeList")
            ManagedContext?.save( nil )

            // Once saved, tell the user about their order
        
            var outMessage = " Name: \(theUsersOrder.name) \n Costume(s) : \(costumeList) \n Total Cost : \(theUsersOrder.totalOrderCost)"
        
            showAlert("Order Placed : ", theMessage: outMessage)
        
            switchUserOrdserIsSet()
        }
        else
        {
            showAlert("ERROR", theMessage: "No items in order")
        }
    }
    
    // Retrieve Order Button - Gets the information for the order by customer name
    
    @IBAction func retrieveOrder(sender: AnyObject)
    {
        let Entity = "OrdersDB"
        let ManagedContext = (UIApplication.sharedApplication().delegate as AppDelegate).managedObjectContext
        
        let name = getUsersName()
        
        // Build a request
        
        var request = NSFetchRequest(entityName: Entity)
        request.returnsObjectsAsFaults = false
        
        // Be specific with a predicate
        
        request.predicate = NSPredicate(format: "name = %@", "" + name)
        
        // Submit request, and get back as NSarray
        
        var results: NSArray = ManagedContext!.executeFetchRequest(request, error: nil)!

        
        if (results.count > 0)
        {
            var res = results[0] as NSManagedObject
            
            var user : AnyObject! = res.valueForKey("name")
            var userid : AnyObject! = res.valueForKey("orderID")
            var totalCost : AnyObject! = res.valueForKey("totalCost")
            var costumeList : AnyObject!  = res.valueForKey("costumeList")
            var totalCostumeCount : AnyObject! = res.valueForKey("totalCostumeCount")
            
            var outMessage =
            "Name : \(user) \n\n Costumes Purchased : \n \t \(costumeList) \n\n Number of  Costumes Purchased : \(totalCostumeCount) \n\n Order Total : \(totalCost)"
            
            showAlert(" Order Number : \(userid) \n ", theMessage: outMessage)
        }
        else
        {
            showAlert("Order Not Found", theMessage: " No record of \(name) .")
        }
        
        firstName.text = ""
        lastName.text = ""
    }
    
    // Cancel Order 
    
    @IBAction func cancelCurrentOrder(sender: AnyObject)
    {
        switchUserOrdserIsSet()
    }

    // Alert Function
    
    func showAlert( theTitle: String, theMessage: String )
    {
        let allertContoller = UIAlertController(title: theTitle, message: theMessage, preferredStyle: .Alert)
        let defaultAction = UIAlertAction(title: "OK", style: .Default, handler: nil)
        allertContoller.addAction(defaultAction)
        presentViewController(allertContoller, animated: true, completion: nil)
    }
    
    // Keyboard Handling
    
    override func touchesBegan(touches: NSSet, withEvent event: UIEvent)
    {
        quantityField.resignFirstResponder()
        firstName.resignFirstResponder()
        lastName.resignFirstResponder()
    }
    
    // Change Screen objects around, and flip theUserOrder.isSet
    
    func switchUserOrdserIsSet()
    {
        if ( theUsersOrder.isSet )
        {
            theUsersOrder.reset()
            currentUserLabel.text = ""
            firstName.text = ""
            lastName.text = ""
            currentUserLabel.hidden = true
            firstNameLabel.hidden = false
            lastNameLabel.hidden = false
            firstName.hidden = false
            lastName.hidden = false
            totalValueLabel.text = "0.0"
            cancelButton.enabled = false
            
        }
        else
        {
            theUsersOrder.name = getUsersName()
            currentUserLabel.text = theUsersOrder.name
            currentUserLabel.hidden = false
            theUsersOrder.isSet = true
            firstNameLabel.hidden = true
            lastNameLabel.hidden = true
            firstName.hidden = true
            lastName.hidden = true
            cancelButton.enabled = true
        }
    }
    
    // Get quality name info
    
    func getUsersName() -> String
    {
        var fname : String = firstName.text.stringByReplacingOccurrencesOfString(" ", withString: "", options: NSStringCompareOptions.LiteralSearch, range: nil)
        
        var lname : String = lastName.text.stringByReplacingOccurrencesOfString(" ", withString: "", options: NSStringCompareOptions.LiteralSearch, range: nil)
        
        return "\(fname) \(lname)"
    }
    
    // Ensure a good form
    
    func acceptForm() -> Bool
    {
        if ( ( firstName.text == "" || lastName.text == "" ) && !theUsersOrder.isSet )
        {
            showAlert("ERROR", theMessage: "No User Information Given")
            return false
        }
        else if ( ( quantityField.text .toInt() ) == nil)
        {
            showAlert("ERROR", theMessage: "Invalid Quantity")
            return false
        }
        else
        {
            return true
        }
    }
    
    // Load DB info
    
    func loadProgramInformation()
    {
        let Entity = "OrdersDB"
        let ManagedContext = (UIApplication.sharedApplication().delegate as AppDelegate).managedObjectContext
        var request = NSFetchRequest(entityName: Entity)
        request.returnsObjectsAsFaults = false
        var results: NSArray = ManagedContext!.executeFetchRequest(request, error: nil)!
        totalOrdersInDatabase = results.count
        // println("\(totalOrdersInDatabase) orders are in the DB")
    }
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        loadProgramInformation()
        cancelButton.enabled = false
    }

    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
    }
}

