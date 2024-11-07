#include "spectrogram.h"
#include <QtWidgets>
Spectrogram::Spectrogram() {
    setScene(&scene);
    setSceneRect(QRectF(0.0, 0.0, 400.0, 300.0));
    setMinimumSize(QSize(400, 300));
    setRenderHint(QPainter::Antialiasing, true);
    scene.addRect(sceneRect());
}

void Spectrogram::setLength(qint64 _length) {
    length = _length;
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

