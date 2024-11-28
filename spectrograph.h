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

    explicit Spectrograph(QWidget *parent = nullptr, int buttonIndex = 1);
    ~Spectrograph();

    // takes samples as input
    //void setupSpectrograph(const QVector<double> &samples);
    void setupSpectrograph(QVector<double> &accumulatedSamples);
    int getWindowSize() const { return windowSize; }
    void reset1();
    void reset2();


private:

    QPushButton *showSpect1;
    QPushButton *showSpect2;

    void newSpectrograph();
    double maxAmp = 1.0;
    int hopSize;
    QVector<double> magnitudes;

   // fftw_plan plan;
    fftw_complex *in;
    fftw_complex *out;

    void hammingWindow(int windowLength, QVector<double> &window);
    fftw_complex *data, *fft_result;
    fftw_plan plan;

    int windowSize = 1024;

    QVector<QVector<double>> spectrogram; // 2D matrix
    QVector<double> hammingWindowValues;
   // QVector<double> accumulatedSamples;
    QVector<double> accumulatedSamples1;
    QVector<double> accumulatedSamples2;


    QMediaPlayer *player;
    QAudioOutput *audioOutput;
   //Spectrograph *spectrograph;
    //QAudioDecoder *decoder;


    QAudioDecoder *decoder1;
    QAudioDecoder *decoder2;
  // QVector<double> accumulatedSamples;

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:
    void uploadAudio1();
    void uploadAudio2();
    // void userAudio();
   // void processAudioBuffer(const QAudioBuffer &buffer);
   // void bufferReady();
    void bufferReady1();
    void bufferReady2();

    void processAudioFile(const QUrl &fileUrl, int audioIndex);
    void handleAudioBuffer(const QAudioBuffer &buffer, int audioIndex);

};





#endif // SPECTROGRAPH_H
