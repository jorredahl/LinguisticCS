#include <QtWidgets>
#include <fftw3.h>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAudioDecoder>

#include "spectrograph.h"
//#include "audio.h"

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
     * i think that this gives the cleanest , its gets laggy after anything else
     * */
    hopSize = windowSize;

    data = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * windowSize);
    fft_result = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * windowSize);
    plan = fftw_plan_dft_1d(windowSize, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE); //1d computation

    // calc hamming window for smoothing out
    hammingWindow(windowSize, hammingWindowValues);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QPushButton *peaksButton = new QPushButton("Peaks", this);
    connect(peaksButton, &QPushButton::clicked, this, &Spectrograph::showPeaks);
    mainLayout->addWidget(peaksButton);

    setLayout(mainLayout);

    // set fixed size for spectrograph , can change later
    setFixedSize(190, 115);
}


void Spectrograph::loadAudioFile(const QString &fileName) {

    QUrl aName = QUrl::fromLocalFile(fileName);
    if (aName.isEmpty())
        return;

    reset();
    processAudioFile(aName);
}


void Spectrograph::processAudioFile(const QUrl &fileUrl) {

    if (!decoder) {
        decoder = new QAudioDecoder(this);
        connect(decoder, &QAudioDecoder::bufferReady, this, &Spectrograph::bufferReady);
    }

    decoder->setSource(fileUrl);
    decoder->start();
}


void Spectrograph::bufferReady() {
    // handle buffer for audio1
    QAudioBuffer buffer = decoder->read();
    handleAudioBuffer(buffer);
}


void Spectrograph::handleAudioBuffer(const QAudioBuffer &buffer) {
    const qint16 *data = buffer.constData<qint16>();
    int sampleCount = buffer.sampleCount();


    // append incoming samples
    for  (int i = 0; i < sampleCount; ++i) {
        accumulatedSamples.append(static_cast<double>(data[i]));
    }

    // Process in overlapping windows
    while (accumulatedSamples.size() >= getWindowSize()) {

        // extract a chunk of windowSize for processing
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
    double freqHeight = static_cast<double>(height) / (numFrequencies/100); //can adj this shows enough

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


    // that below is WORKING ISH
    /*
     *
     * other otion or idea is that
     * spectral view -->
     *
     * red / yeellow the same as b4
     *
     * blue lines drawn over spect is peak frequencies
     * darker area --> purple and black quieter freq meh
     *
     *
     * removes background stuff ? noise ?
     *
     * */


    // peaks of amplitude
    if (showPeaksEnabled) {
        painter.setPen(QPen(Qt::blue, 2));
        QVector<QPointF> peakPoints;

        for (int chunk = 0; chunk < numChunks; ++chunk) {
            // check these values later , and peak freq
            int peakFreq = 0;
            double peakValue = 1.0;

            // Find the peak frequency in this chunk
            for (int freq = 0; freq < numFrequencies; ++freq) {
                if (spectrogram[chunk][freq] > peakValue) {
                    peakFreq = freq;
                    peakValue = spectrogram[chunk][freq];
                }
            }

            int intensityAmp = static_cast<int>((peakValue / maxAmp) * 255.0);

            // only add points for peaks above a certain intensity threshold , can adjust number

            if (intensityAmp  > 210) {
                double x = chunk * chunkWidth + chunkWidth / 2.0;
                double y = height - (peakFreq + 1) * freqHeight;
                peakPoints.append(QPointF(x, y)); // store the peak point
            }
        }

        // Draw the lines connecting the peaks
        for (int i = 1; i < peakPoints.size(); ++i) {
            painter.drawLine(peakPoints[i - 1], peakPoints[i]);
        }
    }
}


void Spectrograph::showPeaks() {
    showPeaksEnabled = !showPeaksEnabled;
    update(); //repaint
}






