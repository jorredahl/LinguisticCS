#ifndef MAINWINDOW_H
#define MAINWINDOW_H

<<<<<<< HEAD
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
=======
#include <QMainWindow>
#include <QHBoxLayout>
#include "audio.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT
    QVBoxLayout *mainLayout;
    Audio *audio1;
    Audio *audio2;


>>>>>>> origin/main


public:
    MainWindow(QWidget *parent = nullptr);
    //make a audio layout function that sets up what visualizer was making so that we can have two audio players
    ~MainWindow();

<<<<<<< HEAD
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

=======
>>>>>>> origin/main
};

#endif // MAINWINDOW_H
