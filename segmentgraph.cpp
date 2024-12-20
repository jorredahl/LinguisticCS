#include "segmentgraph.h"
#include "QBoxLayout"
#include <QtCharts/QLineSeries>
#include <QValueAxis>

/*
 * File: segmentgraph.cpp
 * Description:
 *  This source file implements the 'SegmentGraph', visualizing the samples from the audio data. This class manages
 *  the widget layout, chart updates, and user interactions such as the segment slider and exit button. The constructor
 *  'SegmentGraph(int width, int height)' creates a layout for the widget that is filled with the exit button and slider
 *  above a QChartView. The widget is vertically and horizontally stretchable to fit the window.
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
 *  - Default values for the width and height are 400 and 200 respectively.
 *  - This widget is only set to be visible when the updateGraphs slot is called.
 *
 * References:
 *  - ...
 */

SegmentGraph::SegmentGraph(int width, int height) {
    charts = new QList<QChart *>();

    segGraphLayout = new QVBoxLayout();
    setLayout(segGraphLayout);

    QHBoxLayout *segGraphControlsLayout = new QHBoxLayout();
    segGraphLayout->addLayout(segGraphControlsLayout);

    exitButton = new QPushButton("Close");
    exitButton->setFixedSize(30, 20);
    connect(exitButton, &QPushButton::clicked, this, &SegmentGraph::exitView);
    segGraphControlsLayout->addWidget(exitButton);

    segmentSlider = new QSlider();
    segmentSlider->setOrientation(Qt::Horizontal);
    connect(segmentSlider, &QSlider::sliderMoved, this, &SegmentGraph::slideSegments);
    segGraphControlsLayout->addWidget(segmentSlider);

    QAction *playAction = new QAction();
    connect(playAction, &QAction::triggered, this, &SegmentGraph::playSegmentAudio);
    playAction->setShortcut(Qt::Key_Space);
    playSegmentButton = new QToolButton;
    playSegmentButton->setDefaultAction(playAction);
    playSegmentButton->setIcon(QIcon(":/resources/icons/play.svg"));
    playSegmentButton->setEnabled(false);
    segGraphControlsLayout->addWidget(playSegmentButton);
    audioPlaying = false;

    graph = new QChartView();
    graph->resize(width, height);
    segGraphLayout->addWidget(graph);
}

void SegmentGraph::slideSegments(int position) {
    if (position >= charts->length()) return;
    graph->setChart(charts->at(position));
    getSegmentAudioToPlay(position);
}

void SegmentGraph::exitView() {
    setVisible(false);
    audioPlaying = false;
}

void SegmentGraph::updateGraphs(QList<QList<float>> segments) {
    audioPlaying = false;
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
    segmentSlider->setMaximum(charts->length() - 1);
    slideSegments(0);
    segmentSlider->setSliderPosition(0);
    emit clearSegmentsEnable(true);
}
void SegmentGraph::clearView(){
    if (segmentSlider) segmentSlider->setSliderPosition(0);
    int w = graph->width();
    int h = graph->height();
    delete graph;
    graph = new QChartView();
    graph->resize(w, h);
    segGraphLayout->addWidget(graph);
     if (!charts->isEmpty()){
        charts->clear();
     }
     playSegmentButton->setEnabled(false);
    exitView();
     emit clearSegmentsEnable(false);


}
void SegmentGraph::getSegmentAudioToPlay(int segmentPosition){
    if (startEndSegmentAudioValues.isEmpty()) return;
    startEndOfSelectedSegment = startEndSegmentAudioValues[segmentPosition];
    playSegmentButton->setEnabled(true);

}
void SegmentGraph::playSegmentAudio(){
    QIcon icon = audioPlaying ? QIcon(":/resources/icons/play.svg") : QIcon(":/resources/icons/pause.svg");
    playSegmentButton->setIcon(icon);
    audioPlaying = !audioPlaying;
    emit sendPlaySegmentAudio(startEndOfSelectedSegment);
}
void SegmentGraph::getSegmentStartEnd(QList<QPair<double, double>> startEndValues){
    startEndSegmentAudioValues = startEndValues;
}
void SegmentGraph::changePlayPauseButton(bool segAudioNotPlaying){
    QIcon icon = segAudioNotPlaying ? QIcon(":/resources/icons/play.svg") : QIcon(":/resources/icons/pause.svg");
    playSegmentButton->setIcon(icon);
    audioPlaying = !segAudioNotPlaying;
}

