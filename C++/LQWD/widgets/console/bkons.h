#ifndef CONSOLE_BKONS
#define CONSOLE_BKONS

#include <QDir>
#include <QStack>
#include <QPlainTextEdit>

#include "errormachine.h"
#include "lib/coremodule.h"
#include "consolesettings.h"

class Bkons : public QPlainTextEdit
{
    Q_OBJECT

signals:
    void dataReady(QString data);

public:
    explicit Bkons(QWidget *parent = 0);

    void beginSession();

    void puts(QString, bool);

public slots:

    // Outputs to screen
    void processResponse(QString, bool);

    // Clears the screen
    void clearConsole();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);

private:

    // Settings
    ConsoleSettings defaultSettings;

    // Default module
    CoreModule coreModule;

    ErrorMachine errM;

    // State, and display
    QString lqwdText,
    currentActivity,
    font,
    fontColor,
    backgroundColor,
    promptColor,
    activityName;
    int fontSize,
    maxBlockCount;

    // Input Controls
    // Handles the users input until they hit return/enter
    QString buffer;

    QStringList autoComplete;

    //The current index of the cursor relative to buffer
    int bufferCursorIndex;

    // Indicates when the user is using arrow keys, and where in the buffer they are
    int arrowIndex;

    // Max history saved /Index of histroy being viewd /the history
    int maxHistory, hIndex;
   // QStack<QString> history, tempHistory;
    QStringList history;

    // Module Handling
    int currentModule;

    // Load the module interfacing with bkons
    void loadModule(ConsoleSettings *);

    // Send an input to be added to the buffer
    void addKeyToBuffer(QString key);

    // Call to print input prompt (Dispays activity)
    void getUserPrompt();

    // Update the TAB option for the user on directory change
    void updateAutoComplete();

};

#endif // CONSOLE_H
