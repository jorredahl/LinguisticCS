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

    setChart(samples);

}

void WavForm::setChart(QList<qint16> data) {

    int width = 800;
    int height = 300;

    scene.clear();

    int sampleLength = data.length() / width;

    QList<float> avgs = QList<float>(width);
    QList<float> mins = QList<float>(width);
    QList<float> maxs = QList<float>(width);
    QList<float> rms = QList<float>(width);

    for (int i = 0; i < width; ++i) {
        float sum = 0;
        float squareSum = 0;
        float min = 1.0;
        float max = -1.0;
        for (int j = 0; j < sampleLength; ++j) {
            float adjusted = data[j + i * sampleLength] / 32768.0;
            if (adjusted < min) min = adjusted;
            if (adjusted > max) max = adjusted;
            sum += abs(adjusted);
            squareSum += pow(adjusted, 2);
        }
        mins[i] = min;
        maxs[i] = max;
        avgs[i] = sum / sampleLength;
        rms[i] = sqrt(squareSum / sampleLength);
    }

    for (int i = 0; i < width; ++i) {
        scene.addRect(QRect(i, (height / 2) - ((abs(maxs[i]) * height) / 2), 1, (abs(maxs[i]) * height) / 2), Qt::NoPen, Qt::darkBlue);
        scene.addRect(QRect(i, height / 2, 1, abs(mins[i]) * height / 2), Qt::NoPen, Qt::darkBlue);
        scene.addRect(QRect(i, (height / 2) - ((rms[i] * height) / 2), 1, rms[i] * height), Qt::NoPen, Qt::blue);
        scene.addRect(QRect(i, (height / 2) - ((avgs[i] * height) / 2), 1, avgs[i] * height), Qt::NoPen, QColor(QRgb(0x8888FF)));
    }

    setSceneRect(0,0,width,height);

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
