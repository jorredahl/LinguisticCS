#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H

#include <QtWidgets>
#include <QAudioSource>
#include <QIODevice>
#include <QImage>
#include <fftw3.h>

/* This widget will display our spectrograph */

class Spectrograph : public QWidget
{
    Q_OBJECT
public:

    explicit Spectrograph(QWidget *parent = nullptr);
    ~Spectrograph();

    // takes samples as input
    void setupSpectrograph(const QVector<double> &samples);
    QVector<double> magnitudes;
    double maxMagnitude = 1.0;
    int hopSize;
    int getWindowSize() const { return windowSize; }

private:
   // fftw_plan plan;
    fftw_complex *in;
    fftw_complex *out;

    void hammingWindow(int windowLength, QVector<double> &window);
    fftw_complex *data, *fft_result;
    fftw_plan plan;

    int windowSize = 1024;

    //int windowSize = 2048;
    QVector<QVector<double>> spectrogram; // 2D matrix
    QVector<double> hammingWindowValues; // hamming window value
    QVector<double> accumulatedSamples;

protected:
    void paintEvent(QPaintEvent *event) override;

};

#endif // SPECTROGRAPH_H
