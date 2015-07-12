//
//  ViewController.swift
//  Final Project
//
//  Created by Josh Bosley, Stefan Hurthibise on 11/14/14.
//  Copyright (c) 2014 Josh Bosley. All rights reserved.
//

import UIKit
import Foundation

class CheckReservationController: UIViewController, UITableViewDelegate, UITableViewDataSource
{
    
    @IBOutlet weak var tableView: UITableView!
    
    @IBOutlet weak var saveButton: UIButton!
    
    var editableArray = [reservationStruct]()
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int
    {
        return editableArray.count
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell
    {
        let cell: UITableViewCell = UITableViewCell( style: UITableViewCellStyle.Subtitle, reuseIdentifier: "GOAAT")
        
        // Add equipment from selected row into a sstring
        var equipmentString = ""
        
        for item in reservationManger.reservations[indexPath.row].equipment
        {
            
            // GET NAME BY ID HERE INSTEAED OF IN LOAD
            
            equipmentString += inventoryManager.retrieveTitleById(item)
            
            if item != editableArray[indexPath.row].equipment[editableArray[indexPath.row].equipment.count-1]
            {
                equipmentString += ", "
            }
        }
        
        // Take that string, and show only up to 10 chars
        var count  = 0;
        var showString = ""
        for charachter in equipmentString
        {
            count++
            showString += String(charachter)
        }
        showString += " ... "
        
        // String length shown will be 15 chars
        cell.textLabel.text = editableArray[indexPath.row].title
        cell.detailTextLabel?.text = showString
        return cell
    }
    
    func tableView(tableView: UITableView, commitEditingStyle editingStyle: UITableViewCellEditingStyle, forRowAtIndexPath indexPath: NSIndexPath)
    {
        if ( editingStyle == UITableViewCellEditingStyle.Delete)
        {
            editableArray.removeAtIndex(indexPath.row)
            tableView.reloadData()
            saveButton.enabled = true
        }
    }
    
    // Save Button
    @IBAction func saveButtonClicked(sender: AnyObject)
    {
        reservationManger.reservations = editableArray
        
        // Set the update manager's data to what the updated to
        updateManager.setUpdatedReservations(reservationManger.reservations)
        
        saveButton.enabled = false
        self.view.endEditing(true)
        self.tabBarController?.selectedIndex = 0
    }
    
    override func viewDidLoad()
    {
        if editableArray.count != reservationManger.reservations.count
        {
            editableArray = reservationManger.reservations
        }
        saveButton.enabled = false
        super.viewDidLoad()
    }
    
    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
    }
    
}

