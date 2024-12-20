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
 *  - 'updateTrackPositionFromTimer()': Advances the audio position periodically based on the media player position, with elements of smoothing for a better UI
 *  - 'updateTrackPositionFromScrubber(double position)': Adjusts the player position when the scrubber is moved
 *  - 'ZoomScrubberPosition()': adjusts the scrubber when there is a zoom update
 *  - 'updateAudioPosition(qint64 duration)': Updates audio track duration
 *  - 'AudioLoaded()': allows for the segmenting functionality to start after audio has been loaded in
 *  - 'updateTrackPositionFromSegment(QPair<double, double> startEnd)': updates the audio to play the displayed segment
 *  - 'segmentIntervalControlsEnable(bool ready)': enables interval controls after segments are started
 *  - 'void segmentLengthShow(int numSamples, int sampleRate)': displayes number of samples and samples divided by sampleRate on segmentLengthLabel
 *  - 'segmentCreateControlsEnable(bool ready)': enables the create button once segments are established
 *  - 'toggleBoolManualSegments(double position)': enables the clear button and sends updated delta data and indicates to use segments from the delta value
 *  - 'toggleBoolAutoSegments()': enables clear button and indicates the segments are the auto ones
 *  - 'watchForEndOfSegmentAudio(qint64 audioPosition)': watches for if the end of the indicated segment is reached if the segment play is on
 *  - 'handlePlayPauseButton()': deals with play pause specifically when the button for it is pressed (or if original audio needs to be paused/played)
 *  - 'clearSegmentsEnable(bool enable)': enable/disable the clear segments button
 *  - 'handleLoopClick()': if the loop action is clicked this hadles the logic to make sure audio is looped/ the button looks selected
 *  - 'void handleSpectWithPlay()': when the spect is loading while audio is playing, audio & time stop to prevent jumpy scrubber
 *  - 'void handleWavClearing()': if charts are in use and new audio is uploaded charts & data are cleared
 *  - 'void disableButtonsUntilAudio()': disables buttons until audio is loaded
 *  - 'void enableAudioAligning(bool enable)': gets signal to enable audio aligning checkbox if second audio exists
 *  - 'void disableAudioControls(bool disable)': disables audio2 controls if audio1 aligning checkbox is checked and only allows for scrubber actions on audio2 for user
 *  - 'void audioAligningSegmentControls(bool segEnabled)': if the audio aligning is on but user on audio1 wants to use segmenting, aligning is turned off
 *  - 'void switchControlsWithAlign(bool aligning)': forces follow scrubber on if aligning is on
 *
 * Notes:
 *  - 'WaveForm' class and 'Zoom' class are integrated for visualization and zoom functionality respectively,
 *    see 'wavform.h' and 'wavform.cpp' for 'WaveForm' implementation and 'zoom.h' and 'zoom.cpp' for 'Zoom implementation
 * References:
 *  - ...
 */

