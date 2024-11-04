#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QObject>
#include <QtWidgets>
#include <QMediaPlayer>
#include <QAudioOutput>


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

public slots:
    void uploadAudio();
    void handlePlayPause();
};

#endif // VISUALIZER_H
