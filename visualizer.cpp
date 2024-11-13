#include "visualizer.h"
#include <QtWidgets>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QtCharts/QLineSeries>
#include "spectrogram.h"
#include "wavfile.h"


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
    playButton->setEnabled(false);
    //connect(playButton, &QToolButton::triggered, this, &MainWindow::handlePlayPause);
    layout->addWidget(playButton);

    QLabel *nativeWaveLabel = new QLabel("Speaker Sound Wave"); // using QLabel as a placeholder for waveforms
    //nativeWaveLabel->setAlignment(Qt::AlignCenter);
    nativeWaveLabel->setStyleSheet("border: 1px solid black; min-height: 100px;");
    layout->addWidget(nativeWaveLabel);

    spectrogram = new Spectrogram();
    chart = new QChart();
    layout->addWidget(spectrogram);
    /* should be a qgraphicsview with an underlying scene
     * we add the pixmap of wavelength to the scene with the same size
     * then the line can be drawn using the same mouse click functionality as before
     * and go through when not being played */




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

    //ripped this from chatGPT: couldn't figure out why plain duration wasn't working
    // Connect to durationChanged signal to get the actual duration
    connect(player, &QMediaPlayer::durationChanged, this, [=](qint64 duration) {
        spectrogram->setLength(duration);
        qDebug() << duration << "duration";
    });

    connect(player, &QMediaPlayer::positionChanged, this->spectrogram, &Spectrogram::audioChanged);
    connect(this->spectrogram, &Spectrogram::sendAudioPosition, this, &Visualizer::changeAudioPosition);
    playButton->setEnabled(true);

    WavFile *audio = new WavFile(aName.toLocalFile());
    audioToChart(audio);


}

void Visualizer::handlePlayPause() {
    QIcon icon = audioPlaying ? QIcon(":/resources/icons/play.svg") : QIcon(":/resources/icons/pause.svg");
    playButton->setIcon(icon);
    //player->setPosition(0);

    audioPlaying ? player->pause() : player->play();

    audioPlaying = !audioPlaying;
}

void Visualizer::changeAudioPosition(qint64 pos) {
    player->setPosition(pos);
}

void Visualizer::audioToChart(WavFile* audio){
    QByteArray audioData;
    QList<qint16> samples;

    if(audio->loadFile()) {
        audioData = audio->getAudioData();
        samples = audio->getAudioSamples();
    }

    QLineSeries *series = new QLineSeries();
    // Zoom level one can be every hundredth of a second(?)
    for (int i = 0; i < samples.length(); i += samples.length()/1000) {
        series->append(i / 441, samples[i]);
    }
    chart->legend()->hide();
    chart->addSeries(series);
    spectrogram->setChart(new QChartView(chart));
}
