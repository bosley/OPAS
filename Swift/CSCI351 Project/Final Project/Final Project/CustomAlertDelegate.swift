//
//  CustomAlertDelegate.swift
//  Final Project
//
//  Created by Josh Bosley on 11/14/14.
//  Copyright (c) 2014 Josh Bosley. All rights reserved.
//

import UIKit
import Foundation

var GlobalAlerts = ProjectAlerts()

class ProjectAlerts : UIViewController
{
    var standardAlertTitle = ""
    var standardAlertMessage = ""
    
    func showStnadardAlert()
    {
        let allertContoller = UIAlertController(title: standardAlertTitle, message: standardAlertMessage, preferredStyle: .Alert)
        let defaultAction = UIAlertAction(title: "OK", style: .Default, handler: nil)
        allertContoller.addAction(defaultAction)
        presentViewController(allertContoller, animated: true, completion: nil)
    }
}