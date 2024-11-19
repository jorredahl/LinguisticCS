#ifndef ZOOM_H
#define ZOOM_H
#include "QBoxLayout"
#include "QPushButton"
#include "QCheckBox"

#include <QWidget>

class Zoom : public QWidget
{
    Q_OBJECT
    //zoom
    QVBoxLayout *zoomLayout;
    QPushButton *zoomIn;
    QPushButton *zoomOut;
    QCheckBox *verticalZoom;
public:
    explicit Zoom(QWidget *parent = nullptr);
    int graphWidth;
    int graphHeight;

public slots:
    void handleZoomIn();
    void handleZoomOut();
signals:
    void zoomGraphIn(int width, int height);
    void zoomGraphOut(int width, int height);

};

#endif // ZOOM_H
