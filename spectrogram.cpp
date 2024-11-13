#include "spectrogram.h"
#include <QtWidgets>
#include <QtCharts>
Spectrogram::Spectrogram() {
    setScene(&scene);
    setMinimumSize(QSize(MIN_W, MIN_H));
    setRenderHint(QPainter::Antialiasing, true);
    scene.addRect(sceneRect());
}

void Spectrogram::setLength(qint64 _length) {
    length = _length;
}

void Spectrogram::setChart(QChartView *chartView) {

    //holds the QChart
    chartView->setMinimumSize(MIN_W, MIN_H);
    chartView->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    //makes the chart a scrollable area
    chartScrollArea = new QScrollArea();
    chartScrollArea->setWidget(chartView);
    chartScrollArea->setWidgetResizable(true);

    chartWidget = scene.addWidget(chartScrollArea);
    chartWidget->setMinimumSize(MIN_W, MIN_H);

}
void Spectrogram::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    // resize chart
    if (chartWidget) {
        QSize newSize = viewport()->size();
        chartScrollArea->resize(newSize);
        chartWidget->resize(newSize);

    }
}
void Spectrogram::audioChanged(qint64 position) {
    double x = (double) position / (double) length * 400;
    if (x > 400) {
        return;
    }
    if (lineHasBeenDrawn) scene.removeItem((QGraphicsItem *) currentLine);

    QPointF *first = new QPointF(x, 0);
    QPointF *second = new QPointF(x, 299);
    currentLine = scene.addLine(QLineF(*first, *second), QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
    lineHasBeenDrawn = true;
}


void Spectrogram::mousePressEvent(QMouseEvent *evt) {

    QGraphicsView::mousePressEvent(evt);

    QPointF center = mapToScene(evt->pos());

    double x = center.x();

    if (lineHasBeenDrawn) scene.removeItem((QGraphicsItem *) currentLine);

    QPointF *first = new QPointF(x, 0);
    QPointF *second = new QPointF(x, 299);
    currentLine = scene.addLine(QLineF(*first, *second), QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
    lineHasBeenDrawn = true;

    qint64 correspondingPosition = x / 400 * length;
    emit sendAudioPosition(correspondingPosition);


}

