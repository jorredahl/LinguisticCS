#include "visualizer.h"
#include <QtWidgets>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "spectrogram.h"


Visualizer::Visualizer() {
    // create upload button


    layout = new QHBoxLayout();
    setLayout(layout);
    uploadAudioButton = new QPushButton("Upload");
    layout->addWidget(uploadAudioButton, 0, Qt::AlignCenter);
    connect(uploadAudioButton, &QPushButton::clicked, this, &Visualizer::uploadAudio);

    QAction *playAction = new QAction();
    connect(playAction, &QAction::triggered, this, &Visualizer::handlePlayPause);

    playButton = new QToolButton;
    playButton->setDefaultAction(playAction);
    playButton->setIcon(QIcon(":/resources/icons/play.svg"));

    //connect(playButton, &QToolButton::triggered, this, &MainWindow::handlePlayPause);
    layout->addWidget(playButton);

    QLabel *nativeWaveLabel = new QLabel("Speaker Sound Wave"); // using QLabel as a placeholder for waveforms
    //nativeWaveLabel->setAlignment(Qt::AlignCenter);
    nativeWaveLabel->setStyleSheet("border: 1px solid black; min-height: 100px;");
    layout->addWidget(nativeWaveLabel);

    spectrogram = new Spectrogram();
    layout->addWidget(spectrogram);
    /* should be a qgraphicsview with an underlying scene
     * we add the pixmap of wavelength to the scene with the same size
     * then the line can be drawn using the same mouse click functionality as before
     * and go through when not being played */


    /* we can do spectrogram then generalize later
     * so it is derived from qgraphics view, just like scribbler, initialized with .wav or pixmap
     * has a slot to take in current position of audio file when playing
     * so audio file emits signal when position changes.  or has internal timer that is started when plaing?
     * basically can we continually access state from audio file or keep track ourselves?
     * efficiency vs accuracy
     *
     * so cur/len is percent across the screen of bar
     * we either delete and redraw everytime or move the rec.  probably equivalent
     * when there is a click we get x pos and move bar there, emit signal to change pos of song
     * so need to keep these synced at all times
     * ok bet there is a positionChanged signal.  so we connect that to a slot that draws the bar
     *

*/


}

//Visualizer::~Visualizer() {};

void Visualizer::uploadAudio(){
    QUrl aName = QFileDialog::getOpenFileUrl(this, "Select audio file");
    if (aName.isEmpty()) return;

    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    // ...
    player->setSource(aName);
    audioOutput->setVolume(50);
    //player->play();

    connect(player, &QMediaPlayer::positionChanged, this->spectrogram, &Spectrogram::audioChanged);



}

void Visualizer::handlePlayPause() {
    qDebug() << "Hi";
    audioPlaying = !audioPlaying;
    QIcon icon = audioPlaying ? QIcon(":/resources/icons/play.svg") : QIcon(":/resources/icons/pause.svg");
    playButton->setIcon(icon);
    player->setPosition(0);

    audioPlaying ? player->pause() : player->play();




}
