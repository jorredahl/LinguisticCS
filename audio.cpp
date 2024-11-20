#include "audio.h"
#include <QWidget>
#include <QLabel>
#include "wavform.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>

#define WAVFORM_HEIGHT 200
#define WAVFORM_WIDTH 400

Audio::Audio(QWidget *parent, QString _label)
    : QWidget{parent}, label(_label)
{
    newAudioPlayer();
}
void Audio::newAudioPlayer(){
    audioLayout = new QHBoxLayout();
    setLayout(audioLayout);

// add constructor for iniitalizer of qlabel text

    // Upload: ctrl+U
    // Play/Pause: space
    // loop: ctrl+L

    uploadAudioButton = new QPushButton("Upload");
    uploadAudioButton->setShortcut(Qt::CTRL | Qt::Key_U);
    connect(uploadAudioButton, &QPushButton::clicked, this, &Audio::uploadAudio);
    audioLayout->addWidget(uploadAudioButton, 0, Qt::AlignCenter);

    QAction *playAction = new QAction();
    connect(playAction, &QAction::triggered, this, &Audio::handlePlayPause);
    playAction->setShortcut(Qt::Key_Space);
    playButton = new QToolButton;
    playButton->setDefaultAction(playAction);
    playButton->setIcon(QIcon(":/resources/icons/play.svg"));
    playButton->setEnabled(false);
    audioLayout->addWidget(playButton);


    displayAndControlsLayout = new QVBoxLayout();
    audioLayout->addLayout(displayAndControlsLayout);

    QLabel *nativeWaveLabel = new QLabel(label); // using QLabel as a placeholder for waveforms
    displayAndControlsLayout->addWidget(nativeWaveLabel);

    controlsLayout = new QHBoxLayout();
    displayAndControlsLayout->addLayout(controlsLayout);

    QAction *loopAction = new QAction();
    //connect(loopAction, &QAction::triggered, this, &Audio::handleLoopClick);
    loopButton = new QToolButton;
    loopButton->setDefaultAction(loopAction);
    loopButton->setIcon(QIcon(":/resources/icons/loop.svg"));
    loopButton->setEnabled(false);
    loopButton->setShortcut(Qt::CTRL | Qt::Key_L);
    //connect(playButton, &QToolButton::triggered, this, &MainWindow::handlePlayPause);
    controlsLayout->addWidget(loopButton);

    //Zoom
    zoomButtons = new Zoom(nullptr, WAVFORM_WIDTH, WAVFORM_HEIGHT);
    zoomButtons->setEnabled(false);
    controlsLayout->addWidget(zoomButtons);

    //Chart
    wavChart = new WavForm(WAVFORM_WIDTH, WAVFORM_HEIGHT);
    connect(this, &Audio::emitLoadAudioIn, wavChart, &WavForm::uploadAudio);
    connect(zoomButtons, &Zoom::zoomGraphIn, wavChart, &WavForm::updateChart);

    displayAndControlsLayout->addWidget(wavChart);

    //Timer/scrubber
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Audio::updateTrackPositionFromTimer);
    timerRefreshRate = 10;

    connect(this, &Audio::audioPositionChanged, wavChart, &WavForm::updateScrubberPosition);
    connect(this->wavChart, &WavForm::sendAudioPosition, this, &Audio::updateTrackPositionFromScrubber);
}

void Audio::uploadAudio(){
    QUrl aName = QFileDialog::getOpenFileUrl(this, "Select audio file");
    if (aName.isEmpty()) return;

    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    player->setSource(aName);
    audioOutput->setVolume(50);

    playButton->setEnabled(true);
    loopButton->setEnabled(true);
    emit emitLoadAudioIn(aName.toLocalFile());
    zoomButtons->setEnabled(true);

    //old way of handeling the scrubber
    // Connect to durationChanged signal to get the actual duration
    connect(player, &QMediaPlayer::durationChanged, this, [=](qint64 duration) {
        audioLength = duration;
        qDebug() << duration << "duration";
     });

}

void Audio::handlePlayPause() {
    QIcon icon = audioPlaying ? QIcon(":/resources/icons/play.svg") : QIcon(":/resources/icons/pause.svg");
    playButton->setIcon(icon);

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
}


// this is a slot with no arguments that updates position automatically using setTrackPosition
void Audio::updateTrackPositionFromTimer() {
    setTrackPosition(audioPosition + timerRefreshRate);
}

void Audio::updateTrackPositionFromScrubber(double position) {
    qint64 intPosition = (qint64) (position * audioLength);
    setTrackPosition(intPosition);
    player->setPosition(intPosition);
}

void Audio::setTrackPosition(qint64 position) {
    audioPosition = position;
    //qDebug() << audioPosition;
    double floatPosition = (double) audioPosition / audioLength;


    // set to 1.05 so i don't accidentally trigger with pausing right before end
    //  the timer and player position can be out of sync
    if (floatPosition > 1.05) handlePlayPause();

    //qDebug() << floatPosition;
    emit audioPositionChanged(floatPosition);
}
