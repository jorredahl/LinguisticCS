#include <QtWidgets>
#include <fftw3.h>
#include "spectrograph.h"


Spectrograph::Spectrograph(QWidget *parent)
    : QWidget(parent)
{
    hopSize = windowSize / 2;

    data = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * windowSize);
    fft_result = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * windowSize);
    plan = fftw_plan_dft_1d(windowSize, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE); // 1d computation

    // calc hamming window for smoothing out
    hammingWindow(windowSize, hammingWindowValues);
}

// destructor cleanup
// free mem and destroy plan
Spectrograph::~Spectrograph() {
    fftw_destroy_plan(plan);
    fftw_free(data);
    fftw_free(fft_result);
}


// some numbers from example https://ofdsp.blogspot.com/2011/08/short-time-fourier-transform-with-fftw3.html
// mitigate edge effects by reducing amplitude of start/end of each data chunck
void Spectrograph::hammingWindow(int windowLength, QVector<double> &window) {

    window.resize(windowLength);  // make sure the vector is the correct size
    for (int i = 0; i < windowLength; i++) {
        window[i] = 0.54 - 0.46 * cos(2 * M_PI * i / (windowLength - 1));
    }
}
    // for(int i = 0; i < windowLength; i++) {
    //     window[i] = 0.54 - 0.46 * cos(2 * M_PI * i / (windowLength - 1));
    // }


void Spectrograph::setupSpectrograph(const QVector<double> &samples) {


    /* THIS IS NOT GOOD BUT VISUALLY I THINK IT LOOKS BETTER */

    // accumulatedSamples.append(samples);

    // int signalLength = accumulatedSamples.size();
    // int numChunks = (signalLength - windowSize) / hopSize + 1;

    // // Resize the spectrogram to hold data for each chunk
    // spectrogram.resize(numChunks, QVector<double>(windowSize / 2 + 1));

    // qDebug() << "Total accumulated samples:" << accumulatedSamples.size();
    // qDebug() << "Number of chunks:" << numChunks;

    // for (int chunk = 0; chunk < numChunks; ++chunk) {
    //     int chunkPosition = chunk * hopSize;

    //     qDebug() << "Chunk" << chunk << "starting at sample" << chunkPosition;


    //     // apply the precomputed Hamming window to each sample in the chunk
    //     for (int i = 0; i < windowSize; ++i) {
    //         int readIndex = chunkPosition + i;
    //         data[i][0] = (readIndex < accumulatedSamples.size()) ? accumulatedSamples[readIndex] * hammingWindowValues[i] : 0.0;
    //         data[i][1] = 0.0;  // Imaginary part is zero
    //     }

    //     // Execute the FFT on this windowed chunk
    //     fftw_execute(plan);

    //     // Store magnitudes of each frequency component in the spectrogram
    //     for (int i = 0; i < windowSize / 2 + 1; ++i) {
    //         double real = fft_result[i][0];
    //         double imag = fft_result[i][1];
    //         spectrogram[chunk][i] = std::sqrt(real * real + imag * imag);  // magnitude
    //     }
    // }

    /* THIS I THINK IS DOING THE CHUNKS BETTER BUT VISUALLY NOT EASY TO READ */

    // accumulatedSamples.append(samples);

    //
    // qDebug() << "Before processing, accumulatedSamples size:" << accumulatedSamples.size();

    // int signalLength = accumulatedSamples.size();
    // int numChunks = (signalLength - windowSize) / hopSize + 1;  // Calculate how many chunks we can process

    // qDebug() << "Calculated numChunks:" << numChunks << "with accumulatedSamples size:" << accumulatedSamples.size();

    // // only process if we have enough samples for at least one chunk
    // if (numChunks > 0) {
    //     // resize spectrogram based on new chunks
    //     spectrogram.resize(spectrogram.size() + numChunks, QVector<double>(windowSize / 2 + 1));

    //     // process each chunk starting from the last processed position
    //     for (int chunk = 0; chunk < numChunks; ++chunk) {
    //         int chunkPosition = chunk * hopSize;
    //         qDebug() << "Processing chunk" << chunk << "starting at sample" << chunkPosition;

    //         for (int i = 0; i < windowSize; ++i) {
    //             int readIndex = chunkPosition + i;
    //             data[i][0] = (readIndex < accumulatedSamples.size()) ? accumulatedSamples[readIndex] * hammingWindowValues[i] : 0.0;
    //             data[i][1] = 0.0;  // Imaginary part is zero
    //         }

    //         fftw_execute(plan);

    //         for (int i = 0; i < windowSize / 2 + 1; ++i) {
    //             double real = fft_result[i][0];
    //             double imag = fft_result[i][1];
    //             spectrogram[chunk][i] = std::sqrt(real * real + imag * imag);
    //         }

    //         // this i found online
    //         // for (int i = 0; i < windowSize / 2 + 1; ++i) {
    //         //     double real = fft_result[i][0];
    //         //     double imag = fft_result[i][1];
    //         //     double magnitude = std::sqrt(real * real + imag * imag);

    //         //     // convert to dB scale
    //         //     double dBValue = 10.0 / log(10.0) * log(magnitude + 1e-6);
    //         //     dBValue = qBound(0.0, dBValue / 96.0, 1.0);  // Normalize to [0,1]

    //         //     spectrogram[spectrogram.size() - numChunks + chunk][i] = dBValue;  // Store normalized dB value
    //         // }

    //     }

    //     // trim accumulatedSamples to keep samples that havent been processed
    //     int samplesProcessed = numChunks * hopSize;
    //     accumulatedSamples = accumulatedSamples.mid(samplesProcessed);

    //     qDebug() << "After trimming, accumulatedSamples size:" << accumulatedSamples.size();
    // }

    // // update the display
    // update();


    /* COMBINATION OF THE TOP TWO BUT OUTPUT STILL KIND OF WEIRD  */
    // continuous stream of data
    accumulatedSamples.append(samples);

    // Debugging output to understand sample accumulation
    qDebug() << "before processing, accumulatedsamples size:" << accumulatedSamples.size();

    int signalLength = accumulatedSamples.size();
    int numChunks = (signalLength - windowSize) / hopSize + 1;  // calcu how many chunks we can process

    qDebug() << "calculated numChunks:" << numChunks << "with accumulatedSamples size:" << accumulatedSamples.size();

    // Only process if we have enough samples for at least one chunk
    if (numChunks > 0) {
        // resize spectrogram based on new chunks
        spectrogram.resize(spectrogram.size() + numChunks, QVector<double>(windowSize / 2 + 1));

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

            for (int i = 0; i < windowSize / 2 + 1; ++i) {
                double real = fft_result[i][0];
                double imag = fft_result[i][1];
                double magnitude = std::sqrt(real * real + imag * imag);  // calculate raw magnitude

                // dB scaling but keep a part of the raw magnitude effect
                double dBValue = 10.0 / log(10.0) * log(magnitude + 1e-6); // Convert to dB scale
                dBValue = qBound(0.0, dBValue / 96.0, 1.0);  // normalize to [0,1]

                // combine raw magnitude with dBValue to control line intensity
                spectrogram[spectrogram.size() - numChunks + chunk][i] = 0.5 * dBValue + 0.5 * magnitude;  // mix raw and dB
            }
        }

        // trim accumulatedSamples to keep only unprocessed samples
        int samplesProcessed = numChunks * hopSize;
        accumulatedSamples = accumulatedSamples.mid(samplesProcessed);

        qDebug() << "After trimming, accumulatedSamples size:" << accumulatedSamples.size();
    }

    // update the display
    update();
}