Audio::Audio(QWidget *parent, QString _label, int _audioDiviceNumber)
    : QWidget{parent}, audioDiviceNumber(_audioDiviceNumber), label(_label), segmentAudioPlaying(false), audio2aligned(false)
{
    newAudioPlayer();
    audioUploaded = false;
    disableButtonsUntilAudio();

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
    QVBoxLayout *wavFormSegmentControls = new QVBoxLayout();
    wavFormControls->addLayout(wavFormVertControls);
    wavFormControls->addLayout(wavFormSegmentControls);

    uploadAudioButton = new QPushButton("Upload");
    connect(uploadAudioButton, &QPushButton::clicked, this, &Audio::uploadAudio);
    audioControls->addWidget(uploadAudioButton, 0, Qt::AlignLeft);

    QAction *playAction = new QAction();
    connect(playAction, &QAction::triggered, this, &Audio::handlePlayPauseButton);

    if (audioDiviceNumber == 0) {
        playAction->setShortcut(Qt::Key_Space); // top audio play & pause: Space
    } else {
        playAction->setShortcut(Qt::SHIFT | Qt::Key_Space); // bottom audio: Shift+Space
    }

    playButton = new QToolButton;
    playButton->setDefaultAction(playAction);
    playButton->setIcon(QIcon(":/resources/icons/play.svg"));
    playButton->setEnabled(false);

    QVBoxLayout *playLoopControls = new QVBoxLayout();
    audioControls-> addLayout(playLoopControls);
    playLoopControls->addWidget(playButton,  0, Qt::AlignRight);
    displayAndControlsLayout = new QVBoxLayout();
    audioLayout->addLayout(displayAndControlsLayout);

    QAction *loopAction = new QAction();
    connect(loopAction, &QAction::triggered, this, &Audio::handleLoopClick);
    loopButton = new QToolButton;
    loopButton->setDefaultAction(loopAction);
    loopButton->setIcon(QIcon(":/resources/icons/loop.svg"));
    loopButton->setEnabled(false);
    loopButton->setCheckable(true);
    playLoopControls->addWidget(loopButton, 0, Qt::AlignRight);

    //follow scrubber
    followScrubber = new QCheckBox("follow scrubber");
    followScrubber->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
    followScrubber->setCheckState(Qt::Checked);
    followScrubber->setEnabled(false);
    playLoopControls->addWidget(followScrubber);

    //align audios
    alignAllAudioFocus = new QCheckBox("allign Audios");
    alignAllAudioFocus->setEnabled(false);
    if(audioDiviceNumber == 0){
        connect(alignAllAudioFocus, &QCheckBox::clicked, this, &Audio::switchControlsWithAlign);
        alignAllAudioFocus->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
        playLoopControls->addWidget(alignAllAudioFocus);

    }

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
    connect(followScrubber, &QCheckBox::checkStateChanged, wavChart, &WavForm::changeCenterOnScrubber);

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

    //Delta selector (change distance between segment lines when in segmenting control)
    deltaSelector = new QDoubleSpinBox();
    deltaSelector->setValue(10.0);
    deltaSelector-> setMinimum(10.0);
    deltaSelector->setMaximum(200);
    deltaSelector->setEnabled(false);

    //auto Segments button: finds max peaks to automatically segment
    autoSegmentButton = new QPushButton("auto");
    autoSegmentButton->setEnabled(false);

    connect(deltaSelector, &QDoubleSpinBox::valueChanged, this, &Audio::toggleBoolManualSegments);
    connect(autoSegmentButton, &QPushButton::clicked, this, &Audio::toggleBoolAutoSegments);

    connect(wavChart, &WavForm::segmentReady, this, &Audio::segmentIntervalControlsEnable);
    connect(wavChart, &WavForm::segmentLength, this, &Audio::segmentLengthShow);

    QHBoxLayout *deltaLayout = new QHBoxLayout();
    deltaLayout->addWidget(deltaSelector);
    deltaLayout->addWidget(autoSegmentButton);
    wavFormVertControls->addLayout(deltaLayout);

    //graph audio segments controls
    graphAudioSegments = new WaveFormSegments();
    createGraphSegmentsButton = new QPushButton("create segment graphs");
    createGraphSegmentsButton->setEnabled(false);
    wavFormVertControls->addWidget(createGraphSegmentsButton);
    connect(wavChart, &WavForm::audioFileLoadedTrue, this, &Audio::audioLoaded);

    segmentToolsCheckbox = new QCheckBox("segment controls");
    segmentToolsCheckbox->setEnabled(false);
    connect(segmentToolsCheckbox, &QCheckBox::clicked, wavChart, &WavForm::switchMouseEventControls);
    connect(segmentToolsCheckbox, &QCheckBox::clicked, this, &Audio::audioAligningSegmentControls);

    segmentLengthLabel = new QLabel();
    segmentLengthLabel->setVisible(false);

    connect(createGraphSegmentsButton, &QPushButton::clicked, wavChart, &WavForm::sendIntervalsForSegment);
    connect(wavChart, &WavForm::intervalsForSegments, graphAudioSegments, &WaveFormSegments::collectWavSegment);

    //clearing all intervals (for what we send to close analysis graphs)
    clearAllGraphSegmentsButton = new QPushButton("clear segments");
    clearAllGraphSegmentsButton->setEnabled(false);
    wavFormVertControls->addWidget(clearAllGraphSegmentsButton);
    connect(clearAllGraphSegmentsButton, &QPushButton::clicked, graphAudioSegments, &WaveFormSegments::clearAllWavSegments);
    connect(clearAllGraphSegmentsButton, &QPushButton::clicked, wavChart, &WavForm::clearIntervals);
    connect(wavChart, &WavForm::chartInfoReady, this, &Audio::segmentCreateControlsEnable);
    wavFormSegmentControls-> addWidget(segmentToolsCheckbox);
    wavFormSegmentControls-> addWidget(segmentLengthLabel);

    //Close Analysis Graphs
    segmentGraph = new SegmentGraph(WAVFORM_WIDTH, WAVFORM_HEIGHT);
    segmentGraph->setVisible(false);
    connect(segmentGraph, &SegmentGraph::clearSegmentsEnable, this, &Audio::clearSegmentsEnable);
    connect(wavChart, &WavForm::clearEnable, this,  &Audio::clearSegmentsEnable);
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

    handleSpectWithPlay();
    disableButtonsUntilAudio();
    QUrl aName = QFileDialog::getOpenFileUrl(this, "Select audio file");
    if (aName.isEmpty()) return;
    audioUploaded = true;
    disableButtonsUntilAudio();
    handleWavClearing();

    if (player) player = nullptr;
    if(audioOutput) audioOutput = nullptr;
    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    player->setSource(aName);
    audioOutput->setVolume(50);
    connect(player, &QMediaPlayer::positionChanged, this, &Audio::watchForEndOfSegmentAudio);

    playButton->setEnabled(true);
    loopButton->setEnabled(true);
    followScrubber->setEnabled(true);
    emit emitLoadAudioIn(aName.toLocalFile());
    zoomButtons->setEnabled(true);
    zoomButtons->resetZoom();
    setTrackPosition(player->position());

    // Connect scrubber to spectrograph updates
    connect(player, &QMediaPlayer::positionChanged, this, &Audio::updateTrackPositionFromTimer);

    // Connect to durationChanged signal to get the actual duration
    connect(player, &QMediaPlayer::durationChanged, this, &Audio::updateAudioDuration);

    // emit signal to notify the spectrograph
    emit audioFileSelected(aName.toLocalFile());

    if(audioDiviceNumber == 1){
        emit secondAudioExists(true);
    }

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
    emit playPauseActivated();
}


