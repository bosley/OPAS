#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "stringanalyzer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_comapreWords_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;


    // The analyzer object, with 100% tolerance
    sanalyzer::stringAnalyzer Analyzer;
};

#endif // MAINWINDOW_H
