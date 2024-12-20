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


/* File: spectrograph.h
 * Description:
 *  This header file defines the spectrograph class , which provides visualization for an audio file using a QImage and QPixmap.
 *
 * Purpose:
 *  - Creates a visual representation (spectrogram) from audio data
 *
 * Key Features:
 *  - Displays a spectrogram of audio data
 *  - Provides functionality for enabling / disabling peak amplitude visualization
 *  - Includes audio decoding and FFT transformation using FFTW library
 *
 * Key Methods:
 *  - 'void setupSpectograph(QVector<double> &accumulatedSamples)': Prepares the spectogram using FFT for an audio segment
 *  - 'void renderToPixmap': Creates spectogram visualization using QPixmap
 *  - 'void hammingWindow(int windowLength, QVector<double> &window)': Applies hamming window to smooth audio data
 *
 * Slots:
 *  - 'void bufferReady()': Processes ready audio buffers by decoding into sample data
 *  - 'void loadAudioFile(const QString &fileName)': initilizes processing
 *  - 'void processAudioFile(const QUrl &fileUrl)' : takes in fileUrl to sample values and prepares them for FFT by calling bufferReady and finish signals on QAudioDecoder
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
    QPixmap cachedSpect;

private:
    QGraphicsView *graphicsView; // display the scene
    QGraphicsScene *graphicsScene;

    QString currentAudioFile; // storing file

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
    double maxAmp = 0.0;
    int hopSize;
    int windowSize = 1024;

    // helper method
    void hammingWindow(int windowLength, QVector<double> &window);

public slots:
    void bufferReady();
    void processAudioFile(const QUrl &fileUrl);
    void loadAudioFile(const QString &fileName);
    void renderToPixmap();

private slots:
    void decodingFinished();
};


#endif // SPECTROGRAPH_H
