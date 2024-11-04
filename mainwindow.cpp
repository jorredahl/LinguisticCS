#include "mainwindow.h"
#include <QtWidgets>
#include <QAudioOutput>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // create central widget
    QWidget *center = new QWidget();
    setCentralWidget(center);

    // create main layout for central widget
    mainLayout = new QHBoxLayout(center);

    // create menu bar
    //QMenu *fileMenu = new QMenu("&File");
    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *uploadAction = new QAction("&Upload Audio File", this);
    connect(uploadAction, &QAction::triggered, this, &MainWindow::uploadAudio);
    uploadAction->setShortcut(Qt::CTRL | Qt::Key_N);
    fileMenu->addAction(uploadAction);

    // create upload button
    uploadAudioButton = new QPushButton("Upload");
    mainLayout->addWidget(uploadAudioButton, 0, Qt::AlignCenter);
    connect(uploadAudioButton, &QPushButton::clicked, this, &MainWindow::uploadAudio);

    // create vertical layout for sound waves
    QVBoxLayout *waveformLayout = new QVBoxLayout();

    // Native speaker's sound wave placeholder
    QLabel *nativeWaveLabel = new QLabel("Native Speaker Sound Wave"); // using QLabel as a placeholder for waveforms
    nativeWaveLabel->setAlignment(Qt::AlignCenter);
    nativeWaveLabel->setStyleSheet("border: 1px solid black; min-height: 100px;");

    // user's sound wave placeholder
    QLabel *userWaveLabel = new QLabel("User's Sound Wave");
    userWaveLabel->setAlignment(Qt::AlignCenter);
    userWaveLabel->setStyleSheet("border: 1px solid black; min-height: 100px;");


    waveformLayout->addWidget(nativeWaveLabel);
    waveformLayout->addWidget(userWaveLabel);

    mainLayout->addLayout(waveformLayout);
}

MainWindow::~MainWindow() {}

void MainWindow::uploadAudio(){
    QUrl aName = QFileDialog::getOpenFileUrl(this, "Select audio file");
    if (aName.isEmpty()) return;

    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    // ...
    player->setSource(aName);
    audioOutput->setVolume(50);
    player->play();


}
