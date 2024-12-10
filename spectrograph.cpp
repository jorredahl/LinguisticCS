#include <QtWidgets>
#include <fftw3.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAudioDecoder>
#include "spectrograph.h"

/*
 * File: spectograph.cpp
 * Description:
 *  This source file implements the Spectograph class, which visualizes audio as a spectogram using FFT,
 *  transforming audio data into its frequency-domain representation to display in the spectogram and
 *  providing a detailed view of the frequency content of the audio.
 *
 * Key Methods:
 *  - 'Spectograph(QWidget *parent)': Constructor initializes FFT setup, UI layout, and defines default parameters.
 *  - 'void loadAudioFile(const QString &fileName)': Loads audio file and initializes decoder (processAudioFile).
 *  - 'void processAudioFile(const QUrl &fileUrl)': Sets up 'QAudioDecoder' for decoding audio buffers.
 *  - 'void setupSpectograph(QVector<double> &accumulatedSamples)': Applies FFT to audia data chunks and updates spectogram.
 *  - 'void hammingWindow(int windowLength, QVector<double> &window)': Generates hamming window vector for FFT
 *  - 'void reset()': Clears spectogram and samples data and resets the spectogram.
 *  - 'void paintEvent(QPaintEvent *event)': Creates spectogram visualization with changing color intensity based
 *    on amplitude and draws peak frequency lines if enabled.
 *
 * References:
 *  - This blog explains how to perform Short-Time Fourier Transform using FFTW.
 *    It inspired the implementation of overlapping window processing and FFT setup.
 *    https://ofdsp.blogspot.com/2011/08/short-time-fourier-transform-with-fftw3.html
 *  - https://cplusplus.com/forum/beginner/251061/
 */

Spectrograph::Spectrograph(QWidget *parent)
    : QWidget(parent), graphicsView(new QGraphicsView(this)), graphicsScene(new QGraphicsScene(this))
{

    /* hopSize can be adjusted
     *
     * EXAMPLE: if we do windowSize/5 then 5 chunks will be processed , this can be tested with the qDebug statements
     *
     * Currently, hopSize = windowSize means only one chunk is processed at a time
     * This provides the cleanest visualization, smaller hopSizes can introduce some lag
     * */
    hopSize = windowSize;

    // Allocate memory for FFT input and output arrays
    data = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * windowSize);
    fft_result = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * windowSize);

    // Create an FFTW plan for 1D transformation
    plan = fftw_plan_dft_1d(windowSize, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);

    // calc hamming window for smoothing (FFT processing)
    hammingWindow(windowSize, hammingWindowValues);

    graphicsView->setFixedSize(650, 200);
    graphicsScene->setSceneRect(0, 0, 650, 200); // match the scene size to the view

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    graphicsView->setScene(graphicsScene);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainLayout->addWidget(graphicsView);
    setLayout(mainLayout);

    // set fixed size for spectrograph (can modify)
    //setFixedSize(700, 150);
}

void Spectrograph::loadAudioFile(const QString &fileName) {
    reset(); // Clear curr spect data
    currentAudioFile = fileName; // Set the new audio file
    processAudioFile(QUrl::fromLocalFile(fileName)); // start processing
}


void Spectrograph::processAudioFile(const QUrl &fileUrl) {

    if (!decoder) {
        // init QAudioDecoder for decoding audio buffers
        decoder = new QAudioDecoder(this);
        connect(decoder, &QAudioDecoder::bufferReady, this, &Spectrograph::bufferReady);
        connect(decoder, &QAudioDecoder::finished, this, &Spectrograph::decodingFinished);
    }

    accumulatedSamples.clear(); // clear any prev samples
    decoder->setSource(fileUrl); // set decoder src to the new file
    decoder->start();
}


// used buffer ready should be finished
// when using buffer ready it should only be when QAudioDecoder is finished
void Spectrograph::bufferReady() {

    QAudioBuffer buffer = decoder->read();
    const qint16 *data = buffer.constData<qint16>();
    int sampleCount = buffer.sampleCount();

    for (int i = 0; i < sampleCount; ++i) {
        accumulatedSamples.append(static_cast<double>(data[i]) / 32768.0); // Normalize to 16-bit signed integer
    }
}


// if the decoder is finished then we
void Spectrograph::decodingFinished() {

    // ensure there are samples to process
    if (accumulatedSamples.isEmpty()) {
        qWarning() << "No audio samples to process!";
        return;
    }

    // process the accumulated samples into spectrogram chunks
    QVector<double> windowSamples = accumulatedSamples; // copy all samples
    setupSpectrograph(windowSamples);
    accumulatedSamples.clear();
}


