#include "waveformsegments.h"

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
    std::sort(segmentPlaces.begin(), segmentPlaces.end()); //sort incase lines are sent out of order

    for (int segmentIndx = 0; segmentIndx < segmentPlaces.length() - 1; segmentIndx ++) {
        QList<float> wavSegment = originalAudio.mid(segmentPlaces[segmentIndx], abs(segmentPlaces[segmentIndx + 1] - segmentPlaces[segmentIndx]) + 1);
        wavSegments << wavSegment;
    }
    emit createWavSegmentGraphs(wavSegments);
}

void WaveFormSegments::clearAllWavSegments(){
    wavSegments.clear();
}
void WaveFormSegments::uploadAudio(QList<float> audio){
    if (!originalAudio.empty()){
        originalAudio.clear();
    }
    originalAudio = audio;
    clearAllWavSegments();
}
