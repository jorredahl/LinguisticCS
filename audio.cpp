#include "audio.h"
#include <QWidget>
#include <QLabel>
#include "wavform.h"
#include "waveformsegments.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>

#define WAVFORM_HEIGHT 200
#define WAVFORM_WIDTH 400

/*
 * File: audio.cpp
 * Description:
 *  This source file implements the 'Audio' class, providing the functionality for audio playback, upload,
 *  visualization, and control interactions. The class uses 'QMediaPlayer' and 'QAudioOutput' for audio
 *  playback and integrates waveform and zoom controls. The constructor 'Audio(QWidget *parent)' calls the
 *  'newAudioPlayer()' method which sets up the UI, including the upload and play buttons, waveform display,
 *  and zoom controls, and the audio playback is managed with 'QMediaPlayer' and supports scrubbing and
 *  zooming functionality.
 *
 * Key Methods:
 *  - 'newAudioPlayer()': Sets up the layout, buttons, waveform, and timer connections
 *  - 'uploadAudio()': Handles the file selection process and media player setup
 *  - 'handlePlayPause()': Manages the play/pause state of the audio player and updates the timer
 *  - 'setTrackPosition(qint64 position)': Updates the current track position and emits the
 *    'audioPositionChanged' signal
 *  - 'applySegmentInterval()': Updates waveform segments based on user-defined intervals
 *
 * Slots:
 *  - 'updateTrackPositionFromTimer()': Advances the audio position periodically based on the timer
 *  - 'updateTrackPositionFromScrubber(double position)': Adjusts the player position when the scrubber is moved
 *  - 'ZoomScrubberPosition()': adjusts the scrubber when there is a zoom update
 *  - 'updateAudioPosition(qint64 duration)': Updates audio track duration
 *  - 'AudioLoaded()': allows for the segmenting functionality to start after audio has been loaded in
 *
 * Notes:
 *  - 'WaveForm' class and 'Zoom' class are integrated for visualization and zoom functionality respectively,
 *    see 'wavform.h' and 'wavform.cpp' for 'WaveForm' implementation and 'zoom.h' and 'zoom.cpp' for 'Zoom implementation
 *  - if issues with the scrubber arise unconnect the 'updateAudioDuration(qint64 position)' and use what is below instead:
 *      -       //     connect(player, &QMediaPlayer::durationChanged, this, [=](qint64 duration) {
                //     audioLength = duration;
                //     qDebug() << duration << "duration";
                //  });
 *  - test for the segments is in the play/pause method
 * References:
 *  - ...
 */

