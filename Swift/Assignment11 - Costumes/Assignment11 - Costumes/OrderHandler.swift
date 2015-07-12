//
//  File.swift
//  Assignment11 - Costumes
//
//  Created by Josh Allen Bosley on 10/5/14.
//  Copyright (c) 2014 Lssu. All rights reserved.
//

import Foundation

// Globla for usage

var theUsersOrder = userOrder()


// A structure for each costume item order in total order

class orderBox
{
    var quantity : Int = 0
    var totalCost : Double = 0.0
    var costumeSize : String = ""
    var costumeOrdered : String = ""
    
    init( x : String, y : String, z : Int, d : Double)
    {
        costumeSize = x
        costumeOrdered = y
        quantity = z
    }
    
    func edit( x : String, y : String, z : Int, d : Double )
    {
        costumeSize = x
        costumeOrdered = y
        quantity = z
    }
}

// The current orders for the user

class userOrder
{
    var name : String = ""
    var isSet : Bool = false
    var totalOrderCost : Double = 0.0
    var totalCostumesOrdered : Int = 0
    var items = [orderBox]()
    
    init(){}
    
    func addItem ( theirSize : String, theirCostume : String, count : Int, cost : Double)
    {
        var tcost : Double = cost * Double(count)
        items.append( orderBox(x: theirSize, y: theirCostume, z: count, d : tcost) )
        totalCostumesOrdered += count
        totalOrderCost += tcost
    }
    
    func deleteItem ( index : Int )
    {
        totalOrderCost -= items[index].totalCost
        totalCostumesOrdered -= items[index].quantity
        items.removeAtIndex( index )
    }
    
    
    func reset()
    {
        
        totalOrderCost = 0.0
        totalCostumesOrdered = 0
        isSet = false
        name = ""
        items = [orderBox]()
        
    }
    
    
}











