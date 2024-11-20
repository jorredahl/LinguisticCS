#include "mainwindow.h"
#include <QtWidgets>
#include <QAudioOutput>
#include "audio.h"

/*
 * File: mainwindow.cpp
 * Description:
 *  This source file implements the 'MainWindow' class, providing functionality for the main UI.
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

    audio2 = new Audio(nullptr, "User Sound Wave");
    mainLayout->addWidget(audio2);

}


MainWindow::~MainWindow() {
}






