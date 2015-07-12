//
//  newReservationDelegate.swift
//  Final Project
//
//  Created by Josh Bosley on 11/16/14.
//  Copyright (c) 2014 Josh Bosley. All rights reserved.
//

var newReservation : newReservationDelegate = newReservationDelegate()

class newReservationDelegate
{
    var viewCounter : Int = 0
    var itemsReserved = [inventoryStruct]()
    var startDateTime : String = "NULL"
    var endDateTime : String = "NULL"
    var titleOfReservation : String = "NULL"
    var locationOfReservation : String = "NULL"
    
    func saveCurrentReservation() -> Int
    {
        if itemsReserved.count > 0
        {
            if startDateTime != "NULL" && endDateTime != "NULL"
            {
                if titleOfReservation != "NULL"
                {
                    if locationOfReservation != "NULL"
                    {
                        var newTitle = locationOfReservation + " - " +  titleOfReservation
                        
                        var equipmentList = [String]()
                        for el in itemsReserved
                        {
                            equipmentList.append(el.id)
                        }
                        
                        var newRes : reservationStruct = reservationStruct(id: "NEW",
                            user: currentUser,
                            title: newTitle,
                            equipment: equipmentList,
                            startTime: startDateTime,
                            endTime: endDateTime
                            )
                        
                        // Set reservation, and add to updates
                        reservationManger.reservations.append(newRes)
                        
                        updateManager.setUpdatedReservations(reservationManger.reservations)
                        
                        // Reset this class for reuse
                        self.viewCounter = 0
                        self.itemsReserved = [inventoryStruct]()
                        self.startDateTime = "NULL"
                        self.endDateTime = "NULL"
                        self.titleOfReservation = "NULL"
                        self.locationOfReservation = "NULL"
                        return 1
                    }
                }
            }
        }
        return -1
    }
}
