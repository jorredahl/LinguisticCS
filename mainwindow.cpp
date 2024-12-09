#include "mainwindow.h"
#include <QtWidgets>
#include <QAudioOutput>
#include "audio.h"
#include "spectrograph.h"

/*
 * File: mainwindow.cpp
 * Description:
 *  This source file impl:ements the 'MainWindow' class, providing functionality for the main UI.
 *  It initializes the main window, sets up the main layout in a vertical layout, and creates two
 *  'Audio' objects for dual audio playback functionality. The constructor creates the central widget,
 *  configures the vertical layout, and adds two 'Audio' widgets. The 'audio1' and 'audio2' widgets
 *  are integrated into the layout to enable independent audio playback and visualiztion within the
 *  UI layout.
 *
 * Notes:
 *  - The 'Audio' class is used for playback and visualization(*). See 'audio.h' and 'audio.cpp' for
 *    its implementation.
 *  -
 *
 * References:
 *  - ...
 */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // create central widget
    QWidget *center = new QWidget();
    setCentralWidget(center);

    // create main layout for central widget
    // this was V before
    mainLayout = new QVBoxLayout(center);

    // create menu bar
    //QMenu *fileMenu = new QMenu("&File");
    QMenu *fileMenu = menuBar()->addMenu("&File");

    //QAction *uploadAction = new QAction("&Upload Audio File", this);
    //connect(uploadAction, &QAction::triggered, this, &MainWindow::uploadAudio);
    //uploadAction->setShortcut(Qt::CTRL | Qt::Key_N);
    //fileMenu->addAction(uploadAction);

    audio1 = new Audio(nullptr, "Speaker Sound Wave");
    mainLayout->addWidget(audio1);

    // spectrograph 1
    Spectrograph *spectrograph1 = new Spectrograph();
    mainLayout->addWidget(spectrograph1);
  //  QObject::connect(audio1, &Audio::playPressed, spectrograph1, &Spectrograph::restartSpect);
    connect(audio1, &Audio::audioFileSelected, spectrograph1, &Spectrograph::loadAudioFile);

    audio2 = new Audio(nullptr, "User Sound Wave");
    mainLayout->addWidget(audio2);

    // spectrograph 2
    Spectrograph *spectrograph2 = new Spectrograph();
    mainLayout->addWidget(spectrograph2);
  //  QObject::connect(audio1, &Audio::playPressed, spectrograph2, &Spectrograph::restartSpect);

    // spectrograph2->moveToThread(spectThread2);
    connect(audio2, &Audio::audioFileSelected, spectrograph2, &Spectrograph::loadAudioFile);
    // connect(spectThread2, &QThread::finished, spectrograph2, &QObject::deleteLater);

    center->setLayout(mainLayout);
}


MainWindow::~MainWindow() {

}






