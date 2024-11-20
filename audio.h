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

/*
 * File: audio.h
 * Description:
 *  This header file defines the 'Audio' class, which represents an audio playback and visualization
 *  widget and provides functionality for uploading, playing, pausing, and visualizing audio tracks
 *  using QMediaPlayer and QAudioOutput.
 *
 * Purpose:
 *  - Encapsulates the functionality required for audio interaction and visualization
 *  - Integrates with other classes like 'Wavform' and 'Zoom' for added functionality
 *  - Emits signals for audio position changes and file uploads to communicate to other widgets
 *
 * Key Members:
 *  - 'QPushButton *uploadAudioButton': Button for uploading audio files
 *  - 'QMediaPlayer *player': Media player for playing audio
 *  - 'QAudioOutput *audioOutput': Handles audio output
 *  - 'QToolButton *playButton': Button to toggle play/pause
 *  - 'WavForm *wavChart': Displays the wavForm of the audio file
 *  - 'Zoom *zoomButtons': Zoom controls for adjusting the waveform display
 *  - 'QTimer *timer': Timer for updating audio position during playback
 *
 * Public Methods:
 *  - 'Audio(QWidget *parent = nullptr)': Constructor to initialize audio widget
 *  - 'void newAudioPlayer()': Initializes UI components and layout
 *  - 'void setTrackPosition(qint64 position)': Updates the track position
 *
 * Public Slots:
 *  - 'void uploadAudio()': Opens a file dialog for selecting an audio file and initializes playback
 *  - 'void handlePlayPause()': Toggles between playing and pausing the audio
 *  - 'void updateTrackPositionFromTimer()': Updates track position based on timer
 *  - 'void updateTrackPositionFromScrubber(double position)': Updates track position from scrubbler movement
 *
 * Signals:
 *  - 'void emitLoadAudioIn(QString fName)': Emits signal when an audio file is uploaded
 *  - 'void audioPositionChanged(double position)': Emits signal when the audio position is changed
 *
 * Notes:
 *  - The 'Audio' class relies on the 'WavForm' and 'Zoom' classes for waveform visualization and zooming
 *
 * References:
 *  - ...
 */

class Audio : public QWidget
{
    Q_OBJECT
        QPushButton *uploadAudioButton;
        QMediaPlayer *player;
        QAudioOutput *audioOutput;
        bool audioPlaying;
        QToolButton *playButton;
        QToolButton *loopButton;
        QHBoxLayout *audioLayout;
        QTimer *timer;
        int timerRefreshRate;
        qint64 audioPosition;
        qint64 audioLength;
        Zoom *zoomButtons;
        QVBoxLayout *displayAndControlsLayout;
        QHBoxLayout *controlsLayout;
        QString label;


public:
    explicit Audio(QWidget *parent = nullptr, QString _label = "Sound Wave");
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
