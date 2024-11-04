#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QtWidgets>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QVBoxLayout *mainLayout;



public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


};
#endif // MAINWINDOW_H
