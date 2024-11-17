#include "wavfile.h"
#include <QtWidgets>

WavFile::WavFile(const QString& filePath, QObject* parent)
    : QObject(parent), filePath(filePath), sampleRate(0), numChannels(0), bitDepth(0), dataSize(0) {
    // initialize file path and default values

}

bool WavFile::loadFile() {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Load Error: Could not open file";
        emit fileLoaded(false);
        return false;
    }

    QByteArray fileContent = file.readAll(); //read file into QByteArray
    file.close();

    if (fileContent.size() < 44) {
        qWarning() << "File Error: File header too small to be valid WAV file";
        emit fileLoaded(false);
        return false;
    }

    //set header and audiodata byte chunks for parsing
    QByteArray headerData = fileContent.left(44);
    audioData = fileContent.mid(44);

    if (!readHeader(headerData)) {
        qWarning() << "File Error: Invalid WAV header";
        emit fileLoaded(false);
        return false;
    }

    //collect samples of data
    collectAudioSamples();

    emit fileLoaded(true);
    return true;
}

bool WavFile::readHeader(const QByteArray& headerData) {
    if (headerData.mid(0, 4) != "RIFF") {
        qWarning() << "File Error: Invalid RIFF header";
        return false;
    } else if (headerData.mid(8, 4) != "WAVE") {
        qWarning() << "File Error: Invalid WAV header";
        return false;
    }

    // get channels (position 22)
    numChannels = qFromLittleEndian<quint16> (reinterpret_cast<uchar*> (headerData.mid(22, 2).data()));

    // get sample rate (pos 24-27)
    sampleRate = qFromLittleEndian<quint32> (reinterpret_cast<uchar*> (headerData.mid(24, 4).data()));

    // get bit depth (pos 34)
    bitDepth = qFromLittleEndian<quint16> (reinterpret_cast<uchar*> (headerData.mid(34, 2).data()));

    // get data size (pos 40-43)
    dataSize = qFromLittleEndian<quint32> (reinterpret_cast<uchar*> (headerData.mid(40, 4).data()));

    return true;
}

bool WavFile::readData() {
    if (audioData.size() < dataSize) {
        qWarning() << "File Error: Audio data smaller than expected";
        return false;
    }

}
void WavFile::collectAudioSamples(){
    // Collects each sample from 16-bit WAV file data as a signed integer (32768 to -32768)
    for (int i = 0; i < audioData.size(); i += 2) {
        if (i + 1 < audioData.size()) {
            qint16 sample = qFromLittleEndian<qint16>(reinterpret_cast<const unsigned char*>(audioData.mid(i, 2).constData()));
            samples.append(sample);
        }
    }
}

//access properties
int WavFile::getSampleRate() const {
    return sampleRate;
}

int WavFile::getNumChannels() const {
    return numChannels;
}

int WavFile::getBitDepth() const {
    return bitDepth;
}

QByteArray WavFile::getAudioData() const {
    return audioData;
}

QList<qint16> WavFile::getAudioSamples() const {
    return samples;
}
