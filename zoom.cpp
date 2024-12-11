#include "zoom.h"

/*
 * File: zoom.cpp
 * Description:
 *  This source file implements the 'Zoom' class, providing zoom control functionality using QSliders
 *  and calculating the new dimensions and emitting them to adjust the graphic (waveform) view. The
 *  class manages two QSliders, the horizontal zoom and vertical zoom, each slider applying a scaling
 *  factor to their respective dimensions to change the zoom level. Signals are emitted whenever the
 *  slider is moved to adjust the view. The constructor 'Zoom(QWidget *parent, int viewW, int viewH)'
 *  initializes the layout and sliders, sets the default zoom dimensions, and connects slider movements
 *  to their respective slot functions.
 *
 * Slots:
 *  - 'void verticalZoom(int position)': Calculates the zoom height level based on the posiiton of the
 *    slider and emits the 'zoomGraphIn' signal with the updated dimensions
 *  - 'void horizontalZoom(int position)': Calculates the zoom width level based on the position of the
 *    slider and emits the 'zoomGraphIn' signal with the updated dimensions
 *   - 'void sliderReleased()': signals when the slider is done being moved by the user so that the slider
 *      can be redrawn. It is only necessary to redraw when the width is changed.
 *
 *
 * Notes:
 *  - Zoom levels range from 1x to 10x
 *  - Default values for 'viewW' and 'viewH' are 400 and 200 respectively
 *
 * References:
 *  - ...
 */

Zoom::Zoom(QWidget *parent, int viewW, int viewH)
    : QWidget(parent), graphWidth(viewW), graphHeight(viewH), zoomedWidth(viewW), zoomedHeight(viewH)
{}
void Zoom::setHorizontalSlider(QSlider* slider){
    horizontalSlider = slider;
    connect(horizontalSlider, &QSlider::sliderMoved, this, &Zoom::horizontalZoom);
}
void Zoom::setVerticalSlider(QSlider* slider){
    verticalSlider = slider;
    connect(verticalSlider, &QSlider::sliderMoved, this, &Zoom::verticalZoom);
}
void Zoom::resetZoom(){
    horizontalSlider->setSliderPosition(1);
    verticalSlider->setSliderPosition(1);
    zoomedHeight = graphHeight * 1;
    zoomedWidth = graphWidth * 1;
    emit zoomGraphIn(zoomedWidth, zoomedHeight);
    emit zoomGraphIn(zoomedWidth, zoomedHeight);
    emit resetZoomActivated();

}

void Zoom::verticalZoom(int position) {

    zoomedHeight = graphHeight * position;
    emit zoomGraphIn(zoomedWidth, zoomedHeight);
    emit verticalSliderChanged(position);
}

void Zoom::horizontalZoom(int position) {

    zoomedWidth = graphWidth * position;
    emit zoomGraphIn(zoomedWidth, zoomedHeight);
    emit horizontalSliderChanged(position);
}

