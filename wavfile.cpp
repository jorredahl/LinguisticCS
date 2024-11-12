#include "wavfile.h"
#include <QtWidgets>

WavFile::WavFile(const QString& filePath, QObject* parent)
    : QObject(parent), filePath(filePath), sampleRate(0), numChannels(0), bitDepth(0), dataSize(0) {
    // initialize file path and default values
    
}

bool WavFile::loadFile() {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        // QMessageBox::warning(this, "Load Error", "Could not open file");
        emit fileLoaded(false);
        return false;
    }

    QByteArray fileContent = file.readAll(); //read file into QByteArray
    file.close();

    if (fileContent.size() < 44) {
        // QMessageBox::warning(this, "File Error", "File too small to be valid WAV file");
        emit fileLoaded(false);
        return false;
    }

    //set header and audiodata byte chunks for parsing
    QByteArray headerData = fileContent.left(44);
    audioData = fileContent.mid(44);

    if (!readHeader(headerData)) {
        // QMessageBox::warning(this, "File Error", "Invalid WAV header");
        emit fileLoaded(false);
        return false;
    }

    emit fileLoaded(true);
    return true;
}

bool WavFile::readHeader(const QByteArray& headerData) {
    if (headerData.mid(0, 4) != "RIFF") {
        // QMessageBox::warning(this, "File Error", "Invalid RIFF header");
        return false;
    } else if (headerData.mid(8, 4) != "WAVE") {
        // QMessageBox::warning(this, "File Error", "Invalid WAVE header");
        return false;
    }

    // get channels (position 22)
    // numChannels =

    // get sample rate (pos 24-27)
    // sampleRate =

    // get bit depth (pos 34)
    // bitDepth =

    // get data size (pos 40-43)
    // dataSize =

    return true;
}

bool WavFile::readData() {
    if (audioData.size() < dataSize) {
        // QMessageBox::warning(this, "File Error", "Audio data smaller than expected");
        return false;
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
