#ifndef WAVEFORMSEGMENTS_H
#define WAVEFORMSEGMENTS_H

#include <QObject>

/*
 * File: waveformsegments.h
 * Description:
 *  This header file defines the 'WaveFormSegments' class, this class recieves places to make segments of audio data,
 *  uses them to make lists of the audio sample data of the given segments, and sends them to be graphed.
 *
 * Purpose:
 *  - this class connects the audio graph information to
 *  graphs of user defined segments that allow for closer analysis of raw audio information.
 *
 * Key Members:
 *  -  'QList<QList<float>> wavSegments': holds the wavSegments to be emitted
    -  'QList<float> originalAudio': the original sample audio data to chop up
 *
 * Public Methods:
 *  - 'uploadAudio(QList<float> audio)': gives the object the audio data to slice
 *
 * Slots:
 *  - 'void collectWavSegment(QList<int> segmentPlaces)': collects wav segments and divides audio graph based on segment location
 *  - 'void clearAllWavSegments()': clears segment information (to be used when user clears segments from graph)
 *  - 'void autoSegment(QList<float> dataSample)': creates automated points in the audio wave for segementation
 *
 *Signals:
 *  - 'createWavSegmentGraphs(QList<QList<float>>)' : tells the detailed graphs to make them from the wavSegments
 * Notes:
 *  - This works in tandum with SegmentGraph and user input on the graph closely, all changes to those should involve
 *      double checking the functionality here is not compromised
 *
 */
class WaveFormSegments : public QObject
{
    Q_OBJECT
    QList<QList<float>> wavSegments;
    QList<QPair<double, double>> wavSegmentStartEndPositions;
    QList<float> originalAudio;
    double audioSampleLength;

public:
    explicit WaveFormSegments(QList<float> _audioSamples = QList<float>(), QObject *parent = nullptr);
    void uploadAudio(QList<float> audio);

public slots:
    void collectWavSegment(QList<int> segmentPlaces,  bool isAuto);
    void clearAllWavSegments();
    void autoSegment(QList<float> dataSample, int startIndex);

signals:
    void createWavSegmentGraphs(QList<QList<float>>);
    void drawAutoSegments(QList<int>);
    void storeStartEndValuesOfSegments(QList<QPair<double, double>>);
};

#endif // WAVEFORMSEGMENTS_H
