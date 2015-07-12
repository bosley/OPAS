//
//  ViewController.swift
//  Final Project
//
//  Created by Josh Bosley, Stefan Hurthibise on 11/14/14.
//  Copyright (c) 2014 Josh Bosley. All rights reserved.
//

import UIKit
import CoreData

class NewReservationController: UIViewController, UITableViewDelegate, UITableViewDataSource, UITextFieldDelegate
{
    
    @IBOutlet weak var returnButton: UIButton!
    @IBOutlet weak var doneButton: UIButton!
    @IBOutlet weak var submitResBtn: UIButton!
    
    /*
    
                ITEM SELECTION VIEW START
    
    */
    
    var itemsSelected = [inventoryStruct]()
    var tempInventory = [inventoryStruct]()
    
    
    var itemsSelectedString : String = ""
    
    @IBOutlet weak var tableView: UITableView!
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int
    {
        return tempInventory.count
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell
    {
        let cell: UITableViewCell = UITableViewCell( style: UITableViewCellStyle.Subtitle, reuseIdentifier: "GOAT")
        cell.textLabel.text = tempInventory[indexPath.row].title
        cell.detailTextLabel?.text = tempInventory[indexPath.row].description
        return cell
    }

    @IBAction func addItem(sender: AnyObject)
    {
        
        var x = tableView.indexPathForSelectedRow()
        var y = x?.row
        
        if ( y != nil)
        {
            itemsSelected.append(tempInventory[y!])
            tempInventory.removeAtIndex(y!)
            tableView.reloadData()
        }
    }
    
    @IBAction func itemsAddedNextBtn(sender: AnyObject)
    {
        if itemsSelected.count > 0
        {
            newReservation.itemsReserved = itemsSelected
        }
    }
    
    
    /*
    
            START TIME SELECTION
    
    */
    
    @IBOutlet weak var startDateTimePicker: UIDatePicker!
    
    @IBAction func sdtNextBtn(sender: AnyObject)
    {
        var dateFormatter = NSDateFormatter()
        
        dateFormatter.dateStyle = NSDateFormatterStyle.MediumStyle
        dateFormatter.timeStyle = NSDateFormatterStyle.MediumStyle
        newReservation.startDateTime = dateFormatter.stringFromDate(startDateTimePicker.date)
    }
    
    
    /*
    
    
            END TIME SELECTION
    
    */
    
    @IBOutlet weak var endDateTimePicker: UIDatePicker!
    
    @IBAction func edtNextBtn(sender: AnyObject)
    {
        var dateFormatter = NSDateFormatter()
        dateFormatter.dateStyle = NSDateFormatterStyle.MediumStyle
        dateFormatter.timeStyle = NSDateFormatterStyle.MediumStyle
        newReservation.endDateTime = dateFormatter.stringFromDate(endDateTimePicker.date)
    }
    
    
    /*
    
    
            NAME LOCATION VIEW
    
    
    */
    
    @IBOutlet weak var reservationNameTextField: UITextField!
    
    @IBOutlet weak var reservationLocationTextField: UITextField!
    
    @IBAction func reviewReservationBtn(sender: AnyObject)
    {
        newReservation.titleOfReservation = reservationNameTextField.text
        newReservation.locationOfReservation = reservationLocationTextField.text
    }
    
    
    /*
    
            REVIEW / SAVE VIEW
    
    */
    
    @IBOutlet weak var reviewTextView: UITextView!
    
    @IBAction func submitReservationBtn(sender: AnyObject)
    {
        var saved = newReservation.saveCurrentReservation()
        
        var savedCoreData = saveToCoreDataEntity()
        
        if saved != -1 && savedCoreData == 1
        {
            var emailData = "\n Title : \(newReservation.titleOfReservation) \n Location : \(newReservation.locationOfReservation) \n Start Time :\n \(newReservation.startDateTime) \n End Date Time :\n \(newReservation.endDateTime) \n Items On Reservation : \n \(itemsSelectedString)"
            
            // Setup the activity view controller.
            let activityVC : UIActivityViewController = UIActivityViewController(activityItems:[emailData], applicationActivities: nil)
            
            // Present the view Controller
            presentViewController(activityVC, animated: true, completion: nil)
            
            // Show alert saying it was saved, and return to home screen when okay is pressed
            showCompletionAlert("Saved", message: "Your reservation has been submitted")
        }
        else
        {
            // Tell the user there was an issue saving the reservaton
            showErrorAlert("Error", message: "There was an error with your reservation.\n Did you fill out all of the fields ? ")
        }
    }
    
    @IBAction func doneBtn(sender: AnyObject)
    {}
    
    func saveToCoreDataEntity() -> Int
    {
        let Entity = "UserRequests"
        let ManagedContext = (UIApplication.sharedApplication().delegate as AppDelegate).managedObjectContext
        
        let check = NSFetchRequest(entityName: Entity)
        
        var results: NSArray = ManagedContext!.executeFetchRequest(check, error: nil)!
        
        // Something is in the data base, so iterate through it and make
        // sure that you are not adding it twice
        if( results.count != 0 )
        {
            for el in results
            {
                var ell : String  = el as String
                
                println(" -> \(ell)")
            }
        }
        // If nothing is in the database just add whatever is there.
        else
        {
          
        }
        
        return 1
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
    
    // Alerts
    
    func showDoneButton()
    {
        submitResBtn.hidden = true
        reviewTextView.hidden = true
        doneButton.hidden = false
    }
    
    func showReturnButton()
    {
        submitResBtn.hidden = true
        reviewTextView.hidden = true
        returnButton.hidden = false
    }
    
    func showErrorAlert(title: String, message: String)
    {
        newReservation.viewCounter = 0
        let allertContoller = UIAlertController(title: title, message: message, preferredStyle: .Alert)
        let defaultAction = UIAlertAction(title: "OK", style: .Default, handler: nil)
        allertContoller.addAction(defaultAction)
        presentViewController(allertContoller, animated: true, completion: showReturnButton)
    }
    
    func showCompletionAlert(title: String, message: String)
    {
        newReservation.viewCounter = 0
        let allertContoller = UIAlertController(title: title, message: message, preferredStyle: .Alert)
        let defaultAction = UIAlertAction(title: "OK", style: .Default, handler: nil)
        allertContoller.addAction(defaultAction)
        presentViewController(allertContoller, animated: true, completion: showDoneButton)
    }

    override func viewDidLoad()
    {
        // Item Selection
        tempInventory = inventoryManager.inventory
        
        // For determining when specific view is loaded
        newReservation.viewCounter++
        
        if ( newReservation.viewCounter == 5 )
        {
            // Review page now loaded
            doneButton.hidden = true
            returnButton.hidden = true
            
            for el in newReservation.itemsReserved
            {
                itemsSelectedString = itemsSelectedString + el.title + "\n"
            }
            
            reviewTextView.text = "\n Title : \(newReservation.titleOfReservation) \n Location : \(newReservation.locationOfReservation) \n Start Time :\n \(newReservation.startDateTime) \n End Date Time :\n \(newReservation.endDateTime) \n Items On Reservation : \n \(itemsSelectedString)"
        }
        
        super.viewDidLoad()
    }
    
    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
    }
}