Audio::Audio(QWidget *parent, QString _label)
    : QWidget{parent}, label(_label), segmentAudioPlaying(false)
{
    newAudioPlayer();
}
void Audio::newAudioPlayer(){
    audioLayout = new QHBoxLayout();
    setLayout(audioLayout);
    wavFormAudioControlsLayout = new QVBoxLayout();
    audioLayout-> addLayout(wavFormAudioControlsLayout);
    audioControls = new QHBoxLayout();
    wavFormControls = new QHBoxLayout();
    wavFormAudioControlsLayout-> addLayout(audioControls);
    wavFormAudioControlsLayout->addLayout(wavFormControls);
    QVBoxLayout *wavFormVertControls = new QVBoxLayout();
    wavFormControls->addLayout(wavFormVertControls);

// add constructor for iniitalizer of qlabel text

    // Upload: ctrl+U
    // Play/Pause: space
    // loop: ctrl+L

    uploadAudioButton = new QPushButton("Upload");
    uploadAudioButton->setShortcut(Qt::CTRL | Qt::Key_U);
    connect(uploadAudioButton, &QPushButton::clicked, this, &Audio::uploadAudio);
    audioControls->addWidget(uploadAudioButton, 0, Qt::AlignLeft);

    QAction *playAction = new QAction();
    connect(playAction, &QAction::triggered, this, &Audio::handlePlayPauseButton);
    playAction->setShortcut(Qt::Key_Space);
    playButton = new QToolButton;
    playButton->setDefaultAction(playAction);
    playButton->setIcon(QIcon(":/resources/icons/play.svg"));
    playButton->setEnabled(false);
    QVBoxLayout *playLoopControls = new QVBoxLayout();
    audioControls-> addLayout(playLoopControls);
    playLoopControls->addWidget(playButton);

    displayAndControlsLayout = new QVBoxLayout();
    audioLayout->addLayout(displayAndControlsLayout);

    // QLabel *nativeWaveLabel = new QLabel(label); // using QLabel as a placeholder for waveforms
    // //nativeWaveLabel->setScaledContents(true);
    // //displayAndControlsLayout->addWidget(nativeWaveLabel);


    QAction *loopAction = new QAction();
    //connect(loopAction, &QAction::triggered, this, &Audio::handleLoopClick);
    loopButton = new QToolButton;
    loopButton->setDefaultAction(loopAction);
    loopButton->setIcon(QIcon(":/resources/icons/loop.svg"));
    loopButton->setEnabled(false);
    loopButton->setShortcut(Qt::CTRL | Qt::Key_L);
    //connect(playButton, &QToolButton::triggered, this, &MainWindow::handlePlayPause);
    playLoopControls->addWidget(loopButton);

    //Zoom
    zoomButtons = new Zoom(nullptr, WAVFORM_WIDTH, WAVFORM_HEIGHT);
    zoomButtons->setEnabled(false);

    horizontalSlider = new QSlider();
    horizontalSlider->setEnabled(false);
    horizontalSlider->setOrientation(Qt::Horizontal);
    horizontalSlider->setMinimum(1);
    horizontalSlider->setMaximum(200);

    verticalSlider = new QSlider();
    verticalSlider->setOrientation(Qt::Vertical);
    verticalSlider->setMinimum(1);
    verticalSlider->setMaximum(10);
    verticalSlider->setEnabled(false);
    zoomButtons->setHorizontalSlider(horizontalSlider);
    zoomButtons->setVerticalSlider(verticalSlider);

    //Chart
    wavChart = new WavForm(WAVFORM_WIDTH, WAVFORM_HEIGHT);
    connect(this, &Audio::emitLoadAudioIn, wavChart, &WavForm::uploadAudio);
    connect(zoomButtons, &Zoom::zoomGraphIn, wavChart, &WavForm::updateChart);
    connect(this, &Audio::emitAutoSegmentBool, wavChart, &WavForm::changeBoolAutoSegment);

    QHBoxLayout *ChartAndVerticalSliderLayout = new QHBoxLayout();
    displayAndControlsLayout->addLayout(ChartAndVerticalSliderLayout);
    ChartAndVerticalSliderLayout->addWidget(wavChart);
    ChartAndVerticalSliderLayout->addWidget(verticalSlider);
    //zoom next to it
    displayAndControlsLayout->addWidget(horizontalSlider);

    //Timer/scrubber
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Audio::updateTrackPositionFromTimer);
    timerRefreshRate = 10;

    connect(this, &Audio::audioPositionChanged, wavChart, &WavForm::updateScrubberPosition);
    connect(this->wavChart, &WavForm::sendAudioPosition, this, &Audio::updateTrackPositionFromScrubber);

    connect(zoomButtons, &Zoom::zoomGraphIn, this, &Audio::ZoomScrubberPosition);

    //WavForm Controls
    deltaSelector = new QDoubleSpinBox();
    deltaSelector->setValue(10.0);
    deltaSelector-> setMinimum(10.0);
    deltaSelector->setMaximum(200);
    deltaSelector->setEnabled(false);
    autoSegmentButton = new QPushButton("auto");
    autoSegmentButton->setEnabled(false);


    connect(deltaSelector, &QDoubleSpinBox::valueChanged, this, &Audio::toggleBoolManualSegments);
    connect(autoSegmentButton, &QPushButton::clicked, this, &Audio::toggleBoolAutoSegments);

    connect (wavChart, &WavForm::segmentReady, this, &Audio::segmentIntervalControlsEnable);

    QHBoxLayout *deltaLayout = new QHBoxLayout();
    deltaLayout->addWidget(deltaSelector);
    deltaLayout->addWidget(autoSegmentButton);
    wavFormVertControls->addLayout(deltaLayout);

    graphAudioSegments = new WaveFormSegments();
    createGraphSegmentsButton = new QPushButton("create segment graphs");
    createGraphSegmentsButton->setEnabled(false);
    wavFormVertControls->addWidget(createGraphSegmentsButton);
    connect(wavChart, &WavForm::audioFileLoadedTrue, this, &Audio::audioLoaded);

    segmentToolsCheckbox = new QCheckBox("segment controls");
    segmentToolsCheckbox->setEnabled(false);
    connect(segmentToolsCheckbox, &QCheckBox::clicked, wavChart, &WavForm::switchMouseEventControls);

    connect(createGraphSegmentsButton, &QPushButton::clicked, wavChart, &WavForm::sendIntervalsForSegment);
    connect(wavChart, &WavForm::intervalsForSegments, graphAudioSegments, &WaveFormSegments::collectWavSegment);

    //clearing all intervals (for what we send to close analysis graphs)
    clearAllGraphSegmentsButton = new QPushButton("clear segments");
    clearAllGraphSegmentsButton->setEnabled(false);
    wavFormVertControls->addWidget(clearAllGraphSegmentsButton);
    connect(clearAllGraphSegmentsButton, &QPushButton::clicked, graphAudioSegments, &WaveFormSegments::clearAllWavSegments);
    connect(clearAllGraphSegmentsButton, &QPushButton::clicked, wavChart, &WavForm::clearIntervals);
    connect(wavChart, &WavForm::chartInfoReady, this, &Audio::segmentCreateControlsEnable);
    wavFormControls-> addWidget(segmentToolsCheckbox);
    //Close Analysis Graphs
    segmentGraph = new SegmentGraph(WAVFORM_WIDTH, WAVFORM_HEIGHT);
    segmentGraph->setVisible(false);
    connect(segmentGraph, &SegmentGraph::clearSegmentsEnable, this, &Audio::clearSegmentsEnable);
    connect(graphAudioSegments, &WaveFormSegments::createWavSegmentGraphs, segmentGraph, &SegmentGraph::updateGraphs);
    connect(graphAudioSegments, &WaveFormSegments::storeStartEndValuesOfSegments, segmentGraph, &SegmentGraph::getSegmentStartEnd);
    connect(graphAudioSegments, &WaveFormSegments::drawAutoSegments, wavChart, &WavForm::drawAutoIntervals);
    connect(wavChart, &WavForm::clearAllSegmentInfo, segmentGraph, &SegmentGraph::clearView);
    connect(wavChart, &WavForm::clearAllSegmentInfo, graphAudioSegments, &WaveFormSegments::clearAllWavSegments);
    connect(segmentGraph, &SegmentGraph::sendPlaySegmentAudio, this, &Audio::updateTrackPositionFromSegment);
    connect(this, &Audio::segmentAudioNotPlaying,segmentGraph, &SegmentGraph::changePlayPauseButton);
    audioLayout->addWidget(segmentGraph);

}