// this is a slot with no arguments that updates position automatically using setTrackPosition
void Audio::updateTrackPositionFromTimer() { 
    qint64 updateVal = player->position();

    if (updateVal == player->duration()) {
        updateVal = audioPosition + timerRefreshRate;
    } else if (updateVal != 0) {
        if (prevPosition == 0) {
            timeElapsed = updateVal / timerRefreshRate;
        } else {
            ++timeElapsed;
        }

        if (prevPosition != updateVal) {
            audioVelocity = updateVal / timeElapsed;
            qint64 propVal = audioPosition + audioVelocity;

            if (propVal < updateVal) {
                ++audioVelocity;
            } else if (propVal > updateVal) {
                --audioVelocity;
            }

            updateVal = propVal;
        } else {
            updateVal = audioPosition + audioVelocity;
        }
    }

    prevPosition = player->position();

    setTrackPosition(updateVal);
}

void Audio::updateTrackPositionFromScrubber(double position) {
    double scaledPosition = (double) (position * audioLength);
    player->setPosition(scaledPosition);
    setTrackPosition(scaledPosition);
    segmentAudioPlaying = false;
    emit segmentAudioNotPlaying(true);
    emit scrubberUpdate(position);
    if (alignAllAudioFocus->isChecked()) {
        if (audioPlaying) handlePlayPauseButton();
        emit audioEnded(false);
    }
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

void Audio::handleLoopClick(){
    loopButton->setChecked(!loopButton->isChecked());
}

void Audio::toggleBoolManualSegments(double position) {
    autoSegmentBool = false;
    wavChart->updateDelta(position);
    if (!clearAllGraphSegmentsButton->isEnabled())clearAllGraphSegmentsButton->setEnabled(true);

    emit emitAutoSegmentBool(autoSegmentBool);

}

void Audio::toggleBoolAutoSegments() {
    autoSegmentBool = true;
    if (!clearAllGraphSegmentsButton->isEnabled())clearAllGraphSegmentsButton->setEnabled(true);

    emit emitAutoSegmentBool(autoSegmentBool);
}


void Audio::ZoomScrubberPosition(){
    double floatPosition = (double) audioPosition / audioLength;
    emit audioPositionChanged(floatPosition);
}

void Audio::setTrackPosition(qint64 position) {
    audioPosition = position;
    double floatPosition = (double) audioPosition / audioLength;
    if (floatPosition < 1.0 & abs(audioPosition - player->position()) > 100) {
        audioPosition = player->position();
    }

    if(floatPosition > 1.05 && (alignAllAudioFocus->isChecked() || audio2aligned)){
        QIcon icon = QIcon(":/resources/icons/play.svg");
        playButton->setIcon(icon);
        audioPlaying = false;
        emit segmentAudioNotPlaying(true);
        player->pause();
        timer->stop();
        if(audio2aligned) {
            emit audioEnded(true);
            emit audioPositionChanged(floatPosition);
            return;
        }
        emit audioEnded(false);
        emit audioPositionChanged(floatPosition);
        return;
    }

    // set to 1.05 so i don't accidentally trigger with pausing right before end
    //  the timer and player position can be out of sync
    if (floatPosition > 1.05 && !loopButton->isChecked()){
        floatPosition = 0;
        QIcon icon = QIcon(":/resources/icons/play.svg");
        playButton->setIcon(icon);
        audioPlaying = false;
        emit segmentAudioNotPlaying(true);
        player->pause();
        timer->stop();
    }
    if(loopButton->isChecked() && floatPosition > 1){
        player->play();
        timer->start(timerRefreshRate);
        setTrackPosition(player->position());

    }
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
    segmentLengthLabel->setVisible(ready);
}

void Audio::segmentLengthShow(int numSamples, int sampleRate) {
    double timeSecs = double(numSamples) / double (sampleRate);
    QString formattedTime = QString::number(timeSecs, 'f', 3);
    segmentLengthLabel->setText(QString("Length of segment:\n%1 samples\nor %2 seconds").arg(numSamples).arg(formattedTime));
}

void Audio::segmentCreateControlsEnable(bool ready){
    createGraphSegmentsButton->setEnabled(ready);
    audioPositionOnChart = player? player->position(): 0;

}

void Audio::clearSegmentsEnable(bool enable){
    clearAllGraphSegmentsButton->setEnabled(enable);
}

// when the spect is loading while audio is playing
// audio & time should stop to prevent jumpy scrubber
void Audio::handleSpectWithPlay() {
    // if player exists and playing
    if (player && player->playbackState() == QMediaPlayer::PlayingState) {
        player->stop();  // stop playback
        timer->stop();   // stop timer updates
        audioPosition = 0;  // reset track pos to beginning
        emit audioPositionChanged(0.0);

        // update pay button to state play
        audioPlaying = false;
        playButton->setIcon(QIcon(":/resources/icons/play.svg"));
    }
}

// if charts in use and new audio is uploaded charts & data should be cleaned up
void Audio::handleWavClearing() {
    if (!segmentGraph) return;
    if (!wavChart) return;
    if (segmentGraph || wavChart ) {
        segmentGraph->clearView();
        wavChart->clearIntervals();
    }
}


void Audio::disableButtonsUntilAudio() {

    autoSegmentButton->setEnabled(audioUploaded);
    clearAllGraphSegmentsButton->setEnabled(audioUploaded);
    createGraphSegmentsButton->setEnabled(audioUploaded);
    deltaSelector->setEnabled(audioUploaded);
    zoomButtons->setEnabled(audioUploaded);
    verticalSlider->setEnabled(audioUploaded);
    horizontalSlider->setEnabled(audioUploaded);
    segmentToolsCheckbox->setEnabled(audioUploaded);
}

void Audio::enableAudioAligning(bool enable){ /////////////////////////////////////////////
    alignAllAudioFocus->setEnabled(enable);
}

void Audio::switchControlsWithAlign(bool aligning){
    loopButton->setEnabled(!aligning);
    followScrubber->setEnabled(!aligning);
    followScrubber->setChecked(true);
}

void Audio::audioAligningSegmentControls(bool segEnabled){
    if(alignAllAudioFocus->isEnabled() && segEnabled) {
        alignAllAudioFocus->click();
    }
    else if(!segEnabled && audio2aligned){
        alignAllAudioFocus->setEnabled(true);
        qDebug() << "audioAligningSegmentControls callled";
    }

}

void Audio::disableAudioControls(bool disable){
    if(disable){
        graphAudioSegments->clearAllWavSegments();
        wavChart->clearIntervals();
        segmentToolsCheckbox->setChecked(false);
        wavChart->switchMouseEventControls(false);
    }
    audio2aligned = disable;
    if (autoSegmentButton->isEnabled()) autoSegmentButton->setDisabled(disable);
    if (clearAllGraphSegmentsButton->isEnabled()) clearAllGraphSegmentsButton->setDisabled(disable);
    if (createGraphSegmentsButton->isEnabled()) createGraphSegmentsButton->setDisabled(disable);
    if (deltaSelector->isEnabled()) deltaSelector->setDisabled(disable);
    zoomButtons->setDisabled(disable);
    verticalSlider->setDisabled(disable);
    horizontalSlider->setDisabled(disable);
    segmentToolsCheckbox->setDisabled(disable);
    playButton->setDisabled(disable);
    loopButton->setDisabled(disable);
    followScrubber->setDisabled(disable);
}
