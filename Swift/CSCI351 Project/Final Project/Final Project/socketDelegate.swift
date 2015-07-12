//
//  socketDelegate.swift
//
//  Used as a middle man between socket connection made
//  by objective C class and main project
//
//
//  Created by Josh Allen Bosley on 11/6/14.
//  Copyright (c) 2014 Lssu. All rights reserved.
//

import Foundation

var sockItToMe = JBSocketDelegate()
class JBSocketDelegate
{
    var jbSocket = socketClass()
    var allDataFromSocket : NSMutableArray = []
    var ranServerCall = false
    init()
    {
        jbSocket.initializeSocketClass()
        jbSocket.connectToServer()
    }
    
    func makeRequestForDevices()
    {
        jbSocket.sendRequestForDevices()
    }
    
    func makeRequestForUpdates()
    {
        jbSocket.sendRequestForUpdates()
    }
    
    @objc func setDataRetrieved()
    {
        allDataFromSocket = jbSocket.getAllRetrievedData()
        ranServerCall = true
    }
}

