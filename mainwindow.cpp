#include "mainwindow.h"
#include <QtWidgets>
#include <QAudioOutput>
#include "visualizer.h"

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

    Visualizer *sourceVisualizer = new Visualizer();

    mainLayout->addWidget(sourceVisualizer);


    Visualizer *userVisualizer = new Visualizer();
    mainLayout->addWidget(userVisualizer);


}

MainWindow::~MainWindow() {}

