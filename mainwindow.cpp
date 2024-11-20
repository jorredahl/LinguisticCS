#include "mainwindow.h"
#include "spectrograph.h"

#include <fftw3.h>
#include <QAudioOutput>
<<<<<<< HEAD
#include <QtWidgets>
#include <QAudioSource>
#include <QAudioDevice>
#include <QAudioDecoder>
#include <QAudioOutput>

=======
#include "audio.h"
>>>>>>> origin/main

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), spectrograph(new Spectrograph(this))
{
<<<<<<< HEAD
    QVBoxLayout *menu = new QVBoxLayout();
    uploadAudioButton = new QPushButton("Upload");
    menu->addWidget(uploadAudioButton);

    QWidget *centerWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centerWidget);
    mainLayout->addLayout(menu);
    mainLayout->addWidget(spectrograph);

    setCentralWidget(centerWidget);

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    connect(uploadAudioButton, &QPushButton::clicked, this, &MainWindow::uploadAudio);
=======
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


>>>>>>> origin/main
}

MainWindow::~MainWindow() {
}

<<<<<<< HEAD
void MainWindow::uploadAudio()
{
    QUrl aName = QFileDialog::getOpenFileUrl(this, "select audio file");
    if (aName.isEmpty())
        return;

    spectrograph->reset();

    // bug - it does not play exactly when the spectrograph is shown or like in sync
    player->setSource(aName);
    player->play();

    // decode audio file to raw data
    decoder = new QAudioDecoder(this);
    decoder->setSource(aName);

    connect(decoder, &QAudioDecoder::bufferReady, this, &MainWindow::bufferReady);

    decoder->start();
    //qDebug() << "audio started";
}

void MainWindow::bufferReady() {
    //qDebug() << "buffer ready";

    QAudioBuffer buffer = decoder->read();
    const qint16 *data = buffer.constData<qint16>();
    int sampleCount = buffer.sampleCount();

    // append incoming samples
    for  (int i = 0; i < sampleCount; ++i) {
        accumulatedSamples.append(static_cast<double>(data[i]));
    }

    // Process in overlapping windows
    while (accumulatedSamples.size() >= spectrograph->getWindowSize()) {

        // extract a chunk of windowSize for processing
        QVector<double> windowSamples = accumulatedSamples.mid(0, spectrograph->getWindowSize());
        spectrograph->setupSpectrograph(windowSamples);

        // remove processed samples based on hopSize
        accumulatedSamples = accumulatedSamples.mid(spectrograph->hopSize);
    }
}

void MainWindow::processAudioBuffer(const QAudioBuffer &buffer) {
    QVector<double> samples;

    const qint16 *data = buffer.constData<qint16>();
    int sampleCount = buffer.sampleCount();

    for (int i = 0; i < sampleCount; ++i) {
        samples.append(static_cast<double>(data[i]));
    }

    // send samples to the Spectrograph for processing
    spectrograph->setupSpectrograph(samples);
}




=======
>>>>>>> origin/main
