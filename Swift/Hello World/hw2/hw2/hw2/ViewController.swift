//
//  ViewController.swift
//  hw2
//
//  Created by Josh Allen Bosley on 8/27/14.
//  Copyright (c) 2014 Josh Allen Bosley. All rights reserved.
//

import UIKit

class ViewController: UIViewController
{
    
    @IBOutlet var theLabel: UILabel?
    
    @IBAction func hideButton(sender: AnyObject)
    {
        theLabel?.hidden = true
    }
    
    @IBAction func showButton(sender: AnyObject)
    {
        theLabel?.hidden = false
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
