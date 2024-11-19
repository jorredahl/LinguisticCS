#include "zoom.h"

Zoom::Zoom(QWidget *parent, int viewW, int viewH)
    : QWidget(parent), graphWidth(viewW), graphHeight(viewH), zoomedWidth(viewW), zoomedHeight(viewH)
{

    zoomLayout = new QHBoxLayout();
    setLayout(zoomLayout);

    horizantalSlider = new QSlider();
    horizantalSlider->setOrientation(Qt::Horizontal);
    horizantalSlider->setMinimum(1);
    horizantalSlider->setMaximum(10);
    connect(horizantalSlider, &QSlider::sliderMoved, this, &Zoom::horizantalZoom);
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
