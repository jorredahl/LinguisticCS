#ifndef ZOOM_H
#define ZOOM_H
#include "QBoxLayout"
#include "QPushButton"
#include "QCheckBox"
#include <QSlider>
#include <QWidget>

class Zoom : public QWidget
{
    Q_OBJECT
    //zoom
    QHBoxLayout *zoomLayout;
    QSlider *verticalSlider;
    QSlider *horizantalSlider;

public:
    explicit Zoom(QWidget *parent = nullptr, int viewW = 400, int viewH = 200);
    int graphWidth;
    int graphHeight;
    int zoomedWidth;
    int zoomedHeight;

public slots:

    void verticalZoom(int position);
    void horizantalZoom(int position);
signals:

    void zoomGraphIn(int width, int height);

};

#endif // ZOOM_H
