#include <QtWidgets>
#include <fftw3.h>
#include "spectrograph.h"

#include <QGraphicsScene>
#include <QGraphicsView>

/* Im mainly looking at this https://ofdsp.blogspot.com/2011/08/short-time-fourier-transform-with-fftw3.html */

Spectrograph::Spectrograph(QWidget *parent)
    : QWidget(parent)
{
    /* hopSize can techniclly be adjusted
     *
     * EXAMPLE: if we do windowSize/5 then 5 chunks will be processed , you can test and see w the qDebugs
     *
     * right now bc the hopSize =  windowSize there will just be one chunk being processed
     *
     * i think that this gives the cleanest , also its faster
     * */
    setMouseTracking(true);
    grabKeyboard();

    hopSize = windowSize;

    data = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * windowSize);
    fft_result = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * windowSize);
    plan = fftw_plan_dft_1d(windowSize, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE); //1d computation

    // calc hamming window for smoothing out
    hammingWindow(windowSize, hammingWindowValues);
}

// free mem and destroy plan
Spectrograph::~Spectrograph() {
    fftw_destroy_plan(plan);
    fftw_free(data);
    fftw_free(fft_result);
}

void Spectrograph::hammingWindow(int windowLength, QVector<double> &window) {

    window.resize(windowLength);  // make sure the vector is the correct size
    for (int i = 0; i < windowLength; i++) {
        window[i] = 0.54 - 0.46 * cos(2 * M_PI * i / (windowLength - 1));
    }
}

void Spectrograph::setupSpectrograph(const QVector<double> &samples) {

    // continuous stream of data
    accumulatedSamples.append(samples);

    //qDebug() << "before processing, accumulatedsamples size:" << accumulatedSamples.size();

    int signalLength = accumulatedSamples.size(); // how long wav file
    int numChunks = (signalLength - windowSize) / hopSize + 1;  // calc how many chunks we can process

    //qDebug() << "calculated numChunks:" << numChunks << "with accumulatedSamples size" << accumulatedSamples.size();

    // chunks already represents time based on fftw
    if (numChunks > 0) {
        // resize spectrogram based on new chunks
        spectrogram.resize(spectrogram.size() + numChunks, QVector<double>(windowSize));

        // process each chunk starting from the last processed position
        for (int chunk = 0; chunk < numChunks; ++chunk) {

            int chunkPosition = chunk * hopSize;
            //qDebug() << "Processing chunk" << chunk << "starting at sample" << chunkPosition;

            for (int i = 0; i < windowSize; ++i) {
                int readIndex = chunkPosition + i;
                // ensure we dont go out of bounds for accum samples
                data[i][0] = (readIndex < accumulatedSamples.size()) ? accumulatedSamples[readIndex] * hammingWindowValues[i] : 0.0;
                data[i][1] = 0.0;  // empty space
            }

            fftw_execute(plan);

            for (int i=0; i < windowSize; ++i) {
                //what is displayed from fft
                double real = fft_result[i][0];
                double imag = fft_result[i][1];

                // used this https://cplusplus.com/forum/beginner/251061/ and adjusted just a tiny bit
                double amplitude = 2 * std::sqrt(real * real + imag * imag);

                // store amp to represent intensity for visualizing the spectrogram
                spectrogram[spectrogram.size() - numChunks + chunk][i] = amplitude;

                //qDebug() << amplitude;
            }
        }

        // trim accumulatedSamples to keep only unprocessed samples
        int samplesProcessed = numChunks * hopSize;
        accumulatedSamples = accumulatedSamples.mid(samplesProcessed);
        //qDebug() << "after trimming, accumulatedSamples size:" << accumulatedSamples.size();
    }
    update(); // display
}


void Spectrograph::reset() {
    spectrogram.clear();
    accumulatedSamples.clear();
    update();
}

/* make this look better like adobe audition
 * frequency is along the vertical axis
 * time should be on the horizontal axis
 *
 *  amplitude closer to amp --> quieter amp
 *
 *  bright yellow --> louder the amplitude
 *
 *  in between --> varying levels of amp
 *  */
void Spectrograph::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QTransform transform;
    transform.scale(zoomFactor, zoomFactor);
    painter.setTransform(transform);

    int width = this->width();
    int height = this->height();
    int numChunks = spectrogram.size();
    int numFrequencies = windowSize;

    // case for nothing
    if (numChunks == 0 || numFrequencies == 0)
        return;

    double maxAmp = 0.0;
    for (const auto &row : spectrogram) {
        // calc mag
        for (double amplitude : row) {
           //maxMagnitude = std::max(maxMagnitude, magnitude);
            maxAmp = std::max(maxAmp, amplitude);
        }
    }

    if (maxAmp == 0.0)
        return;

    double chunkWidth = static_cast<double>(width) / numChunks;
    double freqHeight = static_cast<double>(height) / (numFrequencies/100); //can adj

    for (int chunk = 0; chunk < numChunks; ++chunk) {
        for (int freq = 0; freq < numFrequencies; ++freq) {

            double amplitude = spectrogram[chunk][freq]; // get amp for curr chunk and freq
            int intensity = static_cast<int>((amplitude / maxAmp) * 255.0); // normalize our amplitude to 255 range
            int hue = 0;

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
}

/* i can zoom in and out but its kind of weird
 *
 * should be a two finger scroll
 * to zoom in move two fingers up on the trackpad
 *
 * to zoom out should move two fingers down on the trackpad
 * its 1.02 bc it slows it down a bit
 * */
void Spectrograph::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() > 0) {
        zoomFactor *= 1.02;  // Zoom in
    } else {
        zoomFactor /= 1.02;  // Zoom out
    }
    update();
}





