#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H
#include <QGraphicsView>
#include <QtCharts/QChartView>
#include <QWidget>

class Spectrogram : public QGraphicsView
{
    Q_OBJECT
    QGraphicsScene scene;
    qint64 length;
    QGraphicsLineItem *currentLine;
    bool lineHasBeenDrawn;
    QScrollArea *chartScrollArea;
    QGraphicsWidget *chartWidget;
    bool resizing = false;

    //min chart size
    const int MIN_W = 700;
    const int MIN_H = 300;
    //max chart size
    const int MAX_W = 3000;
    const int MAX_H = 500;

public:
    Spectrogram();
    void setLength(qint64 _length);
    void setChart(QChartView *chartView);
    //Spectrogram();


public slots:
    void audioChanged(qint64 position);
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *evt) override;
signals:
    void sendAudioPosition(qint64 pos);
};

#endif // SPECTROGRAM_H
