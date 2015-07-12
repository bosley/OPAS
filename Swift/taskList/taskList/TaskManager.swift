//
//  TaskManager.swift
//  taskList
//
//  Created by Josh Allen Bosley on 9/22/14.
//  Copyright (c) 2014 Josh Allen Bosley. All rights reserved.
//

import UIKit

// Global var for use in the view controllers
var taskMgr : TaskManager = TaskManager()


struct task
{
    var name = "Un-Named"
    var desc = "Un-Described"
}


class TaskManager: NSObject
{
    var tasks = [task]()
   
    func addTask(name: String, desc: String)
    {
        tasks.append(task(name: name, desc: desc))
    }
}