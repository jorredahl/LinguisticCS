#include "mainwindow.h"
#include <QtWidgets>
#include <QAudioOutput>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //widgets
    QWidget *center = new QWidget();
    setCentralWidget(center);
    mainLayout = new QHBoxLayout(center);
    uploadAudioButton = new QPushButton("upload");
    mainLayout-> addWidget(uploadAudioButton, 0, Qt::AlignCenter);
    connect(uploadAudioButton, &QPushButton::clicked, this, &MainWindow::uploadAudio);


}

MainWindow::~MainWindow() {}

void MainWindow::uploadAudio(){
    QUrl aName = QFileDialog::getOpenFileUrl(this, "select audio file");
    if (aName.isEmpty()) return;

    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    // ...
    player->setSource(aName);
    audioOutput->setVolume(50);
    player->play();


}
