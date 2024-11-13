#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QObject>
#include <QtWidgets>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QtCharts/QChart>
#include "spectrogram.h"
#include "wavfile.h"


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

    QChart *chart;
    void audioToChart(WavFile* audio);

public slots:
    void uploadAudio();
    void handlePlayPause();
    void changeAudioPosition(qint64 pos);

signals:
    void emitLoadAudioIn(QString fName);
};

#endif // VISUALIZER_H
