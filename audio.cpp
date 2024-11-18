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

    playButton = new QToolButton;
    playButton->setDefaultAction(playAction);
    playButton->setIcon(QIcon(":/resources/icons/play.svg"));
    playButton->setEnabled(false);
    //connect(playButton, &QToolButton::triggered, this, &MainWindow::handlePlayPause);
    audioLayout->addWidget(playButton);

    QLabel *nativeWaveLabel = new QLabel("Speaker Sound Wave"); // using QLabel as a placeholder for waveforms
    //nativeWaveLabel->setAlignment(Qt::AlignCenter);
    //nativeWaveLabel->setStyleSheet("border: 1px solid black; min-height: 100px;");
    audioLayout->addWidget(nativeWaveLabel);

    wavChart = new WavForm();
    audioLayout->addWidget(wavChart);
    connect(this, &Audio::emitLoadAudioIn, wavChart, &WavForm::uploadAudio);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Audio::updateTrackPositionFromTimer);

    timerRefreshRate = 10;

    connect(this, &Audio::audioPositionChanged, wavChart, &WavForm::updateScrubberPosition);
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

    //old way of handeling the scrubber
    // Connect to durationChanged signal to get the actual duration
    connect(player, &QMediaPlayer::durationChanged, this, [=](qint64 duration) {
        audioLength = duration;
        qDebug() << duration << "duration";
     });

    //connect(player, &QMediaPlayer::positionChanged, this->spectrogram, &Spectrogram::audioChanged);
    //connect(this->spectrogram, &Spectrogram::sendAudioPosition, this, &Visualizer::changeAudioPosition);


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


void Audio::setTrackPosition(qint64 position) {
    audioPosition = position;
    //qDebug() << audioPosition;
    double floatPosition = (double) audioPosition / audioLength;
    //qDebug() << floatPosition;
    emit audioPositionChanged(floatPosition);
}
