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
    ~MainWindow();

};

#endif // MAINWINDOW_H
