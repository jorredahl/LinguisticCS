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

void WaveFormSegments::collectWavSegment(QList<int> segmentPlaces, bool isAuto){
    // only one line given
    clearAllWavSegments();

    if (isAuto) {
        autoSegment(originalAudio.mid(segmentPlaces[0], segmentPlaces[1] - segmentPlaces[0]), segmentPlaces[0]);

    }

    audioSampleLength = originalAudio.length();

    for (int segmentIndx = 0; segmentIndx < segmentPlaces.length() - 1; segmentIndx ++) {
        double startOfSegment = (double) (segmentPlaces[segmentIndx] / audioSampleLength);
        double endOfSegment = (double) (segmentPlaces[segmentIndx + 1] / audioSampleLength);
        QList<float> wavSegment = originalAudio.mid(segmentPlaces[segmentIndx], abs(segmentPlaces[segmentIndx + 1] - segmentPlaces[segmentIndx]) + 1);
        wavSegments << wavSegment;
        wavSegmentStartEndPositions << QPair<double, double> (startOfSegment, endOfSegment);
    }
    if (isAuto) wavSegments.remove(wavSegments.length() - 1);
    if (isAuto) wavSegmentStartEndPositions.remove(wavSegments.length() - 1);
    emit storeStartEndValuesOfSegments(wavSegmentStartEndPositions);
    emit createWavSegmentGraphs(wavSegments);



}

void WaveFormSegments::clearAllWavSegments(){
    if (!wavSegments.isEmpty()) wavSegments.clear();
    if (!wavSegmentStartEndPositions.isEmpty()) wavSegmentStartEndPositions.clear();
}


void WaveFormSegments::uploadAudio(QList<float> audio){
    if (!originalAudio.empty()){
        originalAudio.clear();
    }
    originalAudio = audio;
    clearAllWavSegments();
}

void WaveFormSegments::autoSegment(QList<float> dataSample, int startIndex) {
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
        while(zeroCrossings[i + 1] - zeroCrossings[i] < (dataSample.length() / 100) && zeroCrossings.length() - 1 > i + 1) {
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

    QList<int> trueLocalMaxs;
    for (int i = 0; i < localMaxs.length(); ++i) {
        trueLocalMaxs << startIndex + localMaxs[i];
    }

    collectWavSegment(trueLocalMaxs, false); //now it is defined segments so we send false for auto

    emit drawAutoSegments(localMaxs);
}


