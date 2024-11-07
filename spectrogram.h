#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H
#include <QGraphicsView>
#include <QWidget>

class Spectrogram : public QGraphicsView
{
    Q_OBJECT
    QGraphicsScene scene;
    qint64 length;
    QGraphicsLineItem *currentLine;
    bool lineHasBeenDrawn;

public:
    Spectrogram();
    void setLength(qint64 _length);
    //Spectrogram();


public slots:
    void audioChanged(qint64 position);
    void mousePressEvent(QMouseEvent *evt) override;
signals:
    void sendAudioPosition(qint64 pos);
};

#endif // SPECTROGRAM_H
