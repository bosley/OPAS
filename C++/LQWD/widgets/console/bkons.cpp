#include <QDebug>
#include <QScrollBar>
#include "widgets/console/bkons.h"

/*
    Module that interacts with the user
*/
#include"lib/coremodule.h"

Bkons::Bkons(QWidget *parent)
    : QPlainTextEdit(parent)
{
    errM.consoleOut("Bkons Inititated");

    lqwdText = "@lqwd>  ";
    arrowIndex = 0;

    // In case module doesn't provide a setting - moduleId must be given.
    defaultSettings.loadFile(":/config/bkons/bkons_config.lqwd");

    // Initialize input buffer, history index, and history size limit
    buffer = "";

    // Load default module
    loadModule(&coreModule.settings);

    // Get dir information
    updateAutoComplete();
}

void Bkons::loadModule(ConsoleSettings * settings)
{
    (settings->getValue("moduleId") == "default")
            ? currentModule = defaultSettings.getValue("moduleId").toInt()
            : currentModule = settings->getValue("moduleId").toInt();

    (settings->getValue("maxHistory") == "default")
            ? maxHistory = defaultSettings.getValue("maxHistory").toInt()
            : maxHistory = settings->getValue("maxHistory").toInt();

    (settings->getValue("maxBlockCount") == "default")
            ? maxBlockCount  = defaultSettings.getValue("maxBlockCount").toInt()
            : maxBlockCount = settings->getValue("maxBlockCount").toInt();

    (settings->getValue("fontSize") == "default")
            ? fontSize = defaultSettings.getValue("fontSize").toInt()
            : fontSize = settings->getValue("fontSize").toInt();

    (settings->getValue("font") == "default")
            ? font = defaultSettings.getValue("font")
            : font = settings->getValue("font");

    (settings->getValue("fontColor") == "default")
            ? fontColor = defaultSettings.getValue("fontColor")
            : fontColor = settings->getValue("fontColor");

    (settings->getValue("backgroundColor") == "default")
            ? backgroundColor  = defaultSettings.getValue("backgroundColor")
            : backgroundColor = settings->getValue("backgroundColor");

    (settings->getValue("promptColor") == "default")
            ? promptColor  = defaultSettings.getValue("promptColor")
            : promptColor = settings->getValue("promptColor");

    (settings->getValue("activityName") == "default")
            ? activityName  = defaultSettings.getValue("activityName")
            : activityName = settings->getValue("activityName");

    document()->setMaximumBlockCount(maxBlockCount);

    currentActivity = activityName;

    // Split the rgb values from settings, and load to palette
    QPalette p = palette();
    int r = backgroundColor.split(",")[0].toInt(),
        g = backgroundColor.split(",")[1].toInt(),
        b = backgroundColor.split(",")[2].toInt();
    p.setColor(QPalette::Base, QColor::fromRgb(r,g,b));

    r = fontColor.split(",")[0].toInt(),
    g = fontColor.split(",")[1].toInt(),
    b = fontColor.split(",")[2].toInt();
    p.setColor(QPalette::Text, QColor::fromRgb(r,g,b));

    setFont (QFont (font,fontSize));
    setPalette(p);

    // Using module ids for potential future expansions
    if( 0 == currentModule )
    {
        connect(this, SIGNAL(dataReady(QString)), &coreModule, SLOT(humanInput(QString)));
        connect(&coreModule, SIGNAL(responseReady(QString, bool)), this, SLOT(processResponse(QString, bool)));
    }
    else
        errM.catchError("Module could not be loaded", -1);
}

void Bkons::beginSession()
{
    getUserPrompt();
    setEnabled(true);
}

void Bkons::puts(QString data, bool out)
{
    (out) ? appendHtml( "<br>" + data + "</font> ") : insertPlainText("\n" + data);

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
    getUserPrompt();
}

void Bkons::processResponse(QString response, bool out)
{
    if(response == "clear_console")
    {
        clearConsole();
    }
    else
    {
        updateAutoComplete();
        puts(response, out);
    }

}

void Bkons::clearConsole()
{
    setPlainText("");
    getUserPrompt();
}

void Bkons::updateAutoComplete()
{
    QDir cDir(coreModule.getCWD());
    autoComplete.clear();
    autoComplete = cDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs);
    autoComplete.append(cDir.entryList(QDir::NoDotAndDotDot | QDir::Files));
}

