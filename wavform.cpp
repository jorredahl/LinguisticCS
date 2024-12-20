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
 *  methods for updating the chart. It also supports user interaction, such as selecting segments, updating
 *  the scrubber, and managing intervals. The constructor 'WavForm(int _width, int _height): viewW(_width),
 *  viewH(_height)' initializes the 'QGraphicsView' and scene dimensions.
 *
 * Key Methods:
 *  - 'WavForm(int _width, int _height): Constructor initializes the view and scene dimensions and sets up
 *    default properties for the waveform visualization.
 *  - 'uploadAudio()': Creates a 'WavFile' object and sets up its waveform visualizatio with 'audioToChart()'
 *  - 'audioToChart()': Processes audio data and sets up the waveform chart
 *  - 'setChart()': Splits audio data into pixel-width length and calculates average, min, max and
 *    RMS values for each sample segment. When the width goes past the samples available in the wav file about (400 * 51),
 *    it switches to max and min and draws a line graph of (400*51*2 (2 for max and min)) points across the given width.
 *  - 'updateChart(int width, int height)': Redraws the chart with updated dimensions, preserving current segments
 *    and interval lines
 *  - 'mousePressEvent()': Maps mouse clicks  for user interactions such as adding scrubber line and setting segment
 *    start and end points
 *  - 'updateScrubberPosition()': Moves the scrubber based on given audio playback position
 *  - 'getSamples()': Returns list of audio samples currently loaded into the waveform
 *  - 'switchMouseEventControls(bool segmentControlsOn)': Enables segment selection mode when segmentControlsOn is true
 *    allowing the user to add start and end segment lines, and disables segment selection mode if false
 *  - 'drawIntervalLinesInSegment(double x)': Adds interval lines between the start and end segment spaced by a factor of delta
 *  - 'updateDelta(double _delta)': Updates _delta to redraw the spacing between intervals if segment line control is active
 *  - 'sendIntervalsForSegment()': Emits a list of audio sample indeces of line segment interval lines
 *  - 'clearIntervals()': Clears start and end segment lines and interval lines, resetting segment selection and emits
 *    signals signifying that the segment lines and chart information are nonexistent
 *
 * Notes:
 *  - Dark blue represents min and max values for each sample segment
 *  - ...
 *
 * References:
 *  -view rect: https://stackoverflow.com/questions/1355446/get-visible-rectangle-of-qgraphicsview
 *  - center of rect: https://doc.qt.io/qt-6/qrectf.html#center
 */

WavForm::WavForm(int _width, int _height): centerOnScrubber(true), viewW(_width), viewH(_height), segmentControls(false), scrubberRedraw(false)
{
    setScene(&scene);
    setMinimumSize(QSize(viewW, viewH));

    setRenderHint(QPainter::Antialiasing, true);
    setSceneRect(0, 0, viewW, viewH); // Explicitly set scene rect to match view
    //scene.addRect(sceneRect());
    audioFileLoaded = false;
}
void WavForm::uploadAudio(QString fName){

    audio = new WavFile(fName);
    scene.clear();
    scene.update();
    if(startSegment) startSegment = nullptr;
    if(endSegment) endSegment = nullptr;
    scrubberHasBeenDrawn = false;
    audioToChart();
    audioFileLoaded = true;
    emit audioFileLoadedTrue();
    clearIntervals();
}

void WavForm::audioToChart(){
    QList<float> samples;

    //verify we can load in file then get audio samples
    if(audio->loadFile()) {
        samples = audio->getAudioSamples();
    }

    chartW = viewW;
    chartH = viewH * 0.95;
    setChart(samples, chartW, chartH);

}

