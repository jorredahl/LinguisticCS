#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAudioOutput>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QPushButton>
#include <QIODevice>
#include <QAudioInput>
#include <QtMultimedia/qaudiobuffer.h>
#include <QAudioDecoder>

#include "spectrograph.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void uploadAudio();
   // void userAudio();
    void processAudioBuffer(const QAudioBuffer &buffer);
    void bufferReady();


private:
    QPushButton *uploadAudioButton;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    Spectrograph *spectrograph;
    QAudioDecoder *decoder;
    QVector<double> accumulatedSamples;

};

#endif // MAINWINDOW_H
