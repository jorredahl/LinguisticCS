#ifndef WAVFORM_H
#define WAVFORM_H

#include <QWidget>
#include "wavfile.h"
#include <QtCharts>

/*
 * File: wavform.h
 * Description:
 *  This header file defines the 'WavForm' class, which is a custom 'QGraphicsView' that creates a
 *  waveform visualization for audio data using 'QGraphicsView'. This class renders an audio waveform using
 *  the data from a 'WavFile' object and allows user interaction such as scrubbing and segment selection
 *  with interval-based annotations.
 *
 * Purpose:
 *  - Creates a visual representation (waveform) of WAV audio data.
 *  - Supports user interaction like scrubbing and segment selection.
 *  - Emits signals to communicate playback position and interval/segment details.
 *
 * Key Members:
 *  - 'QGraphicsScene scene': The graphics scene used to render the waveform and scrubber.
 *  - `bool scrubberHasBeenDrawn`: Tracks whether the scrubber line has been added to the scene.
 *  - 'bool centerOnScrubber': Tracks whether view is centered on the scrubber.
 *  - 'bool audioFileLoaded': Tracks if an audio file was successfully loaded.
 *  - `QGraphicsLineItem *lastLine`: Pointer to the last scrubber line drawn.
 *  - `WavFile *audio`: Pointer to the associated `WavFile` object containing audio data.
 *  - `int viewW, viewH`: Dimensions of the `QGraphicsView` widget.
 *  - `int chartW, chartH`: Dimensions of the rendered waveform chart.
 *  - 'bool segmentControls': Tracks whether the user is in segment control mode to initiate start/end line selection.
 *  - 'QPointF startSegmentP, endSegmentP': Start and End point positions for a selected segment.
 *  - 'QGraphicsLineItem *startSegment, *endSegment': Graphic line items for start and end segment lines.
 *  - 'QList<QGraphicsLineItem*> intervalLines': List of interval lines within segment start/end lines.
 *  - 'double delta': Distance between intervals within user selected segments.
 *  - 'QList<float> intLinesX': List of x-coordinates of intervals within selected segment.
 *
 * Public Methods:
 *  - `explicit WavForm(int _width, int _height)`: Constructor initializing the view dimensions.
 *  - `void audioToChart()`: Loads audio data from the `WavFile` and creates a waveform visualization.
 *  - `void setChart(QList<float> data, int width, int height)`: Draws the waveform using audio sample data.
 *  - 'QList<float> getSamples()': Gets audio samples currently displayed in waveform.
 *  - 'void updateDelta(double delta)': Updates delta which calculates spacing between interval lines in segment selections.
 *
 * Slots:
 *  - `void uploadAudio(QString fName)`: Loads a WAV file and generates its waveform visualization.
 *  - `void updateScrubberPosition(double position)`: Updates the scrubber position based on a relative position.
 *  - `void updateChart(int width, int height)`: Redraws the chart to fit new dimensions.
 *  - 'void switchMouseEventControls(bool segmentControlsOn)': Enables or disables segment control mode.
 *  - 'void sendIntervalsForSegment()': Emits a list of audio samples indices for interval positions in segment selections.
 *  - 'void clearIntervals()': Clears segment and interval position data and graphic elements.
 *
 * Signals:
 *  - `void sendAudioPosition(double position)`: Emitted when the scrubber position changes.
 *  - `void sceneSizeChange()`: Emmited when scene size has been changed.
 *  - 'void audioFileLoadedTrue()': Emitted when an audio file is loaded.
 *  - 'void segmentReady(bool ready)': Emitted when start and end segment lines are declared.
 *  - 'void intervalsForSegments(QList<int> intervalLocations)': Emits a list of audio sample indices for interval lines.
 *  - 'void chartInfoReady(bool ready)': Emitted when segment selections and intervals lines are defined or cleared.
 *
 * Protected Methods:
 *  - `void mousePressEvent(QMouseEvent *evt) override`: Handles user interaction for updating the scrubber and segment lines.
 *
 * Notes:
 *  - The waveform is visualized using properties of audio samples from 'WavFile'
 *
 * References:
 *  - ...
 */

class WavForm : public QGraphicsView
{
    Q_OBJECT
    QGraphicsScene scene;
    bool scrubberHasBeenDrawn;
    bool centerOnScrubber;
    bool audioFileLoaded;
    QGraphicsLineItem *lastLine;
    WavFile *audio;
    int viewW;
    int viewH;
    int chartW;
    int chartH;
    bool boolAutoSegment;
    bool segmentControls;
    QPointF startSegmentP;
    QPointF endSegmentP;
    QGraphicsLineItem *startSegment;
    QGraphicsLineItem *endSegment;
    QList<QGraphicsLineItem*> intervalLines;
    double delta;
    void drawIntervalLinesInSegment(double x);
    QList<float> intLinesX;
    void updateIntervals(int oldChartWidth);
    QPointF viewCenterPoint;

public:
    explicit WavForm(int _width, int _height);
    void audioToChart();
    void setChart(QList<float> data, int width, int height);
    QList<float> getSamples();
    void updateDelta(double delta);
public slots:
    void uploadAudio(QString fName);
    void updateScrubberPosition(double position);
    void updateChart(int width, int height);
    void switchMouseEventControls(bool segmentControlsOn);
    void sendIntervalsForSegment();
    void drawAutoIntervals(QList<int> intervalLocsInAudio);
    void clearIntervals();
    void changeBoolAutoSegment(bool _boolAutoSegment);
    void changeCenterOnScrubber(Qt::CheckState checkedState);



protected:
    void mousePressEvent(QMouseEvent *evt) override;

signals:
    void sendAudioPosition(double position);
    void sceneSizeChange();
    void audioFileLoadedTrue();
    void segmentReady(bool ready);
    void segmentLength(int numSamples, int sampleRate);
    void intervalsForSegments(QList<int>, bool);
    void chartInfoReady(bool ready);
    void clearAllSegmentInfo();
};

#endif // WAVFORM_H