void WavForm::setChart(QList<float> data, int width, int height) {

    //draw the new chart with given samples in the given window width and height

    scene.clear();
    const int MAX_SAMPLES = (400 * 51); //if we reach this we stop drawing new samples
    int ogWidth = width; //need to store original width;
    width = std::min(width, MAX_SAMPLES); // whichever is smaller is what we draw to stop at max

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
            float adjusted = data[j + i * sampleLength];
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

    if (MAX_SAMPLES != width){
    // visualization: min/max is darkest, then rms, then average. May need to change some placing if the zoom is enough that a sample covers only positive/negative values
    for (int i = 0; i < width; ++i) {
        scene.addRect(QRect(i, (height / 2) - ((abs(maxs[i]) * height) / 2), 1, (abs(maxs[i]) * height) / 2), Qt::NoPen, Qt::darkBlue);
        scene.addRect(QRect(i, height / 2, 1, abs(mins[i]) * height / 2), Qt::NoPen, Qt::darkBlue);
        scene.addRect(QRect(i, (height / 2) - ((rms[i] * height) / 2), 1, rms[i] * height), Qt::NoPen, Qt::blue);
        scene.addRect(QRect(i, (height / 2) - ((avgs[i] * height) / 2), 1, avgs[i] * height), Qt::NoPen, QColor(QRgb(0x8888FF)));
        setSceneRect(0,0,width,height);
    }
    }else{
        //qDebug() << "width: "<< width << "height: " << height;
        //we are at max zoom threshold where we have to draw max and min as a line graph because we arent at one sample a pixel
        QPointF minPoint;
        QPointF maxPoint;

        float n = ogWidth / (float)MAX_SAMPLES; //step for how much to increment across the x axis in terms of the width/max_samples (like 4 samples across 5 pixels)
        float x = 0;

        QList<QPointF> pointList = QList<QPointF>(); //store points to draw lines between

        for (int i = 0; i < MAX_SAMPLES; ++i){
            minPoint = QPointF(x, (height / 2) + (abs(mins[i]) * height / 2));
            maxPoint = QPointF(x - (n/2),(height / 2) - (abs(maxs[i]) * height / 2));

            QPen pen(Qt::darkBlue);
            pen.setWidth(1);
            pointList.append(maxPoint);
            pointList.append(minPoint);
            x += n;
        }

        for (int i = 0; i < pointList.length() - 1; i++){
            QPen pen(Qt::darkBlue);
            pen.setWidth(1);
            QPointF p1 = pointList[i+1];
            QPointF p2 = pointList[i];

            scene.addLine(QLineF(p1, p2), pen);
        }
        setSceneRect(0,0,ogWidth,height);
    }

    scene.update();


}

void WavForm::updateChart(int width, int height){
    //clear old chart and update with the same samples but different width/height

    //save the old center point in scene
    QRectF oldViewRect = scene.views()[0]->mapToScene(scene.views()[0]->viewport()->geometry()).boundingRect();
    viewCenterPoint = QPointF((oldViewRect.center().x() / chartW) * width, height/2);

    QList<float> samples = audio->getAudioSamples();
    scene.clear();
    scene.update();
    scrubberHasBeenDrawn = false;

    int oldW = chartW;
    chartW = width;
    chartH = height;

    setChart(samples, width, height);

    //segment lines updates
    if(startSegment){
        double x = (startSegmentP.x() / oldW) * chartW;
        startSegmentP.setX(x);
        startSegment = scene.addLine(QLineF(startSegmentP, QPointF(startSegmentP.x(), chartH-1)), QPen(Qt::green, 3, Qt::SolidLine, Qt::FlatCap));
    }
    if(endSegment){
        double x = (endSegmentP.x() / oldW) * chartW;
        endSegmentP.setX(x);
        endSegment = scene.addLine(QLineF(endSegmentP, QPointF(endSegmentP.x(), chartH-1)), QPen(Qt::red, 3, Qt::SolidLine, Qt::FlatCap));
    }
    if((!startSegment|| !endSegment)){
        intervalLines.clear();
        intLinesX.clear();
    }
    if(startSegment && endSegment){
        updateIntervals(oldW);
        scene.update();
    }

    scrubberRedraw = false;
}


