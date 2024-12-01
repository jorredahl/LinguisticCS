#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H

#include <QtMultimedia/qaudiobuffer.h>
#include <QtMultimedia/qaudiooutput.h>
#include <QtMultimedia/qmediaplayer.h>
#include <QtWidgets>
#include <QAudioSource>
#include <QIODevice>
#include <QImage>
#include <fftw3.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAudioDecoder>


/* This widget will display our spectrograph */
class Spectrograph : public QWidget
{
    Q_OBJECT

public:
    explicit Spectrograph(QWidget *parent = nullptr);
    ~Spectrograph();

    // takes samples as input
    void setupSpectrograph(QVector<double> &accumulatedSamples);
    int getWindowSize() const { return windowSize; }
    void reset();

    bool showPeaksEnabled = false;

protected:
    void paintEvent(QPaintEvent *event) override;
    void showPeaks();

private:
    QAudioDecoder *decoder = nullptr; // decoder for Spectrograph instance
    QVector<double> accumulatedSamples; // accumulatedSamples for Spectrograph instance

    void newSpectrograph();
    double maxAmp = 1.0;
    int hopSize;
    QVector<double> magnitudes;

    fftw_complex *in;
    fftw_complex *out;

    void hammingWindow(int windowLength, QVector<double> &window);
    fftw_complex *data, *fft_result;
    fftw_plan plan;

    int windowSize = 1024;

    QVector<QVector<double>> spectrogram; // 2D matrix
    QVector<double> hammingWindowValues;


    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    QAudioDecoder *decoder1;
    QAudioDecoder *decoder2;


public slots:
    void bufferReady();

    void processAudioFile(const QUrl &fileUrl);
    void handleAudioBuffer(const QAudioBuffer &buffer);

    // new for cleaning up
    void loadAudioFile(const QString &fileName);

};


#endif // SPECTROGRAPH_H
