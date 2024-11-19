#include "audio.h"
#include <QWidget>
#include <QLabel>
#include "wavform.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>

Audio::Audio(QWidget *parent)
    : QWidget{parent}
{
    newAudioPlayer();
}
void Audio::newAudioPlayer(){
    audioLayout = new QHBoxLayout();
    setLayout(audioLayout);
    uploadAudioButton = new QPushButton("Upload");
    audioLayout->addWidget(uploadAudioButton, 0, Qt::AlignCenter);
    connect(uploadAudioButton, &QPushButton::clicked, this, &Audio::uploadAudio);

    QAction *playAction = new QAction();
    connect(playAction, &QAction::triggered, this, &Audio::handlePlayPause);
    playAction->setShortcut(Qt::Key_Space);
    playButton = new QToolButton;
    playButton->setDefaultAction(playAction);
    playButton->setIcon(QIcon(":/resources/icons/play.svg"));
    playButton->setEnabled(false);
    audioLayout->addWidget(playButton);

    zoomButtons = new Zoom();
    audioLayout->addWidget(zoomButtons);
    zoomButtons->setEnabled(false);

    //chart
    QVBoxLayout *chartLayout = new QVBoxLayout();
    audioLayout->addLayout(chartLayout);
    QLabel *nativeWaveLabel = new QLabel("Speaker Sound Wave"); // using QLabel as a placeholder for waveforms
    chartLayout->addWidget(nativeWaveLabel);

    wavChart = new WavForm();
    chartLayout->addWidget(wavChart);
    connect(this, &Audio::emitLoadAudioIn, wavChart, &WavForm::uploadAudio);
    connect(zoomButtons, &Zoom::zoomGraphIn, wavChart, &WavForm::updateChart);
    connect(zoomButtons, &Zoom::zoomGraphOut, wavChart, &WavForm::updateChart);

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
