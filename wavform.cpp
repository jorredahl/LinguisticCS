#include "wavform.h"
#include "wavfile.h"
#include <QLineSeries>
#include <QtCharts>
#include <QtWidgets>

/*
 * File: wavform.cpp
 * Description:
 *  This source file implements the 'WavForm' class, a custom 'QGraphicsView' which visualizes audio
 *  waveforms. The class visalizes the waveform using audio data from the 'WavFile' class and provides
 *  methods for updating the chart. The constructor 'WavForm(int _width, int _height): viewW(_width), viewH(_height)'
 *  initializes the 'QGraphicsView' and scene dimensions.
 *
 * Key Methods:
 *  - 'uploadAudio()': Creates a 'WavFile' object and sets up its waveform visualizatio with 'audioToChart()'
 *  - 'audioToChart()': Processes audio data and sets up the waveform chart
 *  - 'setChart()': Splits audio data into pixel-width length and calculates average, min, max and
 *    RMS values for each sample segment
 *  - 'mousePressEvent()': Maps mouse clicks to waveform positions and adds scrubber line to position
 *  - 'updateScrubberPosition()': Moves the scrubber based on position
 *
 * Notes:
 *  - Dark blue represents min and max values for each sample segment
 *  - ...
 *
 * References:
 *  - ...
 */

WavForm::WavForm(int _width, int _height): viewW(_width), viewH(_height)
{
    setScene(&scene);
    setMinimumSize(QSize(viewW, viewH));

    setRenderHint(QPainter::Antialiasing, true);
    setSceneRect(0, 0, viewW, viewH); // Explicitly set scene rect to match view
    scene.addRect(sceneRect());
    audioFileLoaded = false;
}
void WavForm::uploadAudio(QString fName){

    audio = new WavFile(fName);
    audioToChart();
}

void WavForm::audioToChart(){
    QList<qint16> samples;

    //verify we can load in file then get audio samples
    if(audio->loadFile()) {
        samples = audio->getAudioSamples();
    }

    chartW = viewW;
    chartH = viewH * 0.95;
    setChart(samples, chartW, chartH);
}

void WavForm::setChart(QList<qint16> data, int width, int height) {

    //draw the new chart with given samples in the given window width and height

    scene.clear();

    // splits data into samples for each pixel of width
    int sampleLength = data.length() / width;

    QList<float> avgs = QList<float>(width);
    QList<float> mins = QList<float>(width);
    QList<float> maxs = QList<float>(width);
    QList<float> rms = QList<float>(width);

    // finds max value, min value, average value, and root mean square of each sample
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

    // visualization: min/max is darkest, then rms, then average. May need to change some placing if the zoom is enough that a sample covers only positive/negative values
    for (int i = 0; i < width; ++i) {
        scene.addRect(QRect(i, (height / 2) - ((abs(maxs[i]) * height) / 2), 1, (abs(maxs[i]) * height) / 2), Qt::NoPen, Qt::darkBlue);
        scene.addRect(QRect(i, height / 2, 1, abs(mins[i]) * height / 2), Qt::NoPen, Qt::darkBlue);
        scene.addRect(QRect(i, (height / 2) - ((rms[i] * height) / 2), 1, rms[i] * height), Qt::NoPen, Qt::blue);
        scene.addRect(QRect(i, (height / 2) - ((avgs[i] * height) / 2), 1, avgs[i] * height), Qt::NoPen, QColor(QRgb(0x8888FF)));
    }

    setSceneRect(0,0,width,height);



}

void WavForm::updateChart(int width, int height){
    //clear old chart and update with the same samples but different width/height
    QList<qint16> samples = audio->getAudioSamples();
    scene.clear();
    scene.update();

    chartW = width;
    chartH = height;

    setChart(samples, width, height);
}


void WavForm::mousePressEvent(QMouseEvent *evt) {

    QGraphicsView::mousePressEvent(evt);

    if (!audioFileLoaded) return;

    QPointF center = mapToScene(evt->pos());

    double x = center.x();

    if (scrubberHasBeenDrawn) scene.removeItem((QGraphicsItem *) lastLine);

    QPointF *first = new QPointF(x, 0);
    QPointF *second = new QPointF(x, chartH-1);
    lastLine = scene.addLine(QLineF(*first, *second), QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
    scrubberHasBeenDrawn = true;

    centerOnScrubber = false; // if we click somewhere to change audio we don't want to keep centering; gets distracting

    double position = x / chartW;
  
    emit sendAudioPosition(position);


    }

void WavForm::updateScrubberPosition(double position) {


    if (position < 0.05) centerOnScrubber = true; //if starting from beginning we want to center on scrubber
    int scenePosition = (int) (position * chartW);
  
    if (scrubberHasBeenDrawn) scene.removeItem((QGraphicsItem *) lastLine);

    QPointF *first = new QPointF(scenePosition, 0);
    QPointF *second = new QPointF(scenePosition, chartH);

    lastLine = scene.addLine(QLineF(*first, *second), QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));

    if (centerOnScrubber) centerOn(lastLine);

    scrubberHasBeenDrawn = true;

}
