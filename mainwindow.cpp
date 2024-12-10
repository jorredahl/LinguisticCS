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
 *  - scroll area: https://stackoverflow.com/questions/65554770/qscrollarea-how-do-i-make-my-central-widget-scrollable

 */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QScrollArea *scrollArea = new QScrollArea(this);
    // create central widget
    QWidget *center = new QWidget();
    setCentralWidget(scrollArea);
    scrollArea->setWidget(center);
    scrollArea->show();
    scrollArea->setWidgetResizable(true);

    // create main layout for central widget
    // this was V before
    mainLayout = new QVBoxLayout(center);

    audio1 = new Audio(nullptr, "Speaker Sound Wave");
    mainLayout->addWidget(audio1);

    // spectrograph 1
    Spectrograph *spectrograph1 = new Spectrograph();
    mainLayout->addWidget(spectrograph1, 0, Qt::AlignRight);
    connect(audio1, &Audio::audioFileSelected, spectrograph1, &Spectrograph::loadAudioFile);

    audio2 = new Audio(nullptr, "User Sound Wave");
    mainLayout->addWidget(audio2);

    // spectrograph 2
    Spectrograph *spectrograph2 = new Spectrograph();
    mainLayout->addWidget(spectrograph2, 0, Qt::AlignRight);
    connect(audio2, &Audio::audioFileSelected, spectrograph2, &Spectrograph::loadAudioFile);

    center->setLayout(mainLayout);
}


MainWindow::~MainWindow() {

}






