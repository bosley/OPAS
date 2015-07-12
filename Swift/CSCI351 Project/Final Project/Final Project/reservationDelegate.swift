//
//  ViewController.swift
//  Final Project
//
//  Created by Josh Bosley, Stefan Hurthibise on 11/14/14.
//  Copyright (c) 2014 Josh Bosley. All rights reserved.
//

var reservationManger : reservationDelegate = reservationDelegate()

struct reservationStruct
{
    var id = ""
    var user = ""
    var title = ""
    var equipment = [String]()
    var startTime = ""
    var endTime = ""
}

class reservationDelegate : NSObject
{
    var reservations = [reservationStruct]()
 
    func populateReservations(updates : AnyObject)
    {
        
        if (updates[0] as String! != nil)
        {
            var data = updates[0] as String!
            var arrData = data.componentsSeparatedByString("%")
        
            for update in arrData
            {
                var cols = update.componentsSeparatedByString("|")
                
                if (cols[1] == currentUser)
                {
                    var equipmentIds = cols[3].componentsSeparatedByString(",")
                    
                    reservations.append( reservationStruct(id: cols[0], user: cols[1], title: cols[2], equipment: equipmentIds, startTime: cols[4], endTime: cols[5]) )
                }
            }
        }
        
        updateManager.setOriginalReservations(reservations)
        
        println("THERE ARE \(reservations.count) RESERVATIONS")
        
    }
}

