#include "engine.h"

Engine::Engine(QObject *parent) : QObject(parent)
{
    continueWorking = false;
    // Connect this to the socket class via dataready to read info back
    connect(&socketH, SIGNAL(dataReady(QString)), this, SLOT(processServerData(QString)));
}

void Engine::retrieveNewJob()
{
    continueWorking = true;
    // Tell the user whats up
    emit userSignal("Requesting new job");
    socketH.SubmitQuery(REQ_NEW_ITEM);
}

void Engine::processServerData(QString serverData)
{
    // Split up the data to tell the user what size we have
    QByteArray bytes = serverData.toUtf8();
    int l = bytes.size();
    QString update = "["+ QString::number(l) +"] bytes recieved";
    emit userSignal(update);

    // Split by delimter
    QStringList sdata = serverData.split("^");

    // Make sure that we are recieving a job related item
    if(sdata.length() >= 2)
    {
        /*
                Figure out what it is we are trying to do
        */
        if("1" == sdata[1])
        {
            emit userSignal("Type:Sort "+ sdata[2]);
            completeSortingJob(sdata);
        }
        else
        {
            emit userSignal("Unknown job type. Abortion emninent");
            // Send word to the server to put the job back into open jobs
        }
    }
    else
    {
        // Show the user the message that we got from the server
        emit userSignal("Message from server: " + sdata[0]);
    }
}

void Engine::listenForSignal(char signal)
{
    // Figure out what the signal is (There is only 1 atm, but its good for expansion)
    switch(signal)
    {
    case 's':
        continueWorking = false;
        emit userSignal("Recieved stop signal.");
        break;
    default:
        emit userSignal("Unknown signal given to engine. Continuing.");
        break;
    }
}

void Engine::completeSortingJob(QStringList jobInfo)
{
    // Sort here with all sorts of sorts and sort out times
    // Inform the user that the job is completed and give run time stats
    QString bSortTime = "Bubble Sort\t: ";
    QString qSortTime = "Quick Sort\t: ";
    QString mSortTime = "Merge Sort\t: ";
    QString totalTime = "Total Time\t: ";
    QString totalAvgT = "Total Avg\t: ";

    // Convert data to integers, and put in format that the sort machine can understand
    int j = 0;
    int narr[SORT_DATA_SET];
    QStringList nums = jobInfo[3].split("\n");
    for(int i=0; i<nums.length(); i++)
    {
        // Apparently QT Doesn't like to cast substrings directly to ints :(
        QString a = nums[i];
        int b = a.toInt();
        narr[j] = b;
        j++;
    }

    // Load the sorting machine
    NumberSort.loadData(narr, nums.length());

    // Run the sorts
    sReturn returnItem = NumberSort.runSorts();

    // Add sort time to strings
    bSortTime += QString::number(returnItem.btime);
    qSortTime += QString::number(returnItem.qtime);
    mSortTime += QString::number(returnItem.mtime);
    totalTime += QString::number(returnItem.ttime);
    totalAvgT += QString::number(returnItem.tavgtime);

    // Update the user
    QString update = "Completed sorting job! Runtimes:\n\t";
    update += (bSortTime + "\n\t");
    update += (qSortTime + "\n\t");
    update += (mSortTime + "\n\t");
    update += (totalTime + "\n\t");
    update += (totalAvgT + "\n>>Updating Server");

    emit userSignal(update);

    // Build the update item
    QString updateItem = (jobInfo[0] + "^" + jobInfo[1] + "^");
    updateItem += (bSortTime + "," + qSortTime + "," + mSortTime + "," + totalTime + "," + totalAvgT + ";" + jobInfo[2] + "^" + jobInfo[3]);

    //Update the server
    socketH.SubmitQuery(UPD_CUR_ITEM, updateItem);

    // Inform the gui that we are done so that the user can ask us to slave away again.
    emit jobCompleted();
}
