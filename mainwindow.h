#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
class MainWindow : public QMainWindow
{
    Q_OBJECT
    QHBoxLayout *mainLayout;
    QPushButton *uploadAudioButton;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void uploadAudio();
};
#endif // MAINWINDOW_H
