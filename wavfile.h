#ifndef WAVFILE_H
#define WAVFILE_H
#include <QtWidgets>

/*
 * File: wavfile.h
 * Description:
 *  This header file defines the 'WavFile' class, which focuses on file handling providing structured
 *  acces to the contents of a WAV file. It includes functionality to read, parse, and extract audio data
 *  and metadata from a WAV file. The class supports loading WAV files, reading header information, and
 *  extracting audio samples.
 *
 * Purpose:
 *  - Retrieves audio properties such as sample rate, number of channels, and bit depth
 *  - Extracts raw audio data and samples for further processing or visualization
 *
 * Key Members:
 *  - 'QString filePath': Path to the WAV file
 *  - 'int sampleRate': Sample rate of the audio file
 *  - 'int numChannels': Number of audio channels (1 for, mono, 2 for stereo)
 *  - 'int bitDepth': bit depth of each audio sample
 *  - 'QByteArray audioData': Extracted raw audio data from the file
 *  - 'Qlist<float> samples': Parsed audio samples as float values between -1.0 and 1.0
 *
 * Public Methods:
 *  - 'WavFile(const QString& filePath, QObject* parent = nullptr)': Constructor that initializes the WAV
 *    file with the provided file path
 *  - 'int getSampleRate() const': Returns the sample rate of the audio channels
 *  - 'int getnumChannels() const': Returns number of audio channels
 *  - 'int getBitDepth() const': Returns the bit depth of the audio file
 *  - 'QByteArray getAudioData() const': Returns the raw audio data as a byte array
 *  - 'QList<float> getAudioSamples() const': Returns the parsed audio samples
 *  - 'bool loadFile()': Loads and processes the WAV file
 *
 * Signals:
 *  - 'void fileLoaded(bool success)': Emits signal after file loaded: indicates success or failure
 *
 * Private Methods:
 *  - 'bool readHeader(const QByteArray& headerData)': Parses and validates the WAV file header
 *  - 'bool readData()': Validates the size of the audio data chunk
 *  - 'void collectAudioSamples()': Extracts individual audio samples from the raw audio data
 *
 * References:
 */

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
    QList<float> getAudioSamples() const;

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
    QList<float> samples;
};

#endif // WAVFILE_H
