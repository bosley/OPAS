//
//  inventoryDelegate.swift
//  Final Project
//
//  Created by Josh Bosley on 11/14/14.
//  Copyright (c) 2014 Josh Bosley. All rights reserved.
//

var inventoryManager : inventoryDelegate = inventoryDelegate()

struct inventoryStruct
{
    var id = ""
    var title = ""
    var description = ""
    var barcode = ""
}

class inventoryDelegate : NSObject
{
    var inventory = [inventoryStruct]()
    
    func populateInventory(devices : AnyObject)
    {
        if (devices[0] as String! != nil)
        {
            var data = devices[0] as String!
            var arrData = data.componentsSeparatedByString("%")
            
            for device in arrData
            {
                var cols = device.componentsSeparatedByString("|")
                inventory.append(inventoryStruct(id: cols[0].stringByTrimmingCharactersInSet(NSCharacterSet.whitespaceAndNewlineCharacterSet()),
                    title: cols[1],
                    description: cols[2],
                    barcode: cols[3]))
            }
        }
        
        println(" THERE ARE \(inventory.count) DEVICES. ")
    }
    
    func retrieveTitleById( id: String ) -> String
    {
        id.stringByTrimmingCharactersInSet(NSCharacterSet.whitespaceAndNewlineCharacterSet())
        
        for el in inventory
        {
            if el.id == id
            {
                return el.title
            }
        }
        // Not Found
        return "-1"
    }
    
}
