#include "spectrogram.h"
#include <QtWidgets>
#include <QtCharts>
Spectrogram::Spectrogram() {
    setScene(&scene);
    setMinimumSize(QSize(MIN_W, MIN_H));
    setRenderHint(QPainter::Antialiasing, true);
    scene.addRect(sceneRect());
}



// void Spectrogram::setChart(QChartView *chartView) {
//     qDebug() << "set chart initialized";
//     //holds the QChart
//     chartView->setMinimumSize(MIN_W, MIN_H);
//     chartView->setAlignment(Qt::AlignLeft | Qt::AlignTop);

//     //makes the chart a scrollable area
//     chartScrollArea = new QScrollArea();
//     chartScrollArea->setWidget(chartView);
//     chartScrollArea->setWidgetResizable(true);

//     chartWidget = scene.addWidget(chartScrollArea);
//     chartWidget->setMinimumSize(MIN_W, MIN_H);
//     qDebug() << "Spectrogram works";
// }
// void Spectrogram::resizeEvent(QResizeEvent *event) {
//     QGraphicsView::resizeEvent(event);
//     // resize chart
//     if (chartWidget) {
//         QSize newSize = viewport()->size();
//         chartScrollArea->resize(newSize);
//         chartWidget->resize(newSize);

//     }
// }