void Spectrograph::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    int width = this->width();
    int height = this->height();
    int numChunks = spectrogram.size();
    int numFrequencies = windowSize / 2 + 1; // use lower half

    // case for nothing
    if(numChunks == 0 || numFrequencies == 0)
        return;

    double maxMagnitude = 0.0;
    for(const auto &row : spectrogram) {
        for(double magnitude : row) {
            maxMagnitude = std::max(maxMagnitude, magnitude);
        }
    }

    double chunkWidth = static_cast<double>(width) / numChunks;
    double freqHeight = static_cast<double>(height) / (numFrequencies/2); // only half

    // freq on y-axis
    for(int chunk = 0; chunk < numChunks; ++chunk) {
        for(int freq = 0; freq < numFrequencies; ++freq) {

            double magnitude = spectrogram[chunk][freq];

            int intensity = static_cast<int>((magnitude / maxMagnitude) * 255.0);
            QColor color = QColor(intensity, intensity, intensity);  // keep it greyscale

            //color and thickness
            painter.setPen(Qt::NoPen);  // no border
            painter.setBrush(color);

           // QRectF rect(chunk * chunkWidth, height - (freq + 1) * freqHeight, chunkWidth, freqHeight);
            //testing height
            QRectF rect(chunk * chunkWidth, height - (freq + 1) * freqHeight, chunkWidth, freqHeight * 1.5);

            painter.fillRect(rect, color);
        }
    }
}