void Audio::uploadAudio(){
    QUrl aName = QFileDialog::getOpenFileUrl(this, "Select audio file");
    if (aName.isEmpty()) return;

    if (player) player = nullptr;
    if(audioOutput) audioOutput = nullptr;
    player = new QMediaPlayer;
    connect(player, &QMediaPlayer::positionChanged, this, &Audio::watchForEndOfSegmentAudio);
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    player->setSource(aName);
    audioOutput->setVolume(50);

    playButton->setEnabled(true);
    loopButton->setEnabled(true);
    emit emitLoadAudioIn(aName.toLocalFile());
    zoomButtons->setEnabled(true);
    zoomButtons->resetZoom();
    setTrackPosition(player->position());

    // Connect to durationChanged signal to get the actual duration
    connect(player, &QMediaPlayer::durationChanged, this, &Audio::updateAudioDuration);

    // emit signal to notify the spectrograph
    emit audioFileSelected(aName.toLocalFile());

    //enable everything
    verticalSlider->setEnabled(true);
    horizontalSlider->setEnabled(true);
    segmentToolsCheckbox->setEnabled(true);

}
void Audio::handlePlayPauseButton(){
    QIcon icon = audioPlaying ? QIcon(":/resources/icons/play.svg") : QIcon(":/resources/icons/pause.svg");
    playButton->setIcon(icon);
    if (segmentAudioPlaying){
        player->setPosition(audioPositionOnChart);
        segmentAudioPlaying = false;
    }
    handlePlayPause();
    emit segmentAudioNotPlaying(true);
}
void Audio::handlePlayPause() {

    if (audioPlaying) {
        player->pause();
        timer->stop();
    }
    else {
        player->play();
        timer->start(timerRefreshRate);

    }
    setTrackPosition(player->position());
    audioPlaying = !audioPlaying;

    //TEST FOR WAVSEGMENTING
    //graphAudioSegments->collectWavSegment(QList<int> () << 100 << 5000 << 10000 << 15000 << 20000);

}


