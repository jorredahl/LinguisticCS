#ifndef WAVFILE_H
#define WAVFILE_H
#include <QtWidgets>

class WavFile : public QObject
{
    Q_OBJECT
    //44byte header
    //audio info
public:
    explicit WavFile(const QString& filePath, QObject* parent = nullptr);

    //getters
    int getSampleRate() const;
    int getNumChannels() const;
    int getBitDepth() const;
    QByteArray getAudioData() const;
    QList<qint16> getAudioSamples() const;

    //loading function to process file
    bool loadFile();

signals:
    void fileLoaded(bool success);

private:
    //parsing methods
    bool readHeader(const QByteArray& headerData);
    bool readData();
    void collectAudioSamples();

    //member variables
    QString filePath;
    int sampleRate;
    int numChannels;
    int bitDepth;
    int dataSize;
    QByteArray audioData;
    QList<qint16> samples;
};

#endif // WAVFILE_H
