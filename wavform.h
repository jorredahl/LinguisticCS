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
    WavFile *audio;
    int chartW;
    int chartH;

public:
    explicit WavForm();
    void audioToChart();
    void setChart(QList<qint16> data, int width, int height);
public slots:
    void uploadAudio(QString fName);
    void updateScrubberPosition(double position);
    void updateChart(int width, int height);

protected:
    void mousePressEvent(QMouseEvent *evt) override;

signals:
    void sendAudioPosition(double position);
};

#endif // WAVFORM_H
