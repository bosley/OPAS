//
//  ViewController.swift
//  objCClient
//
//  Created by Josh Allen Bosley on 10/31/14.
//  Copyright (c) 2014 Lssu. All rights reserved.
//

import UIKit

class ViewController: UIViewController
{
    
    @IBOutlet var textArea: UITextView!

    var JBSocks = JBSocketDelegate()
    var serverWaitTimer = NSTimer()
    var deviceString : AnyObject!
    var updateString : AnyObject!
    
    func getServerData()
    {
        JBSocks.setDataRetrieved()
        deviceString = JBSocks.allDataFromSocket[1];
        updateString = JBSocks.allDataFromSocket[2];
        
        textArea.text = "Devices : \(deviceString) \n"
        textArea.text = "\(textArea.text) Updates : \(updateString) \n"
        
        serverWaitTimer.invalidate()
    }

    override func viewDidLoad()
    {
        super.viewDidLoad()
        
        JBSocks.makeRequestForDevices()
        JBSocks.makeRequestForUpdates()
        serverWaitTimer = NSTimer.scheduledTimerWithTimeInterval(1, target: self, selector: Selector("getServerData"), userInfo: nil, repeats: true)
    }

    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
    }
}

