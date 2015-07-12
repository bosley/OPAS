//
//  updateServerDelegate.swift
//  Final Project
//
//  Created by Josh Bosley on 11/14/14.
//  Copyright (c) 2014 Josh Bosley. All rights reserved.
//

var updateManager : updatesMadeByUser = updatesMadeByUser()

class updatesMadeByUser
{
    /*
    
            THE PURPOSE OF THIS CLASS IS TO ENSURE THAT ALL DELETIONS / UPDATES
            MADE BY THE USER ARE SAVED TO THE SERVER. SEPERATED INTO ITS OWN CLASS
            TO ENSURE THAT ALL DATA IS SAVED, AS WELL AS MAKING SURE THAT THE CODE 
            REMAINS NEAT
                                                            - Josh Bosley
    */

    var ensureSingleSet : Bool = true
    var originalReservations = [reservationStruct]()
    var updatedReservations = [reservationStruct]()

    func setOriginalReservations( resArray : [reservationStruct] )
    {
        if ( ensureSingleSet)
        {
            originalReservations = resArray
            ensureSingleSet = false
        }
    }
    
    func setUpdatedReservations( resArray : [reservationStruct] )
    {
        updatedReservations = resArray
    }
}