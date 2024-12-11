#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include "audio.h"

/*
 * File: mainwindow.h
 * Description:
 *  This header file defines the 'MainWindow' class, which provides the main UI for the
 *  application. It inherits from QMainWindow and creates two instances of the 'Audio'
 *  class to represent dual Audio widgets. It uses a vertical layout (QVBoxLayout) for
 *  organization and embeds the Audio widgets.
 *
 * Purpose:
 *  - Serves as primary UI, enabling interactions with audio playback
 *  - Organizes main layout within a 'QVBoxLayout'
 *  - Initializes and manages child components for the two Audio objects
 *
 * Key Members:
 *  - 'QVBoxLayout *mainLayout': Vertical layout for arranging UI elements (audio players)
 *  - 'Audio *audio1': First audio player widget
 *  - 'Audio *audio2': Second audio player widget
 *
 * Public Methods:
 *  - 'MainWindow(QWidget *parent = nullptr)': Constructor to initialize main window layout
 *  - '~MainWindow()': Destructor to clean resources
 *
 * References:
 *  -...
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QVBoxLayout *mainLayout;
    Audio *audio1;
    Audio *audio2;


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void audio2ConnectAllowed(bool secondAudioExists);
    void audio2Connect(bool connectAudios);
    void handleEndOfAudio2(bool disc);

signals:
    void disableAudio2(bool disableAudio);
    void canEnableAudioAlignment(bool enable);
};

#endif // MAINWINDOW_H