void Bkons::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Backspace:
    {
        if(buffer.length() > 0)
        {
            setFocus();
            QTextCursor storeCursorPos = textCursor();

            if( 0 == arrowIndex )
            {
                buffer.remove(buffer.length()-1,1);
                moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
                moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
                moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
                textCursor().removeSelectedText();
                textCursor().deletePreviousChar();
                getUserPrompt();
                insertPlainText(buffer);
            }
            else if ( 0 < arrowIndex )
            {
                if( buffer.length() > arrowIndex )
                {
                    buffer.remove(buffer.length()  -1 -arrowIndex,1);
                    moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
                    moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
                    moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
                    textCursor().removeSelectedText();
                    textCursor().deletePreviousChar();
                    setTextCursor(storeCursorPos);
                    getUserPrompt();
                    insertPlainText(buffer);
                    storeCursorPos.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, (arrowIndex));
                    setTextCursor(storeCursorPos);
                }
            }
        }
        break;
    }
    case Qt::Key_Left:
    {
        if( buffer.length() > 0 )
            if( buffer.length() > arrowIndex )
            {
                //int bufferLen = buffer.length();
                QTextCursor tmpCursor = textCursor();
                tmpCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
                setTextCursor(tmpCursor);
                arrowIndex++;
            }
        break;
    }
    case Qt::Key_Right:
    {
        if( buffer.length() > 0 )
            if( 0 < arrowIndex )
            {
                QTextCursor tmpCursor = textCursor();
                tmpCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 1);
                setTextCursor(tmpCursor);
                arrowIndex--;
            }
        break;
    }
    // Cycle up through history
    case Qt::Key_Up:
    {
        if(hIndex < history.length() && hIndex >= 0)
        {
            setFocus();
            QTextCursor storeCursorPos = textCursor();
            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
            textCursor().removeSelectedText();
            textCursor().deletePreviousChar();
            setTextCursor(storeCursorPos);
            buffer = history.at(hIndex);
            getUserPrompt();
            insertPlainText(buffer);
            hIndex--;
        }
        break;
    }
        // Cycle down through history
    case Qt::Key_Down:
    {
        if(history.length() > hIndex+1 )
        {
            setFocus();
            buffer = history.at(hIndex+1);
            QTextCursor storeCursorPos = textCursor();
            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
            textCursor().removeSelectedText();
            textCursor().deletePreviousChar();
            setTextCursor(storeCursorPos);
            getUserPrompt();
            insertPlainText(buffer);
            hIndex++;
        }
        break;
    }
    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
        if( !buffer.trimmed().isEmpty())
        {
            // Make sure that the cursor gets placed back to the end
            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);

            // Add input to history, if its full, make room and then add
            if( history.length() <= maxHistory)
                history.append(buffer);//history.push(buffer);
            else
            {
                history.removeLast();
                history.append(buffer);
            }

            // Send data to module for processing
            emit dataReady(buffer);

            buffer = "";
            arrowIndex = 0;
            hIndex = history.length()-1;
        }
        break;
    }
    case Qt::Key_Tab:
    {
        // Extract which part of the buffer the user is typing in,
        // Example : [mv dolan "Donald Du] -> ["Donald Du]

        bool inQ = false;
        QString temp = "";
        QString currEntry;
        QStringList currCmds;
        bool replaceQ = false;
        foreach(QString ch, buffer)
        {
            // Toggle if within "
            if(ch == QString('"'))
            {
                replaceQ = true;
                (inQ) ? inQ = false : inQ = true;
            }

            // If space is not within "'s add to commands
            if(!inQ && ch == " ")
            {
                currCmds.append(temp);
                temp = "";
            }
            else
                temp += ch;
        }
        currEntry = temp;

        // Cycle through directories and documents find something
        // with the currEntry as a substring, if found insert into console
        bool control = true;
        QString newBuffer = "";

        // Remove " for matching
        currEntry.replace('"', "");
        foreach(QString item, autoComplete)
        {
            // Check substring, removing " for matching - Control ensures only 1 match
            if( control && item.split(" ")[0].contains(QRegExp(("^[%s]", currEntry))))
            {
                control = false;

                // If found, rebuild the buffer
                foreach(QString cmd, currCmds)
                    newBuffer += (cmd + " ");

                // Replace " if it existed on the currEntry, or if " should exist
                if(replaceQ || item.contains(" "))
                {
                    newBuffer += QString('"');
                    newBuffer.append(item);
                    newBuffer += QString('"');
                }
                else
                {
                    newBuffer.append(item);
                }
            }
        }

        // If newBuffer was built, match was found so update console
        if(newBuffer != "")
        {
            buffer = newBuffer;
            setFocus();
            QTextCursor storeCursorPos = textCursor();
            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
            textCursor().removeSelectedText();
            textCursor().deletePreviousChar();
            setTextCursor(storeCursorPos);
            getUserPrompt();
            insertPlainText(buffer);
        }
        break;
    }
    default:
        addKeyToBuffer(e->text());
        break;
    }
}

void Bkons::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Bkons::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Bkons::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}

void Bkons::addKeyToBuffer(QString key)
{
    // Test buffer cap
    if( buffer.length() <= CAP_INPUT_BUFFER )
    {
        // Find out where in the buffer to put the key
        if( 0 == arrowIndex )
        {
            // Appending to end of text
            buffer += key;
            insertPlainText(key);
        }
        else
        {
            // User moved cursor, place key in buffer at location
            buffer.insert((buffer.length()-arrowIndex), key);

            setFocus();
            QTextCursor storeCursorPos = textCursor();

            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
            textCursor().removeSelectedText();
            textCursor().deletePreviousChar();
            getUserPrompt();
            insertPlainText(buffer);
            storeCursorPos.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, arrowIndex);
            setTextCursor(storeCursorPos);
        }
    }
    else
    {
        /*
            If cap is reached, delete all of what was entered
            / clear buffer and give back a prompt
        */
        setFocus();
        buffer.remove(buffer.length()-1,buffer.length());
        QTextCursor storeCursorPos = textCursor();
        moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
        textCursor().removeSelectedText();
        textCursor().deletePreviousChar();
        setTextCursor(storeCursorPos);
        buffer = "";
        getUserPrompt();
        insertPlainText(buffer);
    }
}

void Bkons::getUserPrompt()
{
    appendHtml(("<font color=" + promptColor + ">")
               + currentActivity
               + lqwdText
               + "</font>");
}

