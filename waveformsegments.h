#ifndef WAVEFORMSEGMENTS_H
#define WAVEFORMSEGMENTS_H

#include <QObject>

class WaveFormSegments : public QObject
{
    Q_OBJECT
    QList<QList<float>> *wavSegments;

public:
    explicit WaveFormSegments(QObject *parent = nullptr);

public slots:
    void collectWavSegment(QList<float>);
signals:
    void createWavSegmentGraphs(QList<QList<float>>);
};

#endif // WAVEFORMSEGMENTS_H
