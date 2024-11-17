#include "wavform.h"
#include "wavfile.h"
#include <QLineSeries>
#include <QtCharts>
#include <QtWidgets>

WavForm::WavForm()
{
    setScene(&scene);
    setMinimumSize(QSize(400, 200));
    setRenderHint(QPainter::Antialiasing, true);
    chart = new QChart();
    chart->setMinimumSize(400, 200);
    chart->legend()->hide();
    chart->setMargins(QMargins(0, 0, 0, 0));
    scene.addItem(chart);
    scene.addRect(sceneRect());
}
void WavForm::uploadAudio(QString fName){

    WavFile *audio = new WavFile(fName);
    audioToChart(audio);
}

void WavForm::audioToChart(WavFile* audio){
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

}


//scrubber

// void Spectrogram::setLength(qint64 _length) {
//     length = _length;
// }

// void Spectrogram::audioChanged(qint64 position) {
//     double x = (double) position / (double) length * 400;
//     if (x > 400) {
//         return;
//     }
//     if (lineHasBeenDrawn) scene.removeItem((QGraphicsItem *) currentLine);

//     QPointF *first = new QPointF(x, 0);
//     QPointF *second = new QPointF(x, 299);
//     currentLine = scene.addLine(QLineF(*first, *second), QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
//     lineHasBeenDrawn = true;
// }


// void Spectrogram::mousePressEvent(QMouseEvent *evt) {

//     QGraphicsView::mousePressEvent(evt);

//     QPointF center = mapToScene(evt->pos());

//     double x = center.x();

//     if (lineHasBeenDrawn) scene.removeItem((QGraphicsItem *) currentLine);

//     QPointF *first = new QPointF(x, 0);
//     QPointF *second = new QPointF(x, 299);
//     currentLine = scene.addLine(QLineF(*first, *second), QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
//     lineHasBeenDrawn = true;

//     qint64 correspondingPosition = x / 400 * length;
//     emit sendAudioPosition(correspondingPosition);


// }
