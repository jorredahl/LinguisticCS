#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QMediaPlayer>
#include <QToolButton>
#include <QHBoxLayout>
#include "wavform.h"
#include "zoom.h"

class Audio : public QWidget
{
    Q_OBJECT
        QPushButton *uploadAudioButton;
        QMediaPlayer *player;
        QAudioOutput *audioOutput;
        bool audioPlaying;
        QToolButton *playButton;
        QHBoxLayout *audioLayout;
        QTimer *timer;
        int timerRefreshRate;
        qint64 audioPosition;
        qint64 audioLength;
        Zoom *zoomButtons;
        QVBoxLayout *displayAndControlsLayout;


public:
    explicit Audio(QWidget *parent = nullptr);
    WavForm *wavChart;
    void newAudioPlayer();
    void setTrackPosition(qint64 position);

public slots:
    void uploadAudio();
    void handlePlayPause();
    void updateTrackPositionFromTimer();
    void updateTrackPositionFromScrubber(double position);




signals:
    void emitLoadAudioIn(QString fName);
    void audioPositionChanged(double position);
};

#endif // AUDIO_H
