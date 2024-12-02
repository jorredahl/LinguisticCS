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

/* File spectrograph.h
 * Defines the spectrograph class , which provides visualization for an audio file using QPaintEvent
 *
 * Purpose:
 * Creates a visual representation (spectrogram) from audio data
 *
 * Key Features:
 * - Displays a spectrogram of audio data
 * - Provides functionality for enabling / disabling peak amplitude visualization
 * - Includes audio decoding and FFT transformation using FFTW library
 *
 * */

class Spectrograph : public QWidget
{
    Q_OBJECT

public:
    explicit Spectrograph(QWidget *parent = nullptr);
    ~Spectrograph();

    // configures the spectrogram visualization
    void setupSpectrograph(QVector<double> &accumulatedSamples);
    int getWindowSize() const { return windowSize; }
    void reset();
    bool showPeaksEnabled = false;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    // audio processing
    QAudioDecoder *decoder = nullptr;
    QVector<double> accumulatedSamples;

    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    // FFT and spectrogram
    fftw_complex *in;
    fftw_complex *out;
    fftw_plan plan;
    fftw_complex *data, *fft_result;
    QVector<QVector<double>> spectrogram; // 2D matrix for spectrogram
    QVector<double> hammingWindowValues;

    // parameters
    double maxAmp = 1.0;
    int hopSize;
    int windowSize = 1024;

    // helper method
    void hammingWindow(int windowLength, QVector<double> &window);

   // void newSpectrograph();
   // QVector<double> magnitudes;


public slots:
    void bufferReady();
    void processAudioFile(const QUrl &fileUrl);
    void handleAudioBuffer(const QAudioBuffer &buffer);
    void loadAudioFile(const QString &fileName);
    void showPeaks();

};


#endif // SPECTROGRAPH_H
