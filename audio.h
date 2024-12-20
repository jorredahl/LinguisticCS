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
 *  - 'void updateTrackPositionFromSegment(QPair<double, double> startEnd)': updates the audio to play the displayed segment
 *  - 'void ZoomScrubberPosition()': adjusts the scrubber when there is a zoom update
 *  - 'void updateAudioPosition(qint64 duration)': Updates audio track duration
 *  - 'void audioLoaded()': allows for the segmenting functionality to start after audio has been loaded in
 *  - 'void segmentIntervalControlsEnable(bool ready)': enables interval controls after segments are started
 *  - 'void segmentLengthShow(int numSamples, int sampleRate)': displays selected segment length in samples and seconds on segmentLengthLabel
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
 *  - 'void enableAudioAligning(bool enable)': gets signal to enable audio aligning checkbox if second audio exists
 *  - 'void disableAudioControls(bool disable)': disables audio2 controls if audio1 aligning checkbox is checked and only allows for scrubber actions on audio2 for user
 *  - 'void audioAligningSegmentControls(bool segEnabled)': if the audio aligning is on but user on audio1 wants to use segmenting, aligning is turned off
 *  - 'void switchControlsWithAlign(bool aligning)': forces follow scrubber on if aligning is on
 * Signals:
 *  - 'void emitLoadAudioIn(QString fName)': Emits signal when an audio file is uploaded
 *  - 'void audioPositionChanged(double position)': Emits signal when the audio position is changed
 *  - 'void segmentAudioNotPlaying(bool)': emits when the segment audio is playing/not to update what the player is doing or segment ui
 *  - 'void audioFileSelected(const QString &fileName)': tells spectrograph that the audio is loaded
 *  - signals for audio aliging so that audio2 can do whatever audio1 does:
 *      - 'void playPauseActivated()'
 *      - 'void scrubberUpdate(double position)': position of audio1 scrubber updated
 *      - 'void audioEnded(bool disconnect)': when either audio ends is updated signal for audio 2 to disconnect/connect from/to 1 until play/pause or scrubber is activated
 * Notes:
 *  - The 'Audio' class relies on the 'WavForm', 'SegmentGraph', 'WavFormSegments', and 'Zoom' classes for waveform visualization and zooming
 *
 * References:
 *  - ...
 */

class Audio : public QWidget
{
    Q_OBJECT
    int audioDiviceNumber;
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
    qint64 prevPosition;
    int timeElapsed;
    int timeElapsedUpdate;
    int audioVelocity;
    qint64 audioPosition;
    qint64 audioPositionOnChart;
    qint64 audioLength;
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
    bool audio2aligned;

    WaveFormSegments *graphAudioSegments;

    bool autoSegmentBool;
    bool spectrographReadyFlag;
    bool audioUploaded = false;


public:
    explicit Audio(QWidget *parent = nullptr, QString _label = "Sound Wave", int _audioDiviceNumber = 0);
    WavForm *wavChart;
    void newAudioPlayer();
    void setTrackPosition(qint64 position);
    QCheckBox *alignAllAudioFocus;
    Zoom *zoomButtons;

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

    void enableAudioAligning(bool enable);
    void disableAudioControls(bool disable);
    void audioAligningSegmentControls(bool segEnabled);
    void switchControlsWithAlign(bool aligning);

signals:
    void emitLoadAudioIn(QString fName);
    void audioPositionChanged(double position);
    void emitAutoSegmentBool(bool autoSegmentBool);
    void segmentAudioNotPlaying(bool);
    void secondAudioExists(bool);
    void audioFileSelected(const QString &fileName); // for connecting spectrograph
    void playPauseActivated();
    void scrubberUpdate(double position);
    void audioEnded(bool disconnect);


};

#endif // AUDIO_H
