#include "waveformsegments.h"
#include <QtCore/qdebug.h>

/*
 * File: waveformsegments.cpp
 * Description:
 *  This source file implements the step between getting the user added segments from the graph to the 'SegmentGraph'
 *  (visualizing the samples from the audio data). This takes the given wavfile indx/s and creates the segment
 *  of that wav data given a start and end indx. A given segment starts at a start indx and the segment ends
 *  at the next indx. These segments are sent as a list of lists (segments).
 *      If only one line is given to segment audio: index 0 to given line index, then given line to end are the two creates segments.
 *      If more than one line is given to segment audio:  start1 line is first line, end1 line is second, start2 is second line, end2 line is 3rd line...
 *  once the segments are made they are sent to be  graphed in 'SegmentGraph'.
 *
 * Slots:
 *  - 'void collectWavSegment(QList<int> segmentPlaces)': does most of what is described above, takes indexes to divide the
 *  audio graph information and creates segments from them to be graphed.
 *  - 'void clearAllWavSegments()': clears the wav segments out if user resets the lines
 *  - 'void uploadAudio(QList<float> audio)': uploads new audio to be sliced upon recieving segmentPlaces from collectWavSegment
 *  - 'void autoSegment(QList<float> dataSample)': creates automated segments based of local maximums and sents the indeces
 *
 * Notes:
 *  - this does not delete individual segments, it only takes in all that need to be made, makes them, then sends them off
 *
 */
WaveFormSegments::WaveFormSegments(QList<float> _audioSamples , QObject *parent)
    : QObject{parent}, originalAudio(_audioSamples)
{}

void WaveFormSegments::collectWavSegment(QList<int> segmentPlaces){
    // only one line given
    clearAllWavSegments();

    qDebug() << segmentPlaces.length();

    if (segmentPlaces.length() == 2) {
        autoSegment(originalAudio.mid(segmentPlaces[0], segmentPlaces[1] - segmentPlaces[0]));
        //qDebug() << "auto";

    }
    //else qDebug() << "manual";

    if(segmentPlaces.length() == 1){
        if (segmentPlaces[0] != 0) {
            QList<float> wavSegment1 = originalAudio.mid(0, segmentPlaces[0] + 1);
            wavSegments << wavSegment1;
        }
        if (segmentPlaces[0] != originalAudio.length() - 1){
            QList<float> wavSegment2 = originalAudio.mid(segmentPlaces[0]);
            wavSegments << wavSegment2;
        }
        return;
    }

    //more than one segment line
    //std::sort(segmentPlaces.begin(), segmentPlaces.end()); //sort incase lines are sent out of order

    for (int segmentIndx = 0; segmentIndx < segmentPlaces.length() - 1; segmentIndx ++) {
        //qDebug() << "segment";
        //qDebug() << segmentPlaces[segmentIndx];
        //qDebug() << abs(segmentPlaces[segmentIndx + 1] - segmentPlaces[segmentIndx]) + 1;
        QList<float> wavSegment = originalAudio.mid(segmentPlaces[segmentIndx], abs(segmentPlaces[segmentIndx + 1] - segmentPlaces[segmentIndx]) + 1);
        wavSegments << wavSegment;
        //qDebug() << wavSegment;

    }
    emit createWavSegmentGraphs(wavSegments);
}

void WaveFormSegments::clearAllWavSegments(){
    if (!wavSegments.isEmpty()) wavSegments.clear();
}


void WaveFormSegments::uploadAudio(QList<float> audio){
    if (!originalAudio.empty()){
        originalAudio.clear();
    }
    originalAudio = audio;
    clearAllWavSegments();
}

void WaveFormSegments::autoSegment(QList<float> dataSample) {
    QList<int> zeroCrossings;
    zeroCrossings << 0;
    for (int i = 0; i < dataSample.length() - 1; ++i) {
        if ((dataSample[i] > 0.0 && dataSample[i + 1] < 0.0) || (dataSample[i] < 0.0 && dataSample[i + 1] > 0.0)) {
            zeroCrossings << i;
        }
    }
    zeroCrossings << dataSample.length() - 1;

    QList<QList<float>> localData;

    for (int i = 0; i < zeroCrossings.length() - 1; ++i) {
        while(zeroCrossings[i + 1] - zeroCrossings[i] < 50 && zeroCrossings.length() - 1 > i + 1) {
            zeroCrossings.remove(i + 1);
        }
        localData << dataSample.mid(zeroCrossings[i], zeroCrossings[i + 1] - zeroCrossings[i]);
    }

    QList<int> localMaxs;
    int sumLength = 0;
    for (int i = 0; i < localData.length(); ++i) {
        int maxIndex = 0;
        for (int j = 1; j < localData[i].length(); ++j) {
            if (localData[i][j] > localData[i][maxIndex]) maxIndex = j;
        }
        localMaxs << sumLength + maxIndex;
        sumLength += localData[i].length();
    }

    //qDebug() << localMaxs;

    collectWavSegment(localMaxs);

    emit drawAutoSegments(localMaxs);
}


