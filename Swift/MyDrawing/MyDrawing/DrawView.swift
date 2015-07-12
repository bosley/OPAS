//
//  DrawView.swift
//  MyDrawing
//
//  Created by Josh Allen Bosley on 10/8/14.
//  Copyright (c) 2014 Lssu. All rights reserved.
//

import UIKit

class DrawView: UIView
{
    var lines: [Line] = []
    var lastPoint: CGPoint!
    var drawColor = UIColor.blackColor()
    
    
    
    
    override func touchesBegan(touches: NSSet, withEvent event: UIEvent)
    {
        lastPoint = touches.anyObject()?.locationInView(self)
    }

    override func touchesMoved(touches: NSSet, withEvent event: UIEvent)
    {
        var newPoint = touches.anyObject()?.locationInView(self)
        lines.append(Line(start: lastPoint, end: newPoint!, color: drawColor))
        lastPoint = newPoint
        self.setNeedsDisplay()
    }

    required init(coder aDecoder: NSCoder)
    {
        super.init(coder: aDecoder)
        // Initialization Code here
        // seld.backgroundColor = UIColor.blackColor()
    }

    
    // Only override drawRect: if you perform custom drawing.
    // An empty implementation adversely affects performance during animation.
    override func drawRect(rect: CGRect)
    {
        // Drawing code
        
        var context = UIGraphicsGetCurrentContext()
        CGContextSetLineCap(context, kCGLineCapRound)
        CGContextSetLineWidth(context, 5)
        
        for line in lines
        {
            CGContextBeginPath(context)
            CGContextMoveToPoint(context, line.startX, line.startY)
            CGContextAddLineToPoint(context, line.endX, line.endY)
            CGContextSetStrokeColorWithColor(context, line.color.CGColor)
            CGContextStrokePath(context)
        }
        
        
    }
}
