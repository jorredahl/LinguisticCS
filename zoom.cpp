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
{

    zoomLayout = new QHBoxLayout();
    setLayout(zoomLayout);

    //sliders for horizontal and vertical zoom
    horizantalSlider = new QSlider();
    horizantalSlider->setOrientation(Qt::Horizontal);
    horizantalSlider->setMinimum(1);
    horizantalSlider->setMaximum(200);
    connect(horizantalSlider, &QSlider::sliderMoved, this, &Zoom::horizantalZoom);
    connect(horizantalSlider, &QSlider::sliderReleased, this, &Zoom::sliderReleased);
    zoomLayout->addWidget(horizantalSlider);

    verticalSlider = new QSlider();
    verticalSlider->setOrientation(Qt::Horizontal);
    verticalSlider->setMinimum(1);
    verticalSlider->setMaximum(10);
    connect(verticalSlider, &QSlider::sliderMoved, this, &Zoom::verticalZoom);
    zoomLayout->addWidget(verticalSlider);

}

void Zoom::verticalZoom(int position) {

    zoomedHeight = graphHeight * position;
    emit zoomGraphIn(zoomedWidth, zoomedHeight);

}

void Zoom::horizantalZoom(int position) {

    zoomedWidth = graphWidth * position;
    emit zoomGraphIn(zoomedWidth, zoomedHeight);

}

void Zoom::sliderReleased(){
    emit zoomGraphIn(horizantalSlider->sliderPosition() * graphWidth, verticalSlider->sliderPosition() * graphHeight);
}
