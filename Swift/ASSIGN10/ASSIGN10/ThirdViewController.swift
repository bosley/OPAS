//
//  SecondViewController.swift
//  ASSIGN10
//
//  Created by Josh Allen Bosley on 10/1/14.
//  Copyright (c) 2014 Josh Allen Bosley. All rights reserved.
//

import UIKit

class ThirdViewController: UIViewController, UITextFieldDelegate
{
    var CAN = 1.12
    var USD = 0.90
 
    @IBOutlet var inField: UITextField!
    
    func showAlert(message: String, type: Int)
    {
        var moot : String
        
        if( 0 == type )
        {
                moot = "\(inField.text) $US = \(message) $CAN"
        }
        else
        {
                moot = "\(inField.text) $CAN = \(message) $US"
        }
        
        let allertController = UIAlertController(title: "Converted!", message: moot, preferredStyle: .Alert)
        
        let defaultAction = UIAlertAction(title: "OK", style: .Default, handler: nil)
        
        allertController.addAction(defaultAction)
        
        presentViewController(allertController, animated: true, completion: nil)
    }
    
    @IBAction func convertUStoCAN(sender: AnyObject)
    {
        var data = Double((inField.text as NSString).doubleValue)
        showAlert(NSString(format: "%.2f", (data * USD)),type: 0)
    }

    @IBAction func convertCANtoUS(sender: AnyObject)
    {
        var data = Double((inField.text as NSString).doubleValue)
        showAlert(NSString(format: "%.2f", (data * CAN)),type: 1)
    }
    
    override func touchesBegan(touches: NSSet, withEvent event: UIEvent)
    {
        inField.resignFirstResponder()
    }
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
    }
    
    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
    }
    
}

