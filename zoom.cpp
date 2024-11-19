#include "zoom.h"

Zoom::Zoom(QWidget *parent)
    : QWidget(parent), graphWidth(800), graphHeight(300)
{
    zoomIn = new QPushButton("+");
    zoomOut = new QPushButton("-");

    verticalZoom = new QCheckBox("vertical zoom");
    zoomLayout = new QVBoxLayout();
    setLayout(zoomLayout);

    zoomLayout-> addWidget(zoomIn, 0, Qt::AlignBottom);
    zoomLayout-> addWidget(zoomOut, 0, Qt::AlignTop);
    zoomLayout-> addWidget(verticalZoom, 0, Qt::AlignTop);

    connect(zoomIn, &QPushButton::clicked, this, &Zoom::handleZoomIn);
    connect(zoomOut, &QPushButton::clicked, this, &Zoom::handleZoomOut);
}

void Zoom::handleZoomIn(){
    if (verticalZoom->isChecked()){
        graphHeight += 100;
    }
    else{
        graphWidth +=100;
    }
    emit zoomGraphIn(graphWidth, graphHeight);
    qDebug() << "zoom emitted";

}

void Zoom::handleZoomOut(){
    if (verticalZoom->isChecked()){
        graphHeight -= 100;
    }
    else{
        graphWidth -=100;
    }
    emit zoomGraphOut(graphWidth, graphHeight);
    qDebug() << "zoom emitted";
}
