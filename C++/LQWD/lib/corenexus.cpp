#include "corenexus.h"

CoreNexus::CoreNexus()
{
    loadCommandMap();
    ignoreExplicitlyUndefinedArguments = true;
}

CoreNexus::~CoreNexus(){}

void CoreNexus::loadCommandMap()
{
    QFile inputFile(":/config/coreModule/command_map.lqwd");
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QList<_carg> tempArgList;
       bool insideCommandDefinition = false;
       bool insideArgumentDefinition = false;
       QString currentSequenceName = "", command = "", desc = "";

       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
           QString ln = in.readLine();
           if( ln != "" )
               if( ln != QString(REF_COMMENT))
                   if(ln.at(0) != REF_COMMENT)
                   {
                       // If not inside any definitions
                       if( !insideCommandDefinition && !insideArgumentDefinition )
                       {
                           // If name not set
                           if( currentSequenceName == "" )
                               currentSequenceName = ln.trimmed();
                           else
                           {
                               // Name already given, and not in definitions
                               if( ln.at(0) == JBOS_DEFINE )
                               {
                                   insideCommandDefinition = true;
                               }
                           }
                       }
                       else if( insideCommandDefinition && !insideArgumentDefinition )
                       {
                           // Getting here means it is looking for a command to be mapped, and
                           // looking to see if there are any arguments to be added.
                           if( ln.split(DELIMITER_BLOCK)[0].trimmed() == "command" )
                           {
                               command = ln.split(DELIMITER_BLOCK)[1].trimmed();
                               desc = ln.split(DELIMITER_BLOCK)[2].trimmed();
                           }
                           else if( ln.split(DELIMITER_BLOCK)[0].trimmed() == "args" )
                           {
                               insideArgumentDefinition = true;
                           }
                           else if( currentSequenceName.trimmed() == "os_commands"
                                    && ln.trimmed() != QString(JBOS_ENDEFINE))
                           {
                               // In os specific commands, translate and add to translationMap
                               QStringList transCommand;
                               QStringList cmdBlocks = ln.trimmed().split(DELIMITER_BLOCK);
                               // Add commmand 0 as the input command,
                               // then determine the output command
                               transCommand.append(cmdBlocks[0]);
                               if( cmdBlocks[OSINDEX] == REF_PRIOR )
                               {
                                   if(cmdBlocks[OSINDEX-1] == REF_PRIOR)
                                       transCommand.append(cmdBlocks[OSINDEX-2]);
                                   else if(cmdBlocks[OSINDEX-1] == REF_UNDEFINED)
                                       transCommand.append(cmdBlocks[0]);
                                   else
                                       transCommand.append(cmdBlocks[OSINDEX-1]);
                               }
                               else if( cmdBlocks[OSINDEX] == REF_UNDEFINED )
                               {
                                   if( !ignoreExplicitlyUndefinedArguments )
                                       transCommand.append(cmdBlocks[0]);
                               }
                               else
                               {
                                   transCommand.append(cmdBlocks[OSINDEX]);
                               }
                               translationMap.append(_transCmd(transCommand[0], transCommand[1]));

                           }

                       }
                       else if( insideCommandDefinition && insideArgumentDefinition &&
                                ln.trimmed() != QString(JBOS_ENDEFINE))
                       {
                           tempArgList.append(_carg(ln.split(DELIMITER_BLOCK)[0].trimmed(),
                                              ln.split(DELIMITER_BLOCK)[1].trimmed()));
                       }

                       if( ln.trimmed() == QString(JBOS_ENDEFINE)
                           && currentSequenceName.trimmed() != "os_commands")
                       {
                           // Create the command with the information, and reset local temps
                           _cmd newCmd;
                           newCmd.sequenceName = currentSequenceName;
                           newCmd.description = desc;
                           newCmd.arguments = tempArgList;
                           newCmd.cmd = command;
                           commandMap.append(newCmd);

                           insideArgumentDefinition = false;
                           insideCommandDefinition = false;
                           currentSequenceName = "";
                           tempArgList.clear();
                           command = "";
                           desc = "";
                       }
                   }
       }
       inputFile.close();
    }
    else
    {
        errM.catchError(" Could not open resource file : command_map.lqwd ", -1);
    }
}

QStringList CoreNexus::filterCommand(QString input)
{
    QStringList cmd;
    QString buffer = "";
    bool ignore = false, spaces = false;

    foreach(QString ch, input)
    {
        if( ch == " " && !ignore)
        {
            cmd.append(buffer);
            buffer = "";

            if(!spaces)
                spaces = true;
        }
        else if ( ch == QString('"') && !ignore )
        {
            // Find ' " ' encased strings ex:  "Monty"
            buffer += ch;
            ignore = true;

        }
        else if ( ch == QString('"') && ignore)
        {
            // Get end of encased strings ex:  "Monty"
            buffer += ch;
            ignore = false;
            cmd.append(buffer);
            buffer = "";
        }
        else if(ignore || ch != " ")
        {
            // If its a blank space and its in an encased string, add it
            // if its a blank space and not encased, ignot it
            buffer += ch;
        }

        // Attach last char
        if(input.indexOf(ch) == input.length())
        {
            cmd.append(buffer);
            buffer = "";
        }
    }

    // If there is still something in the buffer, and its not a single command
    if( buffer != "" && input.split(" ").length() > 1)
    {
        cmd.append(buffer);

    }

    // if it is a simple command, and nothing trailing
    if(!spaces)
        cmd.append(input);

    // Just in case any slip by
    cmd.removeAll("");
    return cmd;
}

QStringList CoreNexus::retrieveCommand(QString input)
{
    QStringList reply;
    QStringList inCmd = filterCommand(input);

    // First search through the command map
    for( int i = 0; i < commandMap.length(); i++ )
    {
        if( commandMap[i].cmd == inCmd[0] )
        {
            // Indicate that the command is in command map
            reply.append("C");

            // Add the descriptor so we know what the command is suppose to do
            reply.append(commandMap[i].sequenceName);

            // Add the command
            reply.append(commandMap[i].cmd);

            // Add the other arguments
            for( int j = 1; j < inCmd.length(); j++ )
            {
                reply.append(inCmd[j]);
            }
            return reply;
        }
    }

    // Then search through the translation map
    for( int i = 0; i < translationMap.length(); i++ )
    {
        if( translationMap[i].input == inCmd[0] )
        {
            // Indicate that the command is in translation map
            reply.append("T");

            // Add the command
            reply.append(translationMap[i].translation);

            // Add the other arguments
            for( int j = 1; j < inCmd.length(); j++ )
            {
                reply.append(inCmd[j]);
            }
            return reply;
        }
    }
    reply.append("?");
    return reply;
}
