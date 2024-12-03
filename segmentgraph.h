#ifndef SEGMENTGRAPH_H
#define SEGMENTGRAPH_H

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QChart>
#include <QChartView>
#include <QBoxLayout>

/*
 * File: segmentgraph.h
 * Description:
 *  This header file defines the 'SementGraph' class, this class provides closer visualizations of raw WAV file data
 *  in the form of a QChartView.
 *
 * Purpose:
 *  - The segment graph will appear upon the selection of different segments, this widget will show the raw data of
 *    a given segment as a QChart. An exit button and slider are included for controls of this widget.
 *
 * Key Members:
 *  - 'QSlider *segmentSlider': A slider to select which chart out of the list to show
 *  - 'QPushButton *exitButton': A button that controls the visibility of the widget to make it invisible
 *  - 'QChartView *graph': The main widget of which to view the graphs
 *  - 'QList<QChart *> *charts': Data of preloaded charts to easily switch between on the chartView
 *
 * Public Methods:
 *  - 'SegmentGraph(int width, int height)': Constructor initializing the exit button, slider, and charts. Parameters
 *    for width and height create the size of the main chart
 *
 * Slots:
 *  - 'void slideSegments(int position)': changes the chart to show the graph according to the slider position
 *  - 'void exitView()': makes the SegmentGraph invisible until the next updateGraph
 *  - 'void updateGraphs(QList<QList<float>> segments)': visualizes the segments as a list of QCharts
 *
 * Notes:
 *  - This widget is completely invisible until a signal with a QList<QList<float>> is emitted.
 *
 * References:
 *  - ...
 */

class SegmentGraph: public QWidget
{
    Q_OBJECT
    QVBoxLayout *segGraphLayout;
    QSlider *segmentSlider;
    QPushButton *exitButton;
    QChartView *graph;
    QList<QChart *> *charts;

public:
    SegmentGraph(int width, int height);

public slots:
    void slideSegments(int position);
    void exitView();
    void updateGraphs(QList<QList<float>> segments);
    void clearView();
};

#endif // SEGMENTGRAPH_H
