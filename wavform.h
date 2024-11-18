#ifndef WAVFORM_H
#define WAVFORM_H

#include <QWidget>
#include "wavfile.h"
#include <QtCharts>

class WavForm : public QGraphicsView
{
    Q_OBJECT
    QGraphicsScene scene;
    bool scrubberHasBeenDrawn;
    QGraphicsLineItem *lastLine;
    //QChart *chart;

public:
    explicit WavForm();
    void audioToChart(WavFile* audio);
    void setChart(QList<qint16> data);
public slots:
    void uploadAudio(QString fName);
    void updateScrubberPosition(double position);
signals:
};

#endif // WAVFORM_H
