#ifndef WAVFORM_H
#define WAVFORM_H

#include <QWidget>
#include "wavfile.h"
#include <QtCharts>

/*
 * File: wavform.h
 * Description:
 *  This header file defines the 'WavForm' class, which is a custom 'QGraphicsView' that creates a
 *  waveform visualization using 'QGraphicsView'. This class renders an audio waveform using the data
 *  from a 'WavFile' object and allows user interaction.
 *
 * Purpose:
 *  - Creates a visual representation (waveform) of WAV audio data
 *  - Supports scrubbing for audio position tracking
 *
 * Key Members:
 *  - 'QGraphicsScene scene': The graphics scene used to render the waveform and scrubber.
 *  - `bool scrubberHasBeenDrawn`: Tracks whether the scrubber line has been added to the scene.
 *  - `QGraphicsLineItem *lastLine`: Pointer to the last scrubber line drawn.
 *  - `WavFile *audio`: Pointer to the associated `WavFile` object containing audio data.
 *  - `int viewW, viewH`: Dimensions of the `QGraphicsView` widget.
 *  - `int chartW, chartH`: Dimensions of the rendered waveform chart.
 *
 * Public Methods:
 *  - `explicit WavForm(int _width, int _height)`: Constructor initializing the view dimensions.
 *  - `void audioToChart()`: Loads audio data from the `WavFile` and creates a waveform visualization.
 *  - `void setChart(QList<float> data, int width, int height)`: Draws the waveform using audio sample data.
 *
 * Slots:
 *  - `void uploadAudio(QString fName)`: Loads a WAV file and generates its waveform visualization.
 *  - `void updateScrubberPosition(double position)`: Updates the scrubber position based on a relative position.
 *  - `void updateChart(int width, int height)`: Redraws the chart to fit new dimensions.
 *
 * Signals:
 *  - `void sendAudioPosition(double position)`: Emitted when the scrubber position changes.
 *  - `void sceneSizeChange()`: emmited when scene size is finished change
 *
 * Protected Methods:
 *  - `void mousePressEvent(QMouseEvent *evt) override`: Handles user interaction for updating the scrubber.
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
    bool segmentControls;
    QPointF startSegmentP;
    QPointF endSegmentP;
    QGraphicsLineItem *startSegment;
    QGraphicsLineItem *endSegment;


public:
    explicit WavForm(int _width, int _height);
    void audioToChart();
    void setChart(QList<float> data, int width, int height);
    QList<float> getSamples();
public slots:
    void uploadAudio(QString fName);
    void updateScrubberPosition(double position);
    void updateChart(int width, int height);
    void switchMouseEventControls(bool segmentControlsOn);

protected:
    void mousePressEvent(QMouseEvent *evt) override;

signals:
    void sendAudioPosition(double position);
    void sceneSizeChange();
    void audioFileLoadedTrue();
};

#endif // WAVFORM_H