void WavForm::mousePressEvent(QMouseEvent *evt) {

    QGraphicsView::mousePressEvent(evt);
    if (!audioFileLoaded) return;

    QPointF center = mapToScene(evt->pos());

    double x = center.x();
    scrubberRedraw = false;

    if (segmentControls){
        if (startSegment && endSegment){
            scene.removeItem((QGraphicsItem *) startSegment);
            startSegment = nullptr;
            if(!intervalLines.isEmpty()){
                for(QGraphicsLineItem *l: intervalLines){
                    scene.removeItem((QGraphicsItem *)l);
                }
                intervalLines.clear();
                intLinesX.clear();
                emit chartInfoReady(false);
            }
        }

        if (!startSegment || endSegment){
            startSegmentP = QPointF(x,0);
            startSegment = scene.addLine(QLineF(startSegmentP, QPointF(x, chartH-1)), QPen(Qt::green, 3, Qt::SolidLine, Qt::FlatCap));
            if (endSegment) {
                endSegmentP = QPointF();
                scene.removeItem((QGraphicsItem *) endSegment);
                endSegment = nullptr;
                emit chartInfoReady(false);
            }
        }
        else {
            if (x > startSegmentP.x()){
                endSegmentP = QPointF(x,0);
                endSegment = scene.addLine(QLineF(endSegmentP, QPointF(x, chartH-1)), QPen(Qt::red, 3, Qt::SolidLine, Qt::FlatCap));
                emit chartInfoReady(true);
            }else{

                QMessageBox msgBox;
                msgBox.setText("The end of the segment cannot be before the start");
                msgBox.exec();

            }

        }
        emit segmentReady(startSegment && endSegment ? true: false);
        if (startSegment && endSegment) {
            float segmentProp = (endSegmentP.x() / chartW) - (startSegmentP.x() / chartW);
            int numSamples = int(audio->getAudioSamples().length() * segmentProp);
            emit segmentLength(numSamples, audio->getSampleRate());
        }
        emit clearEnable(startSegment || endSegment ? true: false);
        updateChart(chartW,chartH);
        return;
    }

    if (scrubberHasBeenDrawn) scene.removeItem((QGraphicsItem *) lastLine);

    QPointF *first = new QPointF(x, 0);
    QPointF *second = new QPointF(x, chartH-1);
    lastLine = scene.addLine(QLineF(*first, *second), QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
    scrubberHasBeenDrawn = true;

    double position = x / chartW;
    scrubberRedraw = true;

    emit sendAudioPosition(position);


    }

void WavForm::updateScrubberPosition(double position) {

    double scenePosition = (double) (position * chartW);

    if (scrubberHasBeenDrawn) scene.removeItem((QGraphicsItem *) lastLine);

    QPointF *first = new QPointF(scenePosition, 0);
    QPointF *second = new QPointF(scenePosition, chartH);

    lastLine = scene.addLine(QLineF(*first, *second), QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
    scrubberHasBeenDrawn = true;
    scene.update();
    if (scrubberRedraw) return;
    else if (centerOnScrubber) centerOn(lastLine);
    else centerOn(viewCenterPoint);



}

void WavForm::changeCenterOnScrubber(Qt::CheckState checkedState){
    if (checkedState == Qt::CheckState::Checked) centerOnScrubber = true;
    else centerOnScrubber = false;
}

 QList<float> WavForm::getSamples(){
     return audio->getAudioSamples();
}

void WavForm::switchMouseEventControls(bool segmentControlsOn){
    segmentControls = segmentControlsOn;
 }

void WavForm::drawIntervalLinesInSegment(double x){
    x += delta * chartW;
    while(x < endSegmentP.x()){
        intervalLines << scene.addLine(QLineF(QPointF(x,0), QPointF(x, chartH-1)), QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
        intLinesX << x;
        x+= delta * chartW;
    }
    emit chartInfoReady(true);
}

void WavForm::updateDelta(double _delta){
    delta = _delta / chartW;
    if (startSegment && endSegment) {
        if (!intervalLines.isEmpty()){
            for (QGraphicsLineItem *l : intervalLines){
                scene.removeItem(l);
                delete l;
            }
            intervalLines.clear();
            intLinesX.clear();
        }
        drawIntervalLinesInSegment(startSegmentP.x());
    }
}

void WavForm::updateIntervals(int oldChartWidth){
    if (!startSegment || !endSegment) return;
    if (intLinesX.isEmpty()) return;
    intervalLines.clear();
    QList<float> newIntLinesX;
    for (float x : intLinesX){
        newIntLinesX << (double)(x / oldChartWidth) * chartW;
    }
    intLinesX.clear();
    intLinesX = newIntLinesX;
    for (float x : intLinesX){
        QGraphicsLineItem *l = scene.addLine(QLineF(QPointF(x,0), QPointF(x , chartH-1)), QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
        intervalLines << l;

    }

}

void WavForm::changeBoolAutoSegment(bool _boolAutoSegment) {
    boolAutoSegment = _boolAutoSegment;
    if (boolAutoSegment) sendIntervalsForSegment();

}


void WavForm::sendIntervalsForSegment(){
    QList<int> intervalLocations;
    int audioLength = audio->getAudioSamples().length();
    intervalLocations << (startSegmentP.x()/chartW) * audioLength;

    if (!boolAutoSegment) {
        for (int indx = 0; indx < intLinesX.length(); indx++){
            intervalLocations << (intLinesX[indx]/chartW) * audioLength;
        }
    }

    intervalLocations << (endSegmentP.x()/chartW) * audioLength;
    emit intervalsForSegments(intervalLocations, boolAutoSegment);
}

void WavForm::drawAutoIntervals(QList<int> intervalLocsInAudio){

    //gets the positions (indxs) and put them on screen
    if (!intervalLines.isEmpty()){
        for (QGraphicsLineItem *l : intervalLines){
            scene.removeItem(l);
        }
        intervalLines.clear();
        intLinesX.clear();
    }
    int audioLength = audio->getAudioSamples().length();
    for (int indx = 0; indx < intervalLocsInAudio.length(); indx++){
        intLinesX << startSegmentP.x() + ((double) intervalLocsInAudio[indx] / audioLength) * chartW;
    }

    for (float x : intLinesX){
        intervalLines << scene.addLine(QLineF(QPointF(x ,0), QPointF(x , chartH-1)), QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
    }
}

void WavForm::clearIntervals(){

    if (startSegment) {
        scene.removeItem(startSegment);
        startSegment = nullptr;
    }
    if (endSegment) {
        scene.removeItem(endSegment);
        endSegment = nullptr;
    }
    if (!intervalLines.isEmpty()){
        for(QGraphicsLineItem *l : intervalLines){
            scene.removeItem(l);
        }
        intervalLines.clear();
    }
    if (!intLinesX.isEmpty())intLinesX.clear();
    emit segmentReady(false);
    emit chartInfoReady(false);
    emit clearAllSegmentInfo();


}
