#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include "audio.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT
    QVBoxLayout *mainLayout;
    Audio *audio1;
    Audio *audio2;




public:
    MainWindow(QWidget *parent = nullptr);
    //make a audio layout function that sets up what visualizer was making so that we can have two audio players
    ~MainWindow();

};
#endif // MAINWINDOW_H
