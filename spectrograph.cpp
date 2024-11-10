#include <QtWidgets>
#include <fftw3.h>
#include "spectrograph.h"

/* Im mainly looking at this https://ofdsp.blogspot.com/2011/08/short-time-fourier-transform-with-fftw3.html
 *
 *
 * OTHER:
 * will start looking at how i can find differences, will prob be the rects not sure yet...
 *
 */

Spectrograph::Spectrograph(QWidget *parent)
    : QWidget(parent)
{
    /* hopSize can techniclly be adjusted
     *
     * EXAMPLE: if we do windowSize/5 then 5 chunks will be processed , you can test and see w the qDebugs
     *
     * right now bc the hopSize =  windowSize there will just be one chunk being processed
     *
     * i think that this gives the cleanest output
     * */
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

    qDebug() << "before processing, accumulatedsamples size:" << accumulatedSamples.size();

    // how long wav file
    int signalLength = accumulatedSamples.size();
    int numChunks = (signalLength - windowSize) / hopSize + 1;  // calc how many chunks we can process

    qDebug() << "calculated numChunks:" << numChunks << "with accumulatedSamples size" << accumulatedSamples.size();

    // Only process if we at least one chunk
    if (numChunks > 0) {

        // resize spectrogram based on new chunks
        spectrogram.resize(spectrogram.size() + numChunks, QVector<double>(windowSize));

        // process each chunk starting from the last processed position
        for (int chunk = 0; chunk < numChunks; ++chunk) {

            int chunkPosition = chunk * hopSize;
            qDebug() << "Processing chunk" << chunk << "starting at sample" << chunkPosition;

            for (int i = 0; i < windowSize; ++i) {
                int readIndex = chunkPosition + i;
                // ensure we dont go out of bounds for accum samples
                data[i][0] = (readIndex < accumulatedSamples.size()) ? accumulatedSamples[readIndex] * hammingWindowValues[i] : 0.0;
                data[i][1] = 0.0;  // empty space
            }

            fftw_execute(plan);

            for (int i=0; i < windowSize; ++i) {
                //what is displayed
                double shown = fft_result[i][0];
                double empty = fft_result[i][1];
                double magnitude = std::sqrt(shown * shown + empty * empty);

                // control the line intensity for mag
                spectrogram[spectrogram.size() - numChunks + chunk][i] = magnitude;
            }
        }

        // trim accumulatedSamples to keep only unprocessed samples
        int samplesProcessed = numChunks * hopSize;
        accumulatedSamples = accumulatedSamples.mid(samplesProcessed);

        qDebug() << "after trimming, accumulatedSamples size:" << accumulatedSamples.size();
    }

    update(); // display
}

// color intensity is determined by the magnitude of the frequency component, not sure if i should add more rn
void Spectrograph::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    int width = this->width();
    int height = this->height();
    int numChunks = spectrogram.size();
    int numFrequencies = windowSize;

    // case for nothing
    if (numChunks == 0 || numFrequencies == 0)
        return;

    double maxMagnitude = 0.0;
    // loop through rows
    for (const auto &row : spectrogram) {
        // calc mag
        for (double magnitude : row) {
            maxMagnitude = std::max(maxMagnitude, magnitude);
        }
    }

    double chunkWidth = static_cast<double>(width) / numChunks;
    double freqHeight = static_cast<double>(height) / (numFrequencies/20); //can adjust

    for (int chunk = 0; chunk < numChunks; ++chunk) {
        for (int freq = 0; freq < numFrequencies; ++freq) {

            double magnitude = spectrogram[chunk][freq]; // get mag for curr chunk and freq

            int intensity = static_cast<int>((magnitude / maxMagnitude) * 255.0);
            QColor color = QColor(intensity, intensity, intensity);  // keep greyscale fornow

            painter.setPen(Qt::NoPen);
            painter.setBrush(color);

            QRectF rect(chunk * chunkWidth, height - (freq + 1) * freqHeight, chunkWidth, freqHeight);

            painter.fillRect(rect, color);
        }
    }
}

void Spectrograph::reset() {
    spectrogram.clear();
    accumulatedSamples.clear();
    update();
}
