#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QMediaPlayer>
#include <QToolButton>
#include <QBoxLayout>
#include "wavform.h"
#include "zoom.h"
#include "segmentgraph.h"
#include "waveformsegments.h"

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
 *  - Provides interaction with scrubber and segment graph visualizations
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
        QVBoxLayout *wavFormAudioControlsLayout;
        QHBoxLayout *audioControls;
        QHBoxLayout *wavFormControls;
        QTimer *timer;
        int timerRefreshRate;
        qint64 audioPosition;
        qint64 audioLength;
        Zoom *zoomButtons;
        SegmentGraph *segmentGraph;
        QVBoxLayout *displayAndControlsLayout;
        QHBoxLayout *controlsLayout;
        QString label;
        QPushButton *addIntervalLines;
        QDoubleSpinBox *deltaSelector;
        QPushButton *applyDeltaInterval;
        QPushButton *createGraphSegmentsButton;
        QPushButton *clearAllGraphSegmentsButton;
        QCheckBox *segmentToolsCheckbox;
        QPushButton *autoSegmentButton;

        WaveFormSegments *graphAudioSegments;


        bool autoSegmentBool;


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
    void ZoomScrubberPosition();
    void updateAudioDuration(qint64 duration);
    void audioLoaded();
    void segmentIntervalControlsEnable(bool ready);
    void applySegmentInterval();
    void segmentCreateControlsEnable(bool ready);
    void toggleBoolManualSegments(double position);
    void toggleBoolAutoSegments();


signals:
    void emitLoadAudioIn(QString fName);
    void audioPositionChanged(double position);
    void emitAutoSegmentBool(bool autoSegmentBool);
    // for connecting spectrograph
    void audioFileSelected(const QString &fileName);
};

#endif // AUDIO_H
