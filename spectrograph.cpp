#include <QtWidgets>
#include <fftw3.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAudioDecoder>
#include "spectrograph.h"

/* This blog explains how to perform Short-Time Fourier Transform using FFTW.
 * It inspired the implementation of overlapping window processing and FFT setup.
 * https://ofdsp.blogspot.com/2011/08/short-time-fourier-transform-with-fftw3.html */

Spectrograph::Spectrograph(QWidget *parent)
    : QWidget(parent)
{
    /* hopSize can technically be adjusted
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

    // setup UI layout for toggling peak visualization
    QPushButton *peaksButton = new QPushButton("Highlight", this);
    peaksButton->setFixedSize(60,25);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(peaksButton); // Add the button
    buttonLayout->addStretch();  // Push everything else to the right

    // Add the button layout to the main layout at the top
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(); // Push everything else below the button

    setLayout(mainLayout);

    // set fixed size for spectrograph (can modify)
    setFixedSize(300, 200);

    connect(peaksButton, &QPushButton::clicked, this, &Spectrograph::showHighlights);
}


void Spectrograph::loadAudioFile(const QString &fileName) {

    // recieves file from upload audio in audio.cpp
    QUrl aName = QUrl::fromLocalFile(fileName);
    if (aName.isEmpty())
        return;

    reset();
    processAudioFile(aName);
}


void Spectrograph::processAudioFile(const QUrl &fileUrl) {

    if (!decoder) {
        // init QAudioDecoder for decoding audio buffers
        decoder = new QAudioDecoder(this);
        connect(decoder, &QAudioDecoder::bufferReady, this, &Spectrograph::bufferReady);
    }

    accumulatedSamples.clear(); // clear any prev samples
    decoder->setSource(fileUrl); // set decoder src to the new file
    decoder->start();
}


void Spectrograph::bufferReady() {
    // read the next available audio buffer and process it
    QAudioBuffer buffer = decoder->read();
    handleAudioBuffer(buffer);
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
        setupSpectrograph(windowSamples);

        // remove processed samples based on hopSize
        accumulatedSamples = accumulatedSamples.mid(hopSize);
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
   // qDebug() << "before processing, accumulatedsamples size:" << accumulatedSamples.size();

    // signal length and number of chunks based on hopSize and window size
    int signalLength = accumulatedSamples.size();
    int numChunks = (signalLength - windowSize) / hopSize + 1;

    //qDebug() << "calculated numChunks:" << numChunks << "with accumulatedSamples size" << accumulatedSamples.size();

    if (numChunks > 0) {

        // resize spectrogram to accommodate new chunks
        spectrogram.resize(spectrogram.size() + numChunks, QVector<double>(windowSize));

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
            for (int i=0; i < windowSize; ++i) {
                //what is displayed from fft
                double real = fft_result[i][0];
                double imag = fft_result[i][1];

                // used this https://cplusplus.com/forum/beginner/251061/ and adjusted a tiny bit
                double amplitude = 2 * std::sqrt(real * real + imag * imag);
                // store amp to represent intensity for visualizing the spectrogram
                spectrogram[spectrogram.size() - numChunks + chunk][i] = amplitude;
            }
        }

        // remove processed samples
        int samplesProcessed = numChunks * hopSize;
        accumulatedSamples = accumulatedSamples.mid(samplesProcessed);
    }

    update(); // trigger repaint of spect
}


void Spectrograph::reset() {
    spectrogram.clear();
    accumulatedSamples.clear();
    update();
}


void Spectrograph::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // widget dimensions
    int width = this->width();
    int height = this->height();

    // num of chunks (time axis) and frequencies (freq axis)
    int numChunks = spectrogram.size();
    int numFrequencies = windowSize;

    // case for nothing
    if (numChunks == 0 || numFrequencies == 0)
        return;

    // find the maximum amplitude for normalization
    double maxAmp = 0.0;
    for (const auto &row : spectrogram) {
        for (double amplitude : row) {
            maxAmp = std::max(maxAmp, amplitude);
        }
    }

    if (maxAmp == 0.0)
        return;

    // render each chunk and frequency as a rectangle with color in it based on the amplitude
    double chunkWidth = static_cast<double>(width) / numChunks;
    double freqHeight = static_cast<double>(height) / (numFrequencies/100); //can adjust this shows enough

    for (int chunk = 0; chunk < numChunks; ++chunk) {
        for (int freq = 0; freq < numFrequencies; ++freq) {

            double amplitude = spectrogram[chunk][freq]; // get amp for curr chunk and freq
            int intensity = static_cast<int>((amplitude / maxAmp) * 255.0); // normalize our amplitude to 255 range
            int hue = 0;

            // can change these numbers
            // the higher the intensity the more yellow
            if (intensity > 120) {
                hue = static_cast<int>((60.0 * intensity) / 255.0);

            // medium should be more a red or purple range
            } else if (intensity > 45) {
                hue = static_cast<int>((51.0 * intensity) / 255.0);

            // lowest should be close to black
            } else {
                hue = static_cast<int>((10.0 * intensity) / 255.0);
            }

            QColor color = QColor::fromHsv(hue, 255, intensity);
            painter.setPen(Qt::NoPen);
            painter.setBrush(color);

            QRectF rect(chunk * chunkWidth, height - (freq + 1) * freqHeight, chunkWidth, freqHeight);
            painter.fillRect(rect, color);
        }
    }


    // cover lowest frequencies with black to enhance look od spect
    if (showHighlightsEnabled) {
        painter.setPen(QPen(Qt::black, 2));
        QVector<QPointF> peakPoints;

        for (int chunk = 0; chunk < numChunks; ++chunk) {
            int peakFreq = 0;
            double peakValue = 0.0;

            // find the highest freq
            for (int freq = 0; freq < numFrequencies; ++freq) {
                if (spectrogram[chunk][freq] > peakValue) {
                    peakFreq = freq;
                    peakValue = spectrogram[chunk][freq];
                }
            }

            if (peakValue / maxAmp < 0.75) { // target lower amplitudes to cover
                double x = chunk * chunkWidth + chunkWidth / 2.0; // center peak horizontally
                double y = height - (peakFreq + 1) * freqHeight; // position peak vertically
                peakPoints.append(QPointF(x, y));
            }
        }

        // draw lines connecting peaks
        for (int i = 1; i < peakPoints.size(); ++i) {
            painter.drawLine(peakPoints[i - 1], peakPoints[i]); // draw line connecting areas of low freq

        }
    }
}


void Spectrograph::showHighlights() {
    showHighlightsEnabled = !showHighlightsEnabled;
    update(); //repaint
}






