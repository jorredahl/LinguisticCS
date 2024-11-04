#include "visualizer.h"
#include <QtWidgets>
#include <QMediaPlayer>
#include <QAudioOutput>

Visualizer::Visualizer() {
    // create upload button


    /* layout = new QHBoxLayout();
    setLayout(layout);
    uploadAudioButton = new QPushButton("Upload");
    layout->addWidget(uploadAudioButton, 0, Qt::AlignCenter);
    connect(uploadAudioButton, &QPushButton::clicked, this, &Visualizer::uploadAudio);

    QAction *playAction = new QAction();
    connect(playAction, &QAction::triggered, this, &Visualizer::handlePlayPause);

    playButton = new QToolButton;
    playButton->setIcon(QIcon(":/resources/icons/play.svg"));
    playButton->setDefaultAction(playAction);

    //connect(playButton, &QToolButton::triggered, this, &MainWindow::handlePlayPause);
    layout->addWidget(playButton); */





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
    player->play();


}

void Visualizer::handlePlayPause() {
    qDebug() << "Hi";
    audioPlaying = !audioPlaying;
    //audioPlaying ? player->pause() : player->play();
    QIcon icon = audioPlaying ? QIcon(":/resources/icons/play.svg") : QIcon(":/resources/icons/pause.svg");
    playButton->setIcon(icon);



}
