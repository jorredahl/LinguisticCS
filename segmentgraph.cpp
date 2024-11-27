#include "segmentgraph.h"
#include "QBoxLayout"
#include <QtCharts/QLineSeries>
#include <QValueAxis>

/*
 * File: segmentgraph.cpp
 * Description:
 *  This source file implements the 'SegmentGraph', visualizing the samples from the audio data. This widget is only
 *  set to be visible when the updateGraphs slot is called. The constructor 'SegmentGraph(int width, int height)'
 *  creates a layout for the widget that is filled with the exit button and slider above a QChartView. The widget is
 *  vertically and horizontally stretchable to fit the window.
 *
 * Slots:
 *  - 'void SegmentGraph::slideSegments(int position)': Given a integer of the slider position, the QChartView will
 *    change its QChart to the corresponding value by index
 *  - 'void SegmentGraph::exitView()': Upon pressing the exit button, the widget will be set to be invisible
 *  - 'void SegmentGraph::updateGraphs(QList<QList<float>> segments)': The widget is set as visible, the chart values
 *    are cleared, and then refilled with new charts based on the values of segments. Using this new charts value, the
 *    slider is reset to fid it's values.
 *
 * Notes:
 *  - Default values for the width and height are 400 and 200 respectively
 *
 * References:
 *  - ...
 */

SegmentGraph::SegmentGraph(int width, int height) {
    charts = new QList<QChart *>();

    QVBoxLayout *segGraphLayout = new QVBoxLayout();
    setLayout(segGraphLayout);

    QHBoxLayout *segGraphControlsLayout = new QHBoxLayout();
    segGraphLayout->addLayout(segGraphControlsLayout);

    exitButton = new QPushButton("X");
    exitButton->setFixedSize(30, 20);
    connect(exitButton, &QPushButton::clicked, this, &SegmentGraph::exitView);
    segGraphControlsLayout->addWidget(exitButton);

    segmentSlider = new QSlider();
    segmentSlider->setOrientation(Qt::Horizontal);
    connect(segmentSlider, &QSlider::sliderMoved, this, &SegmentGraph::slideSegments);
    segGraphControlsLayout->addWidget(segmentSlider);

    graph = new QChartView();
    graph->resize(width, height);
    segGraphLayout->addWidget(graph);
}

void SegmentGraph::slideSegments(int position) {
    if (position >= charts->length()) return;
    graph->setChart(charts->at(position));
}

void SegmentGraph::exitView() {
    setVisible(false);
}

void SegmentGraph::updateGraphs(QList<QList<float>> segments) {
    setVisible(true);

    charts->clear();

    for(int i = 0; i < segments.length(); ++i) {
        QLineSeries *segmentLine = new QLineSeries();
        for(int j = 0; j < segments[i].length(); ++j) {
            segmentLine->append(j + 1, segments[i][j]);
        }
        QChart *tempChart = new QChart();
        tempChart->addSeries(segmentLine);
        tempChart->legend()->hide();
        tempChart->createDefaultAxes();
        charts->append(tempChart);
    }

    segmentSlider->setMinimum(0);
    segmentSlider->setMaximum(charts->length());
    slideSegments(0);
}
