#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stringanalyzer.h"
#include <qDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->searchResult->hide();
    ui->compareResult->hide();


    /*
        Each dataSet contains a map,
        and data about the map.
    */
    int dataSetCount = 3;
    sanalyzer::_dataSet dataSets[dataSetCount];
    std::map<int, std::string> dataMaps[dataSetCount];

    ui->setCountLabel->setText( QString::number(dataSetCount) );



    // Setup Set 1
    std::string commands_data_set[3] = {"OFF", "ON", "REBOOT"};
    for(int i = 0; i < 3; i++ )
    {
        dataMaps[0].insert( std::pair<int, std::string> ( i, commands_data_set[i] ) );
    }
    dataSets[0].dataLabel = " Commands Set ";
    dataSets[0].dataSet = dataMaps[0];
    dataSets[0].size = 3;



    // Setup Set 2
    std::string verbs_data_set[4] = {"MOVE", "JUMP", "RUN", "DIVE"};
    for(int i = 0; i < 4; i++ )
    {
        dataMaps[1].insert( std::pair<int, std::string> ( i, verbs_data_set[i] ) );
    }
    dataSets[1].dataLabel = " Verbs Set ";
    dataSets[1].dataSet = dataMaps[1];
    dataSets[1].size = 4;


    // Setup Set 3
    std::string animals_data_set[5] = {"DOG", "BEAR", "MOOSE", "CHEETAH", "BIGFOOT"};
    for(int i = 0; i < 5; i++ )
    {
        dataMaps[2].insert( std::pair<int, std::string> ( i, animals_data_set[i] ) );
    }
    dataSets[2].dataLabel = " Animals Set ";
    dataSets[2].dataSet = dataMaps[2];
    dataSets[2].size = 5;



    // Load Data Sets into the Analyzer
    Analyzer.loadDataSets(dataSetCount, dataSets);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comapreWords_clicked()
{
    std::string x = ui->compareOne->text().toUtf8().constData();
    std::string y = ui->compareTwo->text().toUtf8().constData();

    if ( Analyzer.compair(x, y) )
    {
        ui->compareResult->setText(" MATCH ! ");
    }
    else
    {
        ui->compareResult->setText(" NO MATCH ! ");
    }

    ui->compareResult->show();

}



void MainWindow::on_pushButton_clicked()
{
    std::string x = ui->dataSetSearch->text().toUtf8().constData();
    QString t =  QString::fromStdString( Analyzer.ofDataSet( x ) );
    ui->searchResult->setText( t );
    ui->searchResult->show();
}
