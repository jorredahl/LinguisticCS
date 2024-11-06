#include "mainwindow.h"
#include "spectrograph.h"

#include <fftw3.h>

#include <QAudioOutput>
#include <QtWidgets>
#include <QAudioSource>
#include <QAudioDevice>
#include <QAudioDecoder>
#include <QAudioOutput>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), spectrograph(new Spectrograph(this))
{
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);

    player->setAudioOutput(audioOutput);

    QVBoxLayout *menu = new QVBoxLayout();
    uploadAudioButton = new QPushButton("Upload and Play Audio");
    menu->addWidget(uploadAudioButton);

    QWidget *centerWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centerWidget);
    mainLayout->addLayout(menu);
    mainLayout->addWidget(spectrograph);
    setCentralWidget(centerWidget);

    connect(uploadAudioButton, &QPushButton::clicked, this, &MainWindow::uploadAudio);
}


MainWindow::~MainWindow() {

}


void MainWindow::uploadAudio()
{
    QUrl aName = QFileDialog::getOpenFileUrl(this, "select audio file");
    if (aName.isEmpty())
        return;

    // bug - it does not play exactly when the spectrograph is shown
    player->setSource(aName);
    player->play();

    // decode audio file to raw data
    decoder = new QAudioDecoder(this);
    decoder->setSource(aName);

    connect(decoder, &QAudioDecoder::bufferReady, this, &MainWindow::bufferReady);

    decoder->start();
    qDebug() << "audio started";
}


void MainWindow::bufferReady() {

    qDebug() << "buffer ready";

    QAudioBuffer buffer = decoder->read();
    const qint16 *data = buffer.constData<qint16>();
    int sampleCount = buffer.sampleCount();

    // get samples from each buffer
    // for(int i = 0; i < sampleCount; ++i) {
    //     accumulatedSamples.append(static_cast<double>(data[i]));
    // }

    // append incoming samples to vector
    for(int i = 0; i < sampleCount; ++i) {
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

    // // only call when theres data
    // if(accumulatedSamples.size() >= spectrograph->getWindowSize()) {
    //     spectrograph->setupSpectrograph(accumulatedSamples);
    //     accumulatedSamples.clear();  // Clear after processing
    // }
}


void MainWindow::processAudioBuffer(const QAudioBuffer &buffer) {
    QVector<double> samples;

    const qint16 *data = buffer.constData<qint16>();
    int sampleCount = buffer.sampleCount();

    for(int i = 0; i < sampleCount; ++i) {
        samples.append(static_cast<double>(data[i])); // store as doubles
    }

    // send samples to the Spectrograph for processing
    spectrograph->setupSpectrograph(samples);
}