// this is a slot with no arguments that updates position automatically using setTrackPosition
void Audio::updateTrackPositionFromTimer() {
    setTrackPosition(audioPosition + timerRefreshRate);
}

void Audio::updateTrackPositionFromScrubber(double position) {
    qint64 intPosition = (qint64) (position * audioLength);
    setTrackPosition(intPosition);
    player->setPosition(intPosition);
    segmentAudioPlaying = false;
    emit segmentAudioNotPlaying(true);

}

void Audio::updateTrackPositionFromSegment(QPair<double, double> startEnd){
    segmentAudioStartPosition = (qint64) (startEnd.first * audioLength);
    setTrackPosition(segmentAudioStartPosition);
    player->setPosition(segmentAudioStartPosition);

    segmentAudioPlaying= true;
    segmentAudioEndPosition = (qint64)(startEnd.second * audioLength);
    handlePlayPause();


}
void Audio::watchForEndOfSegmentAudio(qint64 audioPos){
    if(!segmentAudioPlaying) return;
    if (audioPos >= segmentAudioEndPosition){
        setTrackPosition(segmentAudioStartPosition);
        player->setPosition(segmentAudioStartPosition);
    }
}

void Audio::toggleBoolManualSegments(double position) {
    autoSegmentBool = false;
    wavChart->updateDelta(position);
    if (!createGraphSegmentsButton->isEnabled()) createGraphSegmentsButton->setEnabled(true);
    if (!clearAllGraphSegmentsButton->isEnabled())clearAllGraphSegmentsButton->setEnabled(true);

    emit emitAutoSegmentBool(autoSegmentBool);

}

void Audio::toggleBoolAutoSegments() {
    autoSegmentBool = true;

    if(!createGraphSegmentsButton->isEnabled()) createGraphSegmentsButton->setEnabled(true);
    if (!clearAllGraphSegmentsButton->isEnabled())clearAllGraphSegmentsButton->setEnabled(true);

    emit emitAutoSegmentBool(autoSegmentBool);
}


void Audio::ZoomScrubberPosition(){
    double floatPosition = (double) audioPosition / audioLength;
    if (floatPosition < 1.0 & abs(audioPosition - player->position()) > 100) {
        audioPosition = player->position();
    }
    emit audioPositionChanged(floatPosition);
}

void Audio::setTrackPosition(qint64 position) {
    audioPosition = position;
    double floatPosition = (double) audioPosition / audioLength;
    if (floatPosition < 1.0 & abs(audioPosition - player->position()) > 100) {
        audioPosition = player->position();
    }


    // set to 1.05 so i don't accidentally trigger with pausing right before end
    //  the timer and player position can be out of sync
    if (floatPosition > 1.05) handlePlayPauseButton();

    emit audioPositionChanged(floatPosition);
}

void Audio::updateAudioDuration(qint64 duration){
    audioLength = duration;
}

void Audio::audioLoaded(){
    graphAudioSegments->uploadAudio(wavChart->getSamples());
}

// we want the segments to be whatever the last button hit was
// so when auto is clicked it makes them auto
void Audio::segmentIntervalControlsEnable(bool ready){
    deltaSelector->setEnabled(ready);
    autoSegmentButton->setEnabled(ready);
}



void Audio::segmentCreateControlsEnable(bool ready){
    createGraphSegmentsButton->setEnabled(ready);
    audioPositionOnChart = player->position();
}

void Audio::clearSegmentsEnable(bool enable){
    clearAllGraphSegmentsButton->setEnabled(enable);
}
