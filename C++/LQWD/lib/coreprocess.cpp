#include "coreprocess.h"

CoreProcess::CoreProcess(QObject *parent) :
    QObject(parent)
{
    errM.consoleOut("Core Process Initiated");

    // Create the process, and setup error handling
    proc = new QProcess;
    connect(proc, SIGNAL(error(QProcess::ProcessError)), &errM, SLOT(handleProcessError(QProcess::ProcessError)));

    // Test to see what working dir to start off in
    if (OSINDEX != -1)
        (OSINDEX == 1) ? setWorkingDirectory("C:\\") :
                         setWorkingDirectory("/");
    else
        errM.catchError("Unknown OS", -1);
}

QString CoreProcess::getCWD()
{
    return currDir;
}

void CoreProcess::processCommand(QString command)
{
    // Handles if html(1) or plain text(!1) output,
    outputType = false;

    QString result;
    QStringList translation = nexus.retrieveCommand(command);

    if( translation[0] != "?" )
    {
        if( translation[0] == "C" )
        {
            translation.removeAt(0);
            result = qtInterface(translation);
        }
        else if ( translation[0] == "T" )
        {
            translation.removeAt(0);
            result = systemInterface(translation);
        }
        emit processComplete(result, outputType);
    }
    else
    {
        errM.catchError("Core Process - Unknown primary command", 1);

        // This doesn't really need to be tested, but I do so just in case.
        if( command.split(' ').length() >= 1 )
            emit processComplete("Unknown Command [" + command.split(' ')[0] + "]", outputType);
        else
            emit processComplete("Unknown Command", outputType);
    }
}

QString CoreProcess::qtInterface(QStringList cmdChunks)
{
    if( "list_directory" == cmdChunks[0] )
    {
        if (cmdChunks.length() > 3)
            return "Invalid input for listing directory.";

        outputType = 1;
        unsigned i = 1;
        QString temp = "";
        QDir recoredDir;

        // Test to see if listing the current, or specified directory
        if( cmdChunks.length() == 3 )
        {
            qDebug() << "User wants to list a different directory than their current";

            QFile file(cmdChunks[2]);
            if (!file.exists())
                return sendFileNotExist(cmdChunks[2]);

            recoredDir.setPath(cmdChunks[2]);
        }
        else
        {
            qDebug() << "List the current directory";
            recoredDir = QDir::currentPath();
        }
        QStringList directoryList = recoredDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs);
        QStringList documentList = recoredDir.entryList(QDir::NoDotAndDotDot | QDir::Files);

        // Get and display directories
        temp += "<font color=red>[Directories]</font><br><b>";

        foreach(QString el, directoryList)
            if( i++ % 3 == 0 )
                temp += (" " + el + "<br>");
            else
            {
                temp += ( " " + el);

                // If its not the last entry, add a red comma
                if( !((directoryList.indexOf(el)) == (directoryList.length()-1)))
                    temp += ("<font color=red>,</font>");
            }

        // Get and display documents
        temp += "</b><br><br><font color=red>[Documents]</font><br>";
        i = 1;

        // Get and display documents
        foreach(QString el, documentList)
            if ( i++ % 3 == 0 )
                temp += (" " + el + "<br>");
            else
            {
                temp += ( " " + el);

                // If its not the last entry, add a red comma
                if( !((documentList.indexOf(el)) == (documentList.length()-1)))
                    temp += ("<font color=red>,</font>");
            }

        return temp;
    }
    else if( "change_directory" == cmdChunks[0] )
    {
        if(cmdChunks.length() <= 2)
        {
            setWorkingDirectory(QDir::homePath());
            return "";
        }
        else
            return changeDirectory(cmdChunks);
    }
    else if( "remove_document" == cmdChunks[0] )
    {
        if(cmdChunks.length() < 3)
            return "> No name given for directory removal";
        else
        {
            if(QDir(cmdChunks[2]).exists())
            {
                QDir().remove(cmdChunks[2]);
                if(!QDir(cmdChunks[2]).exists())
                    return "Directory removed";
                else
                    return "Error removing directory";
            }
            else
            {
                return sendFileNotExist(cmdChunks[2]);
            }

        }
    }
    else if( "clear_console" == cmdChunks[0] )
    {
        return cmdChunks[0];
    }
    else if( "working_dir" == cmdChunks[0] )
    {
        outputType = 2;
        QString temp = "<font color=red>";
        temp += getCWD();
        temp += "</font>";
        return temp;
    }
    else if("create_directory" == cmdChunks[0] )
    {
        if(cmdChunks.length() < 3)
            return "> No name given for new directory";
        else
        {
            qDebug() << QDir::currentPath();

            if(QDir(cmdChunks[2]).exists())
                return "Directory Exists";
            else
            {
                QDir().mkdir(cmdChunks[2]);
                if(QDir(cmdChunks[2]).exists())
                    return "Directory Created";
                else
                    return "Error creating folder";
            }
            return "Error creating folder";
        }
    }
    else if("load_module" == cmdChunks[0] )
    {
        return "[coreProcess] Module loading not programmed";
    }
    else if("save_output" == cmdChunks[0])
    {
        return "[coreProcess] Saving output not programmed";
    }
    else if("request_help" == cmdChunks[0])
    {
        return "[coreProcess] Help not programmed";
    }
    else
    {
        return "Unhandled command scope.";
    }
}

QString CoreProcess::systemInterface(QStringList cmdChunks)
{
    QString response, command;
    for(int i = 0; i < cmdChunks.length(); i++)
    {
        command += cmdChunks[i];
        if( i != cmdChunks.length()-1 )
            command += " ";
    }

    proc->start(command);
    proc->waitForFinished(-1);
    QByteArray result = proc->readAllStandardOutput();
    QString res(result);
    response += res;
    proc->close();
    return response;
}

void CoreProcess::setWorkingDirectory(QString dir)
{
    QDir::setCurrent(dir);
    proc->setWorkingDirectory(dir);
    currDir = proc->workingDirectory();
}

QString CoreProcess::changeDirectory(QStringList command)
{
    qDebug() << " IN coreProcess->changeDirectory() \n" << command;

    command.removeAt(0);
    QString newDir = "";
    if( command[1] == ".." )
    {
        QDir curr = QDir::current();
        curr.cdUp();
        newDir = curr.path();
    }
    else
        newDir = command[1];

    // Remove " in "path name"
    QString temp = "";
    foreach(QString ch, newDir)
    {
        if(ch != QString('"'))
            temp += ch;
    }
    newDir = temp;

    // If the folder exists, move to it
    if( QDir(newDir).exists() )
    {
        setWorkingDirectory(newDir);
        return "Directory Changed";
    }
    else
        return sendFileNotExist(command[1]);

    return "There was an error changing directories ";
}

QString CoreProcess::sendFileNotExist(QString fileName)
{
    return QString("[ %1 ] Does not exist.").arg(fileName);
}
