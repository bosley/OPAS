//
//  ViewController.swift
//  PayrollProcessing
//
//  Created by Josh Allen Bosley on 9/5/14.
//  Copyright (c) 2014 Josh Allen Bosley. All rights reserved.
//

import UIKit

class ViewController: UIViewController
{
    
    @IBOutlet weak var nameField: UITextField!
    
    @IBOutlet weak var hoursField: UITextField!
    
    @IBOutlet weak var rateField: UITextField!
    
    @IBOutlet weak var resultLabel: UILabel!
    
    @IBAction func computePay(sender: AnyObject)
    {
        var hours = hoursField.text.toInt()!
        var rate = rateField.text.toInt()!
        var result = hours * rate
        resultLabel.text = nameField.text + " " + String(result)
        resultLabel.hidden = false
    }
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

