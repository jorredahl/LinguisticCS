#ifndef WAVFORM_H
#define WAVFORM_H

#include <QWidget>
#include "wavfile.h"
#include <QtCharts>

class WavForm : public QGraphicsView
{
    Q_OBJECT
    QGraphicsScene scene;
    QChart *chart;

public:
    explicit WavForm();
    void audioToChart(WavFile* audio);
public slots:
    void uploadAudio(QString fName);
signals:
};

#endif // WAVFORM_H
