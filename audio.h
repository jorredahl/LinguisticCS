#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QMediaPlayer>
#include <QToolButton>
#include <QHBoxLayout>
#include "wavform.h"

class Audio : public QWidget
{
    Q_OBJECT
        QPushButton *uploadAudioButton;
        QMediaPlayer *player;
        QAudioOutput *audioOutput;
        bool audioPlaying;
        QToolButton *playButton;
        QHBoxLayout *audioLayout;


public:
    explicit Audio(QWidget *parent = nullptr);
    WavForm *wavChart;
    void newAudioPlayer();

public slots:
    void uploadAudio();
    void handlePlayPause();



signals:
    void emitLoadAudioIn(QString fName);
};

#endif // AUDIO_H
