__author__ = 'Josh A. Bosley'

import astar
import random

def randomLine(fileName):
    count = 0
    with open(fileName) as infile:
        for line in infile:
            if line.strip():
                count += 1
        RanLine = random.randint(0, count)
        with open(fileName) as f:
            Choice = f.readlines()[RanLine - 1]
        return Choice;

def GeneGen():
    # Health - Strength - Intelligence
    stats = []
    stats.append(100)
    for XX in range(0,2):
        StatBox = (0)
        for X in range(0,10):
            statgen = (random.randint(0,1))
            StatBox = (StatBox + statgen)
        stats.append(StatBox)
    return(stats);

def ScrubList(dirtyList, dirt):
    cleanList = []
    for SPOT in range(len(dirtyList)):
        cleanData = dirtyList[SPOT].split(dirt)
        cleanList.append(cleanData[0])
    return(cleanList)

def GeneMaker():
    # Creates 10 lists containing 100 binary values each.
    setsOfDNA = 10
    sizeOfDNA = 100
    ListOfGenes = []
    Genes = []
    for Gene in range(sizeOfDNA):
        for Sets in range(setsOfDNA):
            geneInsert = random.randint(0,1)
            Genes.append(geneInsert)
        ListOfGenes.append(Genes)
    return(ListOfGenes);


def BuildStats(Gender):
    #  Health - Strength - Intelligence
    #  Males have higher strength, Females have higher Intelligence

    statsReturn = []

    if (Gender == 1):
        statsReturn.append(100)
        statsReturn.append(random.randint(5,10))
        statsReturn.append(random.randint(20, 95))

    if (Gender == 0):
        statsReturn.append(100)
        statsReturn.append(random.randint(1,6))
        statsReturn.append(random.randint(50, 95))

    return statsReturn

def moveUp(CurrentPosition, GridMax, GridMin):
    CurrX = CurrentPosition[1]
    CurrY = CurrentPosition[0]
    if CurrX != 1:
        NewPosition = (CurrY, CurrX-1)
        return NewPosition
    else:
        return CurrentPosition

def moveDown(CurrentPosition, GridMax, GridMin):
    CurrX = CurrentPosition[1]
    CurrY = CurrentPosition[0]
    if CurrX != GridMax:
        NewPosition = (CurrY, CurrX+1)
        return NewPosition
    else:
        return CurrentPosition

def moveLeft(CurrentPosition, GridMax, GridMin):
    CurrX = CurrentPosition[1]
    CurrY = CurrentPosition[0]
    if CurrY != GridMin:
        NewPosition = (CurrY-1, CurrX)
        return NewPosition
    else:
        return CurrentPosition

def moveRight(CurrentPosition, GridMax, GridMin):
    CurrX = CurrentPosition[0]
    CurrY = CurrentPosition[1]
    if CurrX != GridMax:
        NewPosition = (CurrX+1, CurrY)
        return NewPosition
    else:
        return CurrentPosition

        # DIAGINAL MOVEMENT
def moveRightUP(CurrentPosition, GridMax, GridMin):
    CurrX = CurrentPosition[1]
    CurrY = CurrentPosition[0]
    if CurrX != GridMin and CurrY != GridMax:
        NewPosition = (CurrY+1, CurrX-1)
        return NewPosition
    else:
        return CurrentPosition

def moveRightDOWN(CurrentPosition, GridMax, GridMin):
    CurrX = CurrentPosition[1]
    CurrY = CurrentPosition[0]
    if CurrX != GridMax and CurrY != GridMax:
        NewPosition = (CurrY+1, CurrX+1)
        return NewPosition
    else:
        return CurrentPosition

def moveLeftUP(CurrentPosition, GridMax, GridMin):
    CurrX = CurrentPosition[1]
    CurrY = CurrentPosition[0]
    if CurrX != GridMin and CurrY != GridMin:
        NewPosition = (CurrY-1, CurrX-1)
        return NewPosition
    else:
        return CurrentPosition

def moveLeftDOWN(CurrentPosition, GridMax, GridMin):
    CurrX = CurrentPosition[1]
    CurrY = CurrentPosition[0]
    if CurrX != GridMax and CurrY != GridMin:
        NewPosition = (CurrY-1, CurrX+1)
        return NewPosition
    else:
        return CurrentPosition

def DecisionMaker(grid, sizeGridX, sizeGridY, stats, currPosition, goal):

    if ':' in goal :
        goalSlice = goal.split(':')
        goal  = goalSlice[0]
        goalY = goalSlice[1]
        goalX = goalSlice[2]
        currY = currPosition[1]
        currX = currPosition[0]

    if goal == 'MoveAround':
        return goal

    if goal == 'locateGreenBlock':
        path = astar.BeginAstar(goalX, goalY, grid, sizeGridX, sizeGridY, currPosition, goal);
        if isinstance(path, basestring):   # Python 2.X | isinstance(path, str) # Python 3.X
            path = list(path)
            path.append('*')
            return path

    Health		 = stats[0]
    Strength 	 = stats[1]
    Intelligence = stats[2]

    return 'MoveAround'

def GrabAMove():
    move = random.randint(0,3)
    if move == 0 :
        return 'u'
    if move == 1 :
        return 'd'
    if move == 2 :
        return 'l'
    if move == 3 :
        return 'r'


def CommandLineInterface(data, DotianList):

    ri = '-'
    if data == '#':
        print ' For help, type: ? '

    commandsList = '''\n\n Kill Dotian : kill dotian-id \n Exit CLI : q \n Terminate : stop \n Reset World : reset
 \n Spawn Avatar: spawn avatar \n\n'''

    ri = raw_input(' >  ')

    if ri == '?':
        print commandsList
        CommandLineInterface('*', DotianList)

    if ri == 'q':
        return

    if ri == 'stop':
        return 'stop'

    if ri == 'reset':
        return 'reset'

    if 'kill' in ri:
        x = ri.split(" ")
        if len(x) > 3 or int(x[1]) > len(DotianList)-1:
            print 'Invalid Kill Command'
            CommandLineInterface('*', DotianList)
        else:
            return x

    if 'spawn' in ri:
        x = ri.split(" ")
        if len(x) > 3:
            print 'Invalid Spawn Command'
            CommandLineInterface('*', DotianList)
        else:
            return x

    else:
        print '\n There was an issue with the last command. \n'
        CommandLineInterface('#', DotianList)