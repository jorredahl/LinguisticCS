#include "spectrogram.h"

Spectrogram::Spectrogram(qint64 _length): length(_length) {
//Spectrogram::Spectrogram() {
    setScene(&scene);
    setSceneRect(QRectF(0.0, 0.0, 400.0, 300.0));
    setMinimumSize(QSize(400, 300));
    setRenderHint(QPainter::Antialiasing, true);
    scene.addRect(sceneRect());



}


void Spectrogram::audioChanged(qint64 position) {
    scene.clear();
    if (position > 400) {
        return;
    }
    QPointF *first = new QPointF((position / length) * 400, 0);
    QPointF *second = new QPointF((position / length) * 400, 299);
    scene.addLine(QLineF(*first, *second), QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
}
