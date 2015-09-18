//
//  ViewController.swift
//  Interaction
//
//  Created by Josh Allen Bosley on 9/19/14.
//  Copyright (c) 2014 Josh Allen Bosley. All rights reserved.
//

import UIKit

class ViewController: UIViewController, UITextFieldDelegate
{
    
    @IBOutlet var theMainLabel: UILabel!
    
    @IBOutlet var sliderLabel: UILabel!
    
    @IBOutlet var nameField: UITextField!
    
    @IBOutlet var numf: UITextField!
    
    @IBOutlet var leftSwitch: UISwitch!
    
    @IBOutlet var rightSwitch: UISwitch!
    
    @IBOutlet var doSumthin: UIButton!
    
    @IBOutlet var sliderVar: UISlider!
    
    @IBAction func switchChanged(sender: UISwitch)
    {
        var setting = sender.on
        
        leftSwitch.on = setting
        rightSwitch.on = setting
    }
    
    
    func textFieldShouldReturn(textField: UITextField) -> Bool {
        
        nameField.resignFirstResponder()
        return true
    }
    
    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
        
        numf.resignFirstResponder()
    }
    
    
    
    @IBAction func leftClick(sender: AnyObject)
    {
        theMainLabel.text = "Left Button Was Clicked"
    }
    
    @IBAction func rightClick(sender: AnyObject)
    {
        theMainLabel.text = "Right Button Was Clicked"
    }
    
    
    @IBAction func sliderChanged(sender: AnyObject)
    {
        
        
        
        var progress = lroundf(sender.value)
        
        sliderLabel.text = String(progress)
    }
    
    
    
    @IBAction func segmentedContol(sender: UISegmentedControl)
    {
        if sender.selectedSegmentIndex == 0
        {
            leftSwitch.hidden = false
            rightSwitch.hidden = false
            doSumthin.hidden = true
        }
        
        if sender.selectedSegmentIndex == 1
        {
            leftSwitch.hidden = true
            rightSwitch.hidden = true
            doSumthin.hidden = false
        }
        
        
    }
    
    
    @IBAction func doSumthinClicked(sender: AnyObject)
    {
        
        let allertContoller = UIAlertController(title: "Hey CSCI351!", message: " Yall want a quiz ?", preferredStyle: .Alert)
        
        
        let defaultAction = UIAlertAction(title: "OK", style: .Default, handler: nil)
        
        allertContoller.addAction(defaultAction)
        
        presentViewController(allertContoller, animated: true, completion: nil)
        
        
    }
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        
        doSumthin.hidden = true
        nameField.delegate = self
        
        sliderVar.maximumValue = 100.00
        sliderVar.minimumValue = 0.00
    
    }

    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
    }


}

