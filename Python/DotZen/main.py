__author__ = 'Josh A. Bosley'

import moduleNest as mod
import random
import pygame


class DotAvatar():
    position = (0, 0)
    visible = False
    points = 0
    color = 6


class SpawnObject(object):

    color = 0
    position = (0, 0)
    objType = ''

    def __init__(self, name):

        self.name = name
        if(self.name == ('GreenBlock')):
            color = (2)
            objType = ('GB')


class Dotian(object):

    #  Location information and color / greet info
    NewPosition = (0, 0)
    StartPosition = (0, 0)
    CurrentPosition = (0, 0)
    GreetingChosen = ''
    DotianColor = 2
    points = 0
    dotId = -1

    #  Path / Gender / Stats
    AdventureLog = []
    DotianStats = []
    GenderRoll = 0

    # Prime Directive
    CurrentGoal = ('MoveAround')
        
    def __init__(self, name):

        self.name = name
        self.GenderRoll = random.randint(0,1)
        print(mod.randomLine('data/lib/words/greetings'))


class Doatia(object):


    def __init__(self, name):

        # Set Needed Values
        done = False
        Started = False
        blockAlive = False
        enhancedBoogy = False
        blue     = (   0,   0, 255)
        red		 = ( 255,	0,	 0)
        black    = (   0,   0,   0)
        white    = ( 255, 255, 255)
        green    = (   0, 255,   0)
        cyan 	 = ( 153, 255, 255)
        skyblue  = (   0, 127, 255)
        pink 	 = ( 255,   0, 127)
        sizeGridX = (20)
        sizeGridY = (20)
        GridMin = 1
        GridMax = (sizeGridX-2)
        width  = (20)
        height = (20)
        mouseSpawnLimit = 5
        mouseSpawnCount = 0
        GlobalGoalX = (0)
        GlobalGoalY = (0)
        NewDotianList = []
        genderCensus = []
        objectList = []
        scoreboard = []
        statCensus = []
        census = []
        grid = []

        Avatar = DotAvatar()    # User Interaction

        inputCommands = (''' F1 - Clear Screen \n 1  - Help \n 2  - Census \n 3  - Give Boogy \n
 4  - Take Boogy \n\n Interface: \n\n 9 - Spawn green block \n\n C - Command Line Interface \n''')

        # ---- Create Grid
        for row in range(sizeGridX):
            grid.append([])
            for column in range(sizeGridY):
                grid[row].append(0)

        # ---- Generate Boundaries
        for Gen in range(0,sizeGridX):
            grid[0][Gen] = 9
            grid[Gen][0] = 9
            grid[sizeGridX-1][Gen] = 9
            grid[Gen][sizeGridX-1] = 9

        # ---- Initialize and setup
        pygame.init()
        size = (400, 400)
        screen=pygame.display.set_mode(size)
        pygame.display.set_caption(" Dot Zen ")
        clock = pygame.time.Clock()

        # -------- Event Handling, start main loop
        while not done:

            # fILL SCREEN, AND SETUP QUIT
            screen.fill(black)
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    done = True

                if event.type == pygame.KEYDOWN:

                    # Display possible commands
                    if(event.key == pygame.K_1):
                        for sclean in range(0,25): print('\n')
                        print(inputCommands)

                    # Dispaly Census
                    if(event.key == pygame.K_2):
                        for cls in range(0,25):
                            print('\n')
                        print('  | Dotian Census | ')
                        print('  | Dot ID : Name : Gender : Stats ')
                        print('  | [ Health (100), Strength(10), Intelligence(100) ]\n')
                        for name in range(len(NewDotianList)):
                            outName = census[name].replace('\n', '')
                            outStats = statCensus[name]
                            outGender = genderCensus[name]
                            print '''  | %s : %s : %s : %s''' % (NewDotianList[name].dotId, outName, outGender, outStats)
                        print('\n')

                    # Give boogy
                    if event.key == pygame.K_3:
                        if enhancedBoogy == False : enhancedBoogy = True

                    # Take the boogy
                    if event.key == pygame.K_4:
                        if enhancedBoogy:
                            enhancedBoogy = False

                    if not enhancedBoogy:
                        for ID in range(len(NewDotianList)):
                            if NewDotianList[ID].GenderRoll == 0 :
                                NewDotianList[ID].DotianColor = 4
                            if NewDotianList[ID].GenderRoll == 1 :
                                NewDotianList[ID].DotianColor = 1

                    # - Generate Goal

                    if(event.key == pygame.K_9) and (blockAlive == False):
                        blockAlive = True
                        randX = random.randint(1,GridMax)
                        goal = (0,0)
                        randY = random.randint(1,GridMax)
                        if (grid[randY][randX] != 2) and (grid[randX][randY] != 1) and (grid[randX][randY] != 4):

                            grid[randY][randX] = (2)
                            goal = ('locateGreenBlock:%s:%s') % (randX, randY)
                            NewBlock = SpawnObject('GreenBlock')
                            NewBlock.position = (randX,randY)
                            objectList.append(NewBlock)
                            GlobalGoalX = randX
                            GlobalGoalY = randY

                        # Change every Dotians prime directive to getting the block
                        for ID in range(len(NewDotianList)):
                            NewDotianList[ID].CurrentGoal = goal

                        # Add score to
                        for DOT in range(len(NewDotianList)):
                                dotScore = NewDotianList[DOT].points
                                scoreboard[DOT] = dotScore

                        # Scores
                        for cls in range(0,25):
                            print('\n')
                        print(''' | Score  |  Name ''')

                        CleanNames = mod.ScrubList(census, ('\n'))

                        for dotSpot in range(len(NewDotianList)):
                            print(''' | %s | %s ''') % (scoreboard[dotSpot], CleanNames[dotSpot])

                    # Clear Screen
                    if event.key == pygame.K_F1:
                        for cls in range(0,25):
                            print('\n')

                    # Begin CLI
                    if event.key == pygame.K_c:
                        for cls in range(0,25):
                            print('\n')

                        xout = mod.CommandLineInterface('#', NewDotianList)

                        if xout is not None:

                            if 'kill' in xout:
                                grid[int(NewDotianList[int(xout[1])].CurrentPosition[1])][int(NewDotianList[int(xout[1])].CurrentPosition[0])] = 0
                                NewDotianList.pop(int(xout[1]))

                            if 'stop' in xout:
                                done = True
                                pygame.quit()

                            if 'reset' in xout:

                                for row in range(sizeGridX):
                                    for column in range(sizeGridY):
                                        grid[row][column] = 0

                                for Gen in range(0,sizeGridX):
                                    grid[0][Gen] = 9
                                    grid[Gen][0] = 9
                                    grid[sizeGridX-1][Gen] = 9
                                    grid[Gen][sizeGridX-1] = 9

                                for i in range(len(NewDotianList)):
                                    NewDotianList.pop(0)

                                mouseSpawnCount = 0
                                Avatar.visible = False

                            if 'spawn' in xout:

                                if xout[1] == 'avatar' and not Avatar.visible:
                                    Avatar.position = (random.randint(1, GridMax), random.randint(1, GridMax))
                                    Avatar.visible = True
                                    grid[Avatar.position[0]][Avatar.position[1]] = Avatar.color

                    # Avatar Movements
                    if event.key == pygame.K_w and Avatar.visible:
                        pos = (Avatar.position[0]-1, Avatar.position[1])
                        if validLocation(grid, pos):
                            grid[Avatar.position[0]][Avatar.position[1]] = 0
                            Avatar.position = pos
                            grid[Avatar.position[0]][Avatar.position[1]] = Avatar.color

                    if event.key == pygame.K_s and Avatar.visible:
                        pos = (Avatar.position[0]+1, Avatar.position[1])
                        if validLocation(grid, pos):
                            grid[Avatar.position[0]][Avatar.position[1]] = 0
                            Avatar.position = pos
                            grid[Avatar.position[0]][Avatar.position[1]] = Avatar.color

                    if event.key == pygame.K_a and Avatar.visible:
                        pos = (Avatar.position[0], Avatar.position[1]-1)
                        if validLocation(grid, pos):
                            grid[Avatar.position[0]][Avatar.position[1]] = 0
                            Avatar.position = pos
                            grid[Avatar.position[0]][Avatar.position[1]] = Avatar.color

                    if event.key == pygame.K_d and Avatar.visible:
                        pos = (Avatar.position[0], Avatar.position[1]+1)
                        if validLocation(grid, pos):
                            grid[Avatar.position[0]][Avatar.position[1]] = 0
                            Avatar.position = pos
                            grid[Avatar.position[0]][Avatar.position[1]] = Avatar.color


                if event.type == pygame.MOUSEBUTTONDOWN and mouseSpawnCount != mouseSpawnLimit :

                    pos = pygame.mouse.get_pos()
                    column = pos[0] // width
                    row = pos[1] // height

                    # Boundary Check
                    if column != 0 and column != sizeGridX-1 and row != 0 and \
                                    row != sizeGridY-1 and grid[row][column] != 9:

                        mouseSpawnCount += 1

                        # Tuple info
                        Position = (column,row)
                        Started = True
                        NewName = mod.randomLine('data/lib/names/names')
                        NewDotian = Dotian(NewName)
                        NewDotian.StartPosition = Position
                        NewDotian.CurrentPosition = Position
                        NewDotian.GridMax = (sizeGridX - 2)
                        NewDotian.GridMin = 1

                        census.append(NewName)
                        NewDotian.GenderRoll = random.randint(0,1)
                        if(NewDotian.GenderRoll == 1): genderCensus.append('Male')
                        if(NewDotian.GenderRoll == 0): genderCensus.append('Female')
                        NewDotian.DotianStats = mod.GeneGen()
                        statCensus.append(NewDotian.DotianStats)
                        NewDotianList.append(NewDotian)
                        NewDotianList[-1].dotId = len(NewDotianList)-1
                        scoreboard.append(NewDotian.points)

                        # Display Dotians
                        if grid[row][column] != 9:
                            if NewDotian.GenderRoll == 0 :
                                grid[row][column] = 4
                                NewDotian.DotianColor = 4
                            if NewDotian.GenderRoll == 1 :
                                grid[row][column] = 1
                                NewDotian.DotianColor = 1
            if Started :

                for ID in range(len(NewDotianList)):

                    DecisionMade = mod.DecisionMaker(grid, sizeGridX, sizeGridY, NewDotianList[ID].DotianStats, NewDotianList[ID].CurrentPosition, NewDotianList[ID].CurrentGoal);

                    # SENT DOTIAN DATA TO DECISIONMAKING METHOD. THE RESULTS DETERMINE ACTION.
                    # If Decision comes back with a * in it, that means its been run through A*, so it needs to be handled
                    if ('*') in DecisionMade:
                        DecisionMade = list(DecisionMade)
                        # * was appended to the path, so if its in spot 1, a comparison went wrong somewhere, and
                        # it needs to me fixed by moving a dotian in one of 4 directions.
                        if DecisionMade[0] == ('*'):
                            DecisionMade = mod.GrabAMove()
                        # If its just fine, and * is in the [-1] position, continue on.
                        else:
                            # Delete the star, set global goal
                            del(DecisionMade[-1])
                            goalPot = (GlobalGoalX, GlobalGoalY)
                            #  Debugging comment:
                            #  print(' Dotian Position: %s | Goal Position: %s ') % (NewDotianList[ID].CurrentPosition, goalPot)
                            # For the length of moves given, make the moves.
                            for Move in range(len(DecisionMade)):
                                DecisionMade[Move] = int(DecisionMade[Move])
                                # print(DecisionMade[Move])
                                if (DecisionMade[Move] == 0): NewDotianList[ID].NewPosition = mod.moveDown(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                                if (DecisionMade[Move] == 1): NewDotianList[ID].NewPosition = mod.moveRightDOWN(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                                if (DecisionMade[Move] == 2): NewDotianList[ID].NewPosition = mod.moveRight(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                                if (DecisionMade[Move] == 3): NewDotianList[ID].NewPosition = mod.moveRightUP(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                                if (DecisionMade[Move] == 4): NewDotianList[ID].NewPosition = mod.moveUp(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                                if (DecisionMade[Move] == 5): NewDotianList[ID].NewPosition = mod.moveLeftUP(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                                if (DecisionMade[Move] == 6): NewDotianList[ID].NewPosition = mod.moveLeft(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                                if (DecisionMade[Move] == 7): NewDotianList[ID].NewPosition = mod.moveLeftDOWN(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                                pass
                            pass

                    # Single Moves
                    if (DecisionMade == ('u')): NewDotianList[ID].NewPosition = mod.moveUp(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                    if (DecisionMade == ('d')): NewDotianList[ID].NewPosition = mod.moveDown(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                    if (DecisionMade == ('l')): NewDotianList[ID].NewPosition = mod.moveLeft(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                    if (DecisionMade == ('r')): NewDotianList[ID].NewPosition = mod.moveRight(NewDotianList[ID].CurrentPosition, GridMax, GridMin);

                    # Make a random move in 1 of 8 directions.
                    if (DecisionMade == ('MoveAround')):
                        # GENERATE MOVEMENT DIRECTION
                        Movement = random.randint(0,7)
                        # Linear Movement
                        if (Movement == 0): NewDotianList[ID].NewPosition = mod.moveUp(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                        if (Movement == 1): NewDotianList[ID].NewPosition = mod.moveDown(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                        if (Movement == 2): NewDotianList[ID].NewPosition = mod.moveLeft(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                        if (Movement == 3): NewDotianList[ID].NewPosition = mod.moveRight(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                        # Diagonal movement
                        if (Movement == 4): NewDotianList[ID].NewPosition = mod.moveRightDOWN(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                        if (Movement == 5): NewDotianList[ID].NewPosition = mod.moveRightUP(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                        if (Movement == 6): NewDotianList[ID].NewPosition = mod.moveLeftUP(NewDotianList[ID].CurrentPosition, GridMax, GridMin);
                        if (Movement == 7): NewDotianList[ID].NewPosition = mod.moveLeftDOWN(NewDotianList[ID].CurrentPosition, GridMax, GridMin);

                    # Check to see if there are other Dotians on upcoming tiles - Then update the map

                    if validLocation(grid , NewDotianList[ID].NewPosition):

                        # Update position on screen, and remove old position.
                        NewDotianList[ID].AdventureLog.append(NewDotianList[ID].CurrentPosition)                                        # Keep track of paths
                        grid[NewDotianList[ID].CurrentPosition[1]][NewDotianList[ID].CurrentPosition[0]] = 0 		                    # Clear Current spot.
                        grid[NewDotianList[ID].NewPosition[1]][NewDotianList[ID].NewPosition[0]] = NewDotianList[ID].DotianColor		# Update to new position.
                        NewDotianList[ID].CurrentPosition = NewDotianList[ID].NewPosition					 		 					# Update Current position.


                        # Handle the boogy
                        if enhancedBoogy == True :
                            for IDF in range(len(NewDotianList)):
                                ColorNew = random.randint(4,7)
                                NewDotianList[IDF].DotianColor = ColorNew

                        # Handle Goal Reaching
                        if NewDotianList[ID].CurrentGoal != ('MoveAround') and grid[NewDotianList[ID].NewPosition[1]][NewDotianList[ID].NewPosition[0]] == grid[GlobalGoalY][GlobalGoalX]:

                            # Changes all Dotians Prime Directive to move around like an idiot
                            for EG in range(len(NewDotianList)):
                                NewDotianList[EG].CurrentGoal = ('MoveAround')

                            NewDotianList[ID].points = (NewDotianList[ID].points + 1)
                            for popObj in range(len(objectList)-1):
                                objectList.pop(popObj)

                            # Allow new objects
                            blockAlive = False
                            pass

                    else:

                        NewDotianList[ID].NewPosition = NewDotianList[ID].CurrentPosition


# - - - Update the Grid

            for row in range(sizeGridX):

                for column in range(sizeGridY):

                    color = white
                    if grid[row][column] == 0: color = white
                    if grid[row][column] == 1: color = blue
                    if grid[row][column] == 2: color = green
                    if grid[row][column] == 3: color = cyan
                    if grid[row][column] == 4: color = pink
                    if grid[row][column] == 5: color = red
                    if grid[row][column] == 6: color = skyblue
                    if grid[row][column] == 7: color = cyan
                    if grid[row][column] == 9: color = black
                    pygame.draw.rect(screen, color,
                         [width*column, height*row,
                          width, height])
            clock.tick(10)
            pygame.display.flip()

        pygame.quit()


def validLocation(grid, pos):

    if grid[pos[1]][pos[0]] != 1 and grid[pos[1]][pos[0]] != 4 and grid[pos[1]][pos[0]] != 6 \
            and grid[pos[1]][pos[0]] != 9:
        return True
    else:
        return False


Doatia(' Dot Zen ')
