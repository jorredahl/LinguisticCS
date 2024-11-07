#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H
#include <QGraphicsView>
#include <QWidget>

class Spectrogram : public QGraphicsView
{
    Q_OBJECT
    QGraphicsScene scene;
    qint64 length;


public:
    Spectrogram(qint64 _length);
    //Spectrogram();


public slots:
    void audioChanged(qint64 position);
};

#endif // SPECTROGRAM_H
