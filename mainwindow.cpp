#include "mainwindow.h"
#include <QtWidgets>
#include <QAudioOutput>
#include "audio.h"
#include "spectrograph.h"


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

    Spectrograph *spectWidget1 = new Spectrograph(nullptr, 1);
    mainLayout->addWidget(spectWidget1);

    audio2 = new Audio(nullptr, "User Sound Wave");
    mainLayout->addWidget(audio2);

    Spectrograph *spectWidget2 = new Spectrograph(nullptr, 2);
    mainLayout->addWidget(spectWidget2);


}


MainWindow::~MainWindow() {
}






