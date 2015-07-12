//
//  SecondViewController.swift
//  ASSIGN10
//
//  Created by Josh Allen Bosley on 10/1/14.
//  Copyright (c) 2014 Josh Allen Bosley. All rights reserved.
//

import UIKit

class SecondViewController: UIViewController {

    @IBOutlet var webV: UIWebView!
    
    var theURL = "http://en.wikipedia.org/wiki/Sault_Ste._Marie,_Ontario"
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        
        let theRequestURL = NSURL(string: theURL )
        let theRequest = NSURLRequest(URL: theRequestURL)
        webV.loadRequest(theRequest)
    }

    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
    }
}

