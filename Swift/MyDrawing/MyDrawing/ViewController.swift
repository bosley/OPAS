//
//  ViewController.swift
//  MyDrawing
//
//  Created by Josh Allen Bosley on 10/8/14.
//  Copyright (c) 2014 Lssu. All rights reserved.
//

import UIKit

class ViewController: UIViewController
{
    
    @IBOutlet var drawView: DrawView!
    
    @IBAction func clearTapped(sender: AnyObject)
    {
        var theDrawView = drawView as DrawView
        theDrawView.lines = []
        theDrawView.setNeedsDisplay()
    }
    
    @IBAction func colorTapped(button: UIButton!)
    {
        var theDrawView = drawView as DrawView
        
        var color: UIColor!
        
        if( button.titleLabel?.text == "RED")
        {
            color = UIColor.redColor()
        }
        else if ( button.titleLabel?.text == "BLACK")
        {
            color = UIColor.blackColor()
        }
        theDrawView.drawColor = color
        
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
