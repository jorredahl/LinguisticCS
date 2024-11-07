#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QObject>
#include <QtWidgets>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "spectrogram.h"


class Visualizer : public QWidget
{
    Q_OBJECT
public:
    Visualizer();
    QPushButton *uploadAudioButton;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    bool audioPlaying;
    QToolButton *playButton;
    QHBoxLayout *layout;
    Spectrogram *spectrogram;

public slots:
    void uploadAudio();
    void handlePlayPause();
};

#endif // VISUALIZER_H
