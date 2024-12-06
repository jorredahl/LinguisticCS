#ifndef ZOOM_H
#define ZOOM_H
#include "QBoxLayout"
#include "QPushButton"
#include "QCheckBox"
#include <QSlider>
#include <QWidget>

/*
 * File: zoom.h
 * Description:
 *  This header file defines the 'Zoom' class, which provides functionality for controlling the zoom level
 *  of our waveform view using horizontal and vertical sliders
 *
 * Purpose:
 *  - Allows user to zoom in and out of waveform view along horizontal and vertical axes
 *  - Emits updated dimensions of the viewer and scrubber position whenever the zoom level has changed
 *
 * Key Members:
 *  - 'QHBoxLayout *zoomLayout': Horizontal layout containing the QSliders for zoom controls (widht and height)
 *  - 'QSlider *verticalSlider': QSlider for controlling vertical zoom
 *  - 'QSlider *horizontalSlider': QSlider for controlling horizontal zoom
 *  - 'int graphWidth': Initial graph width
 *  - 'int graphHeight': Initical graph height
 *  - 'int zoomWidth': Initial zoom width level
 *  - 'int zoomHeight': Initical zoom height level
 *
 * Public Methods:
 *  - 'Zoom(QWidget *parent = nullptr, int viewW = 400, int viewH = 200)': Constructor intializing the zoom
 *    layout and sliders
 *
 * Public slots:
 *  - 'void verticalZoom(int position)': Updates the vertical zoom level and emits new dimensions
 *  - 'void horizontalZoom(int position)': Updates the horizontal zoom level and emits new dimensions
 *
 * Signals:
 *  - 'void zoomGraphIn(int width, int height)': Signal emitted whenever vertical or horizontal zoom level changes
 *
 * References:
 *  - ...
 */

class Zoom : public QWidget
{
    Q_OBJECT
    //zoom
    QHBoxLayout *zoomLayout;
    QSlider *verticalSlider;
    QSlider *horizontalSlider;

public:
    explicit Zoom(QWidget *parent = nullptr, int viewW = 400, int viewH = 200);
    int graphWidth;
    int graphHeight;
    int zoomedWidth;
    int zoomedHeight;
    void resetZoom();

public slots:

    void verticalZoom(int position);
    void horizontalZoom(int position);
signals:

    void zoomGraphIn(int width, int height);

};

#endif // ZOOM_H