void Spectrograph::handleAudioBuffer(const QAudioBuffer &buffer) {

    // convert audio buffer to 16-bit signed integers
    const qint16 *data = buffer.constData<qint16>();
    int sampleCount = buffer.sampleCount();


    // append samples to accumulatedSamples for further processing
    for  (int i = 0; i < sampleCount; ++i) {
        accumulatedSamples.append(static_cast<double>(data[i]));
    }

    // Process accumulatedSamples in overlapping windows of size windowSize
    while (accumulatedSamples.size() >= getWindowSize()) {

        // extract a windowSize for FFT processing
        QVector<double> windowSamples = accumulatedSamples.mid(0, getWindowSize());
        // remove processed samples based on hopSize
        accumulatedSamples = accumulatedSamples.mid(hopSize);
        setupSpectrograph(windowSamples);
    }
}


// free mem and destroy plan
Spectrograph::~Spectrograph() {
    fftw_destroy_plan(plan);
    fftw_free(data);
    fftw_free(fft_result);
    if (decoder) delete decoder;
}


void Spectrograph::hammingWindow(int windowLength, QVector<double> &window) {

    window.resize(windowLength);  // make sure the vector is the correct size
    for (int i = 0; i < windowLength; i++) {
        window[i] = 0.54 - 0.46 * cos(2 * M_PI * i / (windowLength - 1));
    }
}


void Spectrograph::setupSpectrograph(QVector<double> &accumulatedSamples) {
    graphicsScene->clear();

    // signal length and number of chunks based on hopSize and window size
    int signalLength = accumulatedSamples.size();
    int numChunks = (signalLength - windowSize) / hopSize + 1;

    if (numChunks > 0) {

        // resize spectrogram to accommodate new chunks
        spectrogram.resize(spectrogram.size() + numChunks, QVector<double>(windowSize/ 2));

        // process each chunk
        for (int chunk = 0; chunk < numChunks; ++chunk) {
            int chunkPosition = chunk * hopSize;

            // apply the hamming window and prepare data for FFT
            for (int i = 0; i < windowSize; ++i) {
                int readIndex = chunkPosition + i;

                // ensure we dont go out of bounds for accum samples
                data[i][0] = (readIndex < accumulatedSamples.size()) ? accumulatedSamples[readIndex] * hammingWindowValues[i] : 0.0;
                data[i][1] = 0.0;  // imaginary part is zero
            }

            // execute FFT
            fftw_execute(plan);

            // store amplitudes in spectrograph
            for (int i=0; i < windowSize/2; ++i) {
                //what is displayed from fft
                double real = fft_result[i][0];
                double imag = fft_result[i][1];

                // used this https://cplusplus.com/forum/beginner/251061/ and adjusted a tiny bit
                double amplitude = 2 * std::sqrt(real * real + imag * imag);
                // store amp to represent intensity for visualizing the spectrogram
                spectrogram[spectrogram.size() - numChunks + chunk][i] = amplitude;
            }
        }
    }
    renderToPixmap();
}


void Spectrograph::renderToPixmap() {
    if (spectrogram.isEmpty())
        return;

    QImage image(width(), height(), QImage::Format_RGB32);
    image.fill(Qt::black); // base color

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // determine dimensions for each chunk and frequency
    int numChunks = spectrogram.size();
    int numFrequencies = windowSize / 2;
    double chunkWidth = static_cast<double>(width()) / (numChunks);
    double freqHeight = static_cast<double>(height()) / (numFrequencies/105); // cut in half again bc fftw is mirrored

    // Find the maximum amplitude for normalization
    double maxAmp = 0.0;
    for (const auto &row : spectrogram) {
        for (double amplitude : row) {
            maxAmp = std::max(maxAmp, amplitude);
        }
    }

    if (maxAmp == 0.0) return;

    // render the spectrogram data
    for (int chunk = 0; chunk < numChunks; ++chunk) {
        for (int freq = 0; freq < numFrequencies; ++freq) {
            double amplitude = spectrogram[chunk][freq];
            int intensity = static_cast<int>((amplitude / maxAmp) * 255.0);

            // ensure intensity stays within 255
            intensity = std::clamp(intensity, 0, 255);

            QColor color;
            if (intensity <= 127) {
                // map from black (quietest amp) to red (middle amp)
                int red = std::clamp(intensity * 2, 0, 255);
                color.setRgb(red, 0, 0); // Only red increases
            } else {
                // map from red (middle) to yellow (loudest amp)
                int green = std::clamp((intensity - 127) * 2, 0, 255);
                color.setRgb(255, green, 0); // Red stays max, green increases, blue stays 0
            }

            QRectF rect(chunk * chunkWidth, height() - (freq + 1) * freqHeight, chunkWidth, freqHeight);
            painter.fillRect(rect, color);
        }
    }
    painter.end();

    // cache the rendered image as a pixmap
    cachedSpect = QPixmap::fromImage(image);
    graphicsScene->clear();
    graphicsScene->addPixmap(cachedSpect);
}

void Spectrograph::reset() {
    if (decoder) {
        decoder->stop();
        delete decoder;
        decoder = nullptr;
    }

    spectrogram.clear();
    accumulatedSamples.clear();
    update();
}
