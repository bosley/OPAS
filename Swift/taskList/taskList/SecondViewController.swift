//
//  SecondViewController.swift
//  taskList
//
//  Created by Josh Allen Bosley on 9/22/14.
//  Copyright (c) 2014 Josh Allen Bosley. All rights reserved.
//

import UIKit

class SecondViewController: UIViewController, UITextFieldDelegate
{
    @IBOutlet var txtTask: UITextField!
    @IBOutlet var txtDesc: UITextField!
    
    
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

    
    // Touch Stuff
    func textFieldShouldReturn(textField: UITextField) -> Bool
    {
        textField.resignFirstResponder()
        return true
    }
    
    override func touchesBegan(touches: NSSet, withEvent event: UIEvent)
    {
        
        self.view.endEditing(true)
    }
    
    // Button Click Handle
    @IBAction func btnAddTask(sender: AnyObject)
    {
        taskMgr.addTask(txtTask.text, desc: txtDesc.text)
        self.view.endEditing(true)
        txtTask.text = " "
        txtDesc.text = " "
        self.tabBarController?.selectedIndex = 0
    }
    


}

