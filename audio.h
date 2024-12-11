#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QMediaPlayer>
#include <QToolButton>
#include <QBoxLayout>
#include <QMediaCaptureSession>
#include <QAudioInput>
#include <QMediaRecorder>
#include <QAudioDecoder>
#include "wavform.h"
#include "zoom.h"
#include "segmentgraph.h"
#include "waveformsegments.h"
//what do I need to do for multimedia inclusion in .pro file(qml)?
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
 *  - 'void updateTrackPositionFromSegment(QPair<double, double> startEnd)': updates the audio to play the displayed segment
 *  - 'void ZoomScrubberPosition()': adjusts the scrubber when there is a zoom update
 *  - 'void updateAudioPosition(qint64 duration)': Updates audio track duration
 *  - 'void audioLoaded()': allows for the segmenting functionality to start after audio has been loaded in
 *  - 'void segmentIntervalControlsEnable(bool ready)': enables interval controls after segments are started
 *  - 'void segmentCreateControlsEnable(bool ready)': enables the create button once segments are established
 *  - 'void toggleBoolManualSegments(double position)': enables the clear button and sends updated delta data and indicates to use segments from the delta value
 *  - 'void toggleBoolAutoSegments()': enables clear button and indicates the segments are the auto ones
 *  - 'void watchForEndOfSegmentAudio(qint64 audioPosition)': watches for if the end of the indicated segment is reached if the segment play is on
 *  - 'void handlePlayPauseButton()': deals with play pause specifically when the button for it is pressed (or if original audio needs to be paused/played)
 *  - 'void clearSegmentsEnable(bool enable)': enable/disable the clear segments button
 *  - 'void handleLoopClick()': if the loop action is clicked this hadles the logic to make sure audio is looped/ the button looks selected
 *  - 'void handleSpectWithPlay()': when the spect is loading while audio is playing, audio & time stop to prevent jumpy scrubber
 *  - 'void handleWavClearing()': if charts are in use and new audio is uploaded charts & data are cleared
 *  - 'void disableButtonsUntilAudio()': disables buttons until audio is loaded
 *
 * Signals:
 *  - 'void emitLoadAudioIn(QString fName)': Emits signal when an audio file is uploaded
 *  - 'void audioPositionChanged(double position)': Emits signal when the audio position is changed
 *  - 'void segmentAudioNotPlaying(bool)': emits when the segment audio is playing/not to update what the player is doing or segment ui
 *  - 'void audioFileSelected(const QString &fileName)': tells spectrograph that the audio is loaded
 * Notes:
 *  - The 'Audio' class relies on the 'WavForm', 'SegmentGraph', 'WavFormSegments', and 'Zoom' classes for waveform visualization and zooming
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
        qint64 audioPositionOnChart;
        qint64 audioLength;
        Zoom *zoomButtons;
        SegmentGraph *segmentGraph;
        QVBoxLayout *displayAndControlsLayout;
        QString label;
        QPushButton *addIntervalLines;
        QDoubleSpinBox *deltaSelector;
        QPushButton *createGraphSegmentsButton;
        QPushButton *clearAllGraphSegmentsButton;
        QCheckBox *segmentToolsCheckbox;
        QLabel *segmentLengthLabel;
        QPushButton *autoSegmentButton;
        bool segmentAudioPlaying;
        qint64 segmentAudioEndPosition;
        qint64 segmentAudioStartPosition;
        QSlider *horizontalSlider;
        QSlider *verticalSlider;
        QCheckBox *followScrubber;

        WaveFormSegments *graphAudioSegments;

        bool autoSegmentBool;
        bool spectrographReadyFlag;
        bool audioUploaded = false;

        // for audio recording
        //QAudioRecorder *audioRecorder;
        QMediaCaptureSession *captureSession;
        QAudioInput *audioInput;
        QMediaRecorder *mediaRecorder;
        QAudioFormat *desiredFormat;
        QAudioDecoder *decoder;
        QPushButton *recordButton;
        bool isRecording; //to track recording state

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
    void updateTrackPositionFromSegment(QPair<double, double> startEnd);
    void ZoomScrubberPosition();
    void updateAudioDuration(qint64 duration);
    void audioLoaded();
    void segmentIntervalControlsEnable(bool ready);
    void segmentLengthShow(int numSamples, int sampleRate);
    void segmentCreateControlsEnable(bool ready);
    void toggleBoolManualSegments(double position);
    void toggleBoolAutoSegments();

    void watchForEndOfSegmentAudio(qint64 audioPosition);
    void handlePlayPauseButton();
    void clearSegmentsEnable(bool enable);
    void handleLoopClick();

    void handleSpectWithPlay();
    void handleWavClearing();
    void disableButtonsUntilAudio();
    // slot methods for audio recording
    void startRecording();
    void stopRecording();
    void processAudioData(const QByteArray &data);



signals:
    void emitLoadAudioIn(QString fName);
    void audioPositionChanged(double position);
    void emitAutoSegmentBool(bool autoSegmentBool);
    void  segmentAudioNotPlaying(bool);
    // for connecting spectrograph
    void audioFileSelected(const QString &fileName);
};

#endif // AUDIO_H
