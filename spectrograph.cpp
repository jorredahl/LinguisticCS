#include <QtWidgets>
#include <fftw3.h>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAudioDecoder>

#include "spectrograph.h"

/* Im mainly looking at this https://ofdsp.blogspot.com/2011/08/short-time-fourier-transform-with-fftw3.html */

Spectrograph::Spectrograph(QWidget *parent, int buttonIndex)
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

    // button for the spectrogram
    // later maybe a box with like a small image
    // each spect instance gets own button/layout
    if (buttonIndex == 1) {

        showSpect1 = new QPushButton("show spect", this);
        connect(showSpect1, &QPushButton::clicked, this, &Spectrograph::uploadAudio1);
        QHBoxLayout *spectLayout1 = new QHBoxLayout(this);
        spectLayout1->addWidget(showSpect1, 0, Qt::AlignLeft);
        setLayout(spectLayout1);

    } else if (buttonIndex == 2) {

        showSpect2 = new QPushButton("show spect2", this);
        connect(showSpect2, &QPushButton::clicked, this, &Spectrograph::uploadAudio2);
        QHBoxLayout *spectLayout2 = new QHBoxLayout(this);
        spectLayout2->addWidget(showSpect2, 0, Qt::AlignLeft);
        setLayout(spectLayout2);
    }
}


void Spectrograph::uploadAudio1() {
    QUrl aName = QFileDialog::getOpenFileUrl(this, "select audio file");
    if (aName.isEmpty())
        return;

    reset1();

    // handle audio processing
    processAudioFile(aName, 1);

    // bug - it does not play exactly when the spectrograph is shown or like in sync
   // player->setSource(aName);
   // player->play();

    // decode audio file to raw data
    // decoder = new QAudioDecoder(this);
    // decoder->setSource(aName);

    // connect(decoder, &QAudioDecoder::bufferReady, this, &Spectrograph::bufferReady);

    // decoder->start();
    //qDebug() << "audio started";
}

void Spectrograph::uploadAudio2() {
    QUrl aName = QFileDialog::getOpenFileUrl(this, "select audio file");
    if (aName.isEmpty())
        return;

    reset2();

    // handle audio processing
    processAudioFile(aName, 2);

    // bug - it does not play exactly when the spectrograph is shown or like in sync
    // player->setSource(aName);
    // player->play();

    // decode audio file to raw data
    // decoder = new QAudioDecoder(this);
    // decoder->setSource(aName);

    // connect(decoder, &QAudioDecoder::bufferReady, this, &Spectrograph::bufferReady);

    // decoder->start();
    //qDebug() << "audio started";
}


void Spectrograph::processAudioFile(const QUrl &fileUrl, int audioIndex) {

    if (audioIndex == 1) {
        decoder1 = new QAudioDecoder(this);
        decoder1->setSource(fileUrl);
        connect(decoder1, &QAudioDecoder::bufferReady, this, &Spectrograph::bufferReady1);
        decoder1->start();

    } else if (audioIndex == 2) {
        decoder2 = new QAudioDecoder(this);
        decoder2->setSource(fileUrl);
        decoder2->setSource(fileUrl);
        connect(decoder2, &QAudioDecoder::bufferReady, this, &Spectrograph::bufferReady2);
        decoder2->start();
    }
}


void Spectrograph::bufferReady1() {
    // handle buffer for audio1
    QAudioBuffer buffer = decoder1->read();
    handleAudioBuffer(buffer, 1);
}


void Spectrograph::bufferReady2() {
    // handle buffer for audio2
    QAudioBuffer buffer = decoder2->read();
    handleAudioBuffer(buffer, 2);
}


void Spectrograph::handleAudioBuffer(const QAudioBuffer &buffer, int audioIndex) {
    const qint16 *data = buffer.constData<qint16>();
    int sampleCount = buffer.sampleCount();

    if (audioIndex == 1) {
        // Process for audio 1
        const qint16 *data = buffer.constData<qint16>();
        int sampleCount = buffer.sampleCount();

        // append incoming samples
        for  (int i = 0; i < sampleCount; ++i) {
            accumulatedSamples1.append(static_cast<double>(data[i]));
        }

        // Process in overlapping windows
        while (accumulatedSamples1.size() >= getWindowSize()) {

            // extract a chunk of windowSize for processing
            QVector<double> windowSamples = accumulatedSamples1.mid(0, getWindowSize());
            setupSpectrograph(windowSamples);

            // remove processed samples based on hopSize
            accumulatedSamples1 = accumulatedSamples1.mid(hopSize);
        }
    } else if (audioIndex == 2) {

        // append incoming samples
        for  (int i = 0; i < sampleCount; ++i) {
            accumulatedSamples2.append(static_cast<double>(data[i]));
        }

        // Process in overlapping windows
        while (accumulatedSamples2.size() >= getWindowSize()) {

            // extract a chunk of windowSize for processing
            QVector<double> windowSamples = accumulatedSamples2.mid(0, getWindowSize());
            setupSpectrograph(windowSamples);

            // remove processed samples based on hopSize
            accumulatedSamples2 = accumulatedSamples2.mid(hopSize);
        }
    }
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


void Spectrograph::setupSpectrograph(QVector<double> &accumulatedSamples) {

    // continuous stream of data
   // accumulatedSamples.append(samples);

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


void Spectrograph::reset1() {
    spectrogram.clear();
    accumulatedSamples1.clear();
    update();
}

void Spectrograph::reset2() {
    spectrogram.clear();
    accumulatedSamples2.clear();
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




