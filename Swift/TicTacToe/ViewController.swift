//
//  ViewController.swift
//  joshtictactoe
//
//  Created by Josh Allen Bosley on 9/14/14.
//  Copyright (c) 2014 Josh Allen Bosley. All rights reserved.
//

import UIKit

class ViewController: UIViewController
{
    var debug = true
    
    enum players: Int
    {
        case computer = 0, human = 1
    }
    
    var board = [Int:Int]()

    @IBOutlet var img1: UIImageView!
    @IBOutlet var img2: UIImageView!
    @IBOutlet var img3: UIImageView!
    @IBOutlet var img4: UIImageView!
    @IBOutlet var img5: UIImageView!
    @IBOutlet var img6: UIImageView!
    @IBOutlet var img7: UIImageView!
    @IBOutlet var img8: UIImageView!
    @IBOutlet var img9: UIImageView!
    
    @IBOutlet var resetBtn: UIButton!
    @IBOutlet var userMessage: UILabel!
    
    var ticTacImages = [UIImageView]()
    
    var done = false, aiWorking = false
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        
        // Setup images for tap recognition
        ticTacImages = [img1, img2, img3,
                        img4, img5, img6,
                        img7, img8, img9]
        
        for item in ticTacImages
        {
            item.userInteractionEnabled = true
            item.addGestureRecognizer(UITapGestureRecognizer(target: self, action: "imageClicked:"))
        }
    }

    // Image Clicked Function [ PRETTY MUCH THE MAIN BODY ]
    func imageClicked(reco: UITapGestureRecognizer) {
        
        var imageViewTapped = reco.view as UIImageView
        
        // -1 Because tags are 1 - 9, and the board is 0 - 8
        
        if board[imageViewTapped.tag-1] == nil && !aiWorking && !done
        {
            setImageForSpot(imageViewTapped.tag-1, player:.human)
        }
        
        checkForWin()
        
        if ( !done)
        {
            doAiTurn()
        }
    }
    
    
    // Sets Image For Selected Spot
    func setImageForSpot(spot:Int, player: players)
    {
        var playerMark = player == .human ? "x" : "o"
        board[spot] = player.toRaw()
        ticTacImages[spot].image = UIImage(named: playerMark)
    }
    
    
    // CHECK BOARD FOR WIN
    func checkForWin()
    {
        
        var whoWon = ["I":0,"YOU":1]
        
        // THIS IS A REALLY STUPID WORKAROUND FOR NOT BEING ABLE TO PUT MULTIPLE &&s
        
        for (key, value) in whoWon
        {
            if ( board[0] == value && board[1] == value && board[2] == value )
            {
                doOrWorkAround( key )
            }
            if ( board[3] == value && board[4] == value && board[5] == value )
            {
                doOrWorkAround( key )
            }
            if ( board[6] == value && board[7] == value && board[8] == value )
            {
                doOrWorkAround( key )
            }
            if ( board[0] == value && board[3] == value && board[6] == value )
            {
                doOrWorkAround( key )
            }
            if ( board[1] == value && board[4] == value && board[7] == value )
            {
                doOrWorkAround( key )
            }
            if ( board[2] == value && board[5] == value && board[8] == value )
            {
                doOrWorkAround( key )
            }
            if ( board[0] == value && board[4] == value && board[8] == value )
            {
                doOrWorkAround( key )
            }
            if ( board[2] == value && board[4] == value && board[6] == value )
            {
                doOrWorkAround( key )
            }
        }
        
        if ( board.count == 9)
        {
            userMessage.text = "Looks Like we Tied !"
            userMessage.hidden = false
            resetBtn.hidden = false
            done = true
        }
        
    
    }
    
    func doOrWorkAround(person: String)
    {
        userMessage.text = "Looks like \(person) won !"
        userMessage.hidden = false
        resetBtn.hidden = false
        done = true
    }
    
    @IBAction func resetClicked(sender: AnyObject)
    {
        board = [:]
        done = false
        userMessage.hidden = true
        resetBtn.hidden = true
        
        for image in ticTacImages
        {
            image.image = nil
        }
    }
    
    func doAiTurn()
    {
        aiWorking = true
        
        // Determine if can win this move
        // If not, can the human be blocked ? 
        // If not, or unessicary ( human moved once ) pick random spot of available
        let theHuman = players.human.toRaw()
        let hal = players.computer.toRaw()
        
        // DEFENSE
        // Check to see if the human is one move away from winning
        // HORIZONTAL MOVES
        
        // Two accross the top - Block in top right corner
        if ( board[0] == theHuman && board[1] == theHuman && aiWorking && board[2] != hal)
        {
            setImageForSpot( 2 , player:.computer )
            aiWorking = false
        }
        
        // Two accross the top - Block in top left corner
        if ( board[1] == theHuman && board[2] == theHuman && aiWorking && board[0] != hal)
        {
            setImageForSpot( 0 , player:.computer )
            aiWorking = false
        }
        
        // Two accross the middle - Block in middle right
        if ( board[3] == theHuman && board[4] == theHuman && aiWorking && board[5] != hal)
        {
            setImageForSpot( 5 , player:.computer )
            aiWorking = false
        }
        
        // Two accross the middle - Block in middle left
        if ( board[4] == theHuman && board[5] == theHuman && aiWorking && board[3] != hal)
        {
            setImageForSpot( 3 , player:.computer )
            aiWorking = false
        }
        
        // Two accross the botttom - Block in bottom right corner
        if ( board[6] == theHuman && board[7] == theHuman && aiWorking && board[8] != hal)
        {
            setImageForSpot( 8 , player:.computer )
            aiWorking = false
        }
        
        // Two accross the bottom - Block in top bottom corner
        if ( board[7] == theHuman && board[8] == theHuman && aiWorking && board[6] != hal)
        {
            setImageForSpot( 6 , player:.computer )
            aiWorking = false
        }
        
        
        // VERTICAL MOVES
        
        // Left Column Bottom Two - Block In top most block
        if ( board[6] == theHuman && board[3] == theHuman && aiWorking && board[0] != hal)
        {
            setImageForSpot( 0 , player:.computer )
            aiWorking = false
        }
        
        // Left Column Top Two - Block In bottom most block
        if ( board[0] == theHuman && board[3] == theHuman && aiWorking && board[6] != hal)
        {
            setImageForSpot( 6 , player:.computer )
            aiWorking = false
        }
        
        // Middle Column Bottom Two - Block In top most block
        if ( board[4] == theHuman && board[7] == theHuman && aiWorking && board[1] != hal)
        {
            setImageForSpot( 1 , player:.computer )
            aiWorking = false
        }
        
        // Middle Column Top Two - Block In bottom most block
        if ( board[1] == theHuman && board[4] == theHuman && aiWorking && board[7] != hal)
        {
            setImageForSpot( 7 , player:.computer )
            aiWorking = false
        }
        
        // Roght Column Bottom Two - Block In top most block
        if ( board[5] == theHuman && board[8] == theHuman && aiWorking && board[2] != hal)
        {
            setImageForSpot( 2 , player:.computer )
            aiWorking = false
        }
        
        // Right Column Top Two - Block In bottom most block
        if ( board[2] == theHuman && board[5] == theHuman && aiWorking && board[8] != hal)
        {
            setImageForSpot( 8 , player:.computer )
            aiWorking = false
        }
        
        
        // DIAGONAL MOVES
        
        // Top Left to bottom right
        if ( board[0] == theHuman && board[4] == theHuman && aiWorking && board[8] != hal)
        {
            setImageForSpot( 8 , player:.computer )
            aiWorking = false
        }
        
        // Top Right to bottom left
        if ( board[2] == theHuman && board[4] == theHuman && aiWorking  && board[6] != hal )
        {
            setImageForSpot( 6 , player:.computer )
            aiWorking = false
        }
        
        // Bottom Left to top right && board[] != hal
        if ( board[6] == theHuman && board[4] == theHuman && aiWorking && board[2] != hal)
        {
            setImageForSpot( 2 , player:.computer )
            aiWorking = false
        }
        
        // Bottom Right to top left
        if ( board[8] == theHuman && board[4] == theHuman && aiWorking && board[0] != hal)
        {
            setImageForSpot( 0 , player:.computer )
            aiWorking = false
        }
        
        
        // OFFENSE
        
        // HORIZONTAL MOVES
        
        // Two accross the top - place in top right corner
        if ( board[0] == hal && board[1] == hal && aiWorking && board[2] != theHuman)
        {
            setImageForSpot( 2 , player:.computer )
            aiWorking = false
        }
        
        // Two accross the top - Block in top left corner
        if ( board[1] == hal && board[2] == hal && aiWorking && board[0] != theHuman)
        {
            setImageForSpot( 0 , player:.computer )
            aiWorking = false
        }
        
        // Two accross the middle - Block in middle right
        if ( board[3] == hal && board[4] == hal && aiWorking && board[5] != theHuman)
        {
            setImageForSpot( 5 , player:.computer )
            aiWorking = false
        }
        
        // Two accross the middle - Block in middle left
        if ( board[4] == hal && board[5] == hal && aiWorking && board[3] != theHuman)
        {
            setImageForSpot( 3 , player:.computer )
            aiWorking = false
        }
        
        // Two accross the botttom - Block in bottom right corner
        if ( board[6] == hal && board[7] == hal && aiWorking && board[8] != theHuman)
        {
            setImageForSpot( 8 , player:.computer )
            aiWorking = false
        }
        
        // Two accross the bottom - Block in top bottom corner
        if ( board[7] == hal && board[8] == hal && aiWorking && board[6] != theHuman)
        {
            setImageForSpot( 6 , player:.computer )
            aiWorking = false
        }
        
        
        // VERTICAL MOVES
        
        // Left Column Bottom Two - Block In top most block
        if ( board[6] == hal && board[3] == hal && aiWorking && board[0] != theHuman)
        {
            setImageForSpot( 0 , player:.computer )
            aiWorking = false
        }
        
        // Left Column Top Two - Block In bottom most block
        if ( board[0] == hal && board[3] == hal && aiWorking && board[6] != theHuman)
        {
            setImageForSpot( 6 , player:.computer )
            aiWorking = false
        }
        
        // Middle Column Bottom Two - Block In top most block
        if ( board[4] == hal && board[7] == hal && aiWorking && board[1] != theHuman)
        {
            setImageForSpot( 1 , player:.computer )
            aiWorking = false
        }
        
        // Middle Column Top Two - Block In bottom most block
        if ( board[1] == hal && board[4] == hal && aiWorking && board[7] != theHuman)
        {
            setImageForSpot( 7 , player:.computer )
            aiWorking = false
        }
        
        // Roght Column Bottom Two - Block In top most block
        if ( board[5] == hal && board[8] == hal && aiWorking && board[2] != theHuman)
        {
            setImageForSpot( 2 , player:.computer )
            aiWorking = false
        }
        
        // Right Column Top Two - Block In bottom most block
        if ( board[2] == hal && board[5] == hal && aiWorking && board[8] != theHuman)
        {
            setImageForSpot( 8 , player:.computer )
            aiWorking = false
        }
        
        // DIAGONAL MOVES
        
        // Top Left to bottom right
        if ( board[0] == hal && board[4] == hal && aiWorking && board[8] != theHuman)
        {
            setImageForSpot( 8 , player:.computer )
            aiWorking = false
        }
        
        // Top Right to bottom left
        if ( board[2] == hal && board[4] == hal && aiWorking  && board[6] != theHuman )
        {
            setImageForSpot( 6 , player:.computer )
            aiWorking = false
        }
        
        // Bottom Left to top right && board[] != hal
        if ( board[6] == hal && board[4] == hal && aiWorking && board[2] != theHuman)
        {
            setImageForSpot( 2 , player:.computer )
            aiWorking = false
        }
        
        // Bottom Right to top left
        if ( board[8] == hal && board[4] == hal && aiWorking && board[0] != theHuman)
        {
            setImageForSpot( 0 , player:.computer )
            aiWorking = false
        }

        
        // If no spots found by now, grab a random spot that is free.
        var res : Int
        while( aiWorking )
        {
            res = randomInt(0, max: 8)
            
            if ( board[res] != hal && board[res] != theHuman)
            {
                setImageForSpot( res , player:.computer )
                aiWorking = false
            }
            
            if ( debug )
            {
                println(res)
                println(board)
            }
        }
        
        checkForWin()
    }

    func randomInt(min: Int, max:Int) -> Int
    {
        return min + Int(arc4random_uniform(UInt32(max - min + 1)))
    }
    
    
    
    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

