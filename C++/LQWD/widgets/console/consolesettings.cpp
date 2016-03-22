#include "consolesettings.h"

ConsoleSettings::ConsoleSettings()
{
}

void ConsoleSettings::loadFile(QString file)
{
    QFile inputFile(file);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
           QString ln = in.readLine();
           if( ln != "" )
               if( ln != QString(REF_COMMENT))
                   if(ln.at(0) != REF_COMMENT)
                   {
                       // Split setting into key, val and put in map
                       QStringList setting = ln.split(DELIMITER_BLOCK);
                       map.insert(setting[0], setting[1]);
                   }
       }
       inputFile.close();
    }
    else
    {
        qDebug() << " Could not open resource file : config/bkons/bkons_config.lqwd" ;
    }
}

QString ConsoleSettings::getValue(QString key)
{
   QString ret = map.value(key);
   if(ret == "" || ret == "default")
       return "default";
   else
       return ret;
}
