<p align="center">
  <a href="" rel="noopener">
 <img width=200px height=200px src="https://github.com/jorredahl/LinguisticCS/blob/readme/resources/icons/phonetics_logo.jpg" alt="Project logo"></a>
</p>

<h3 align="center">Phonetics Visualizer</h3>

<div align="center">

  [![Status](https://img.shields.io/badge/status-active-success.svg)]() 
  [![License](https://img.shields.io/badge/license-MIT-blue.svg)](/LICENSE)

</div>

---

<p align="center"> A minimal app for visualizing and comparing phonetic audio samples using waveforms and spectrograms.  A user can compare two waveforms and examine differences in their waveform and spectrogram to analyze their pronounciation of words when learning a language.  
    <br> 
  Built using C++ and Qt.
</p>

## 📝 Table of Contents
- [About](#about)
- [Getting Started](#getting_started)
- [Usage](#usage)
- [Built Using](#built_using)
- [TODO](../TODO.md)
- [Contributing](../CONTRIBUTING.md)
- [Authors](#authors)
- [Acknowledgments](#acknowledgement)

## 🧐 About <a name = "about"></a>
Write about 1-2 paragraphs describing the purpose of your project.

This project was developed as a final project for Middlebury College's CS318: OOP & GUI Development.  The original concept of the Phonetics Visualizer is to aid language learners' pronounciation by algorithmically identifying differences in pronounciation between an uploaded and a recorded sentence.  These could be differences in length of phoneme, pitch, relative emphasis, et cetera.  We believe that identifying these differences for the listener, then giving them capability to isolate, listen, and visualize these shortcomings in pronounciation would significantly enhance the language learning process.

Although over the short course of the semester we were not able to implement an algorithmic framework to recognize differences in these across recordings, our team developed visualization and playback capabilities that allow for user analysis.  



## 🏁 Getting Started <a name = "getting_started"></a>
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See [deployment](#deployment) for notes on how to deploy the project on a live system.

### Prerequisites
What things you need to install the software and how to install them.

1. An instance of Qt and Qt Creator.  This is for the development version.
    1. This requires XCode if you are on a Mac.

3.  Right now the software is dependent on the [FFTW](https://www.fftw.org/) library.  We have been running it from within our [Qt](https://www.qt.io/) development instance, which is dynamically linking to the FFTW headers from our `\usr\local\lib` directory.  Hopefully for the production version we will have this code fully linked and included in a standalone executable.  


### Installing
A step by step series of examples that tell you how to get a development env running.

1. Install XCode, Qt, Qt Creator.  There are plenty of better guides to do this online than anything we could write.


2. Git clone the project into your desired directory.

```
git clone https://github.com/jorredahl/LinguisticCS.git
```
3. Install [FFTW](https://www.fftw.org/).  We did this using homebrew, then to get the FFTW package to where our `.pro` file was looking we used 

 ```
 sudo cp /opt/homebrew/Cellar/fftw/3.x.x/lib/* /usr/local/lib/

 ```



## 🎈 Usage <a name="usage"></a>
Upon starting the Phonetics Visualizer, the user will be greeted with some controls and a blank waveform visualizer.  Almost every control is initially disabled - you have to first upload an audio file.  

### Loading audio files
The first step is to load an audio file to compare.  
1. Click the top "Upload" button to upload a source file to compare your audio against.
2. There will be a pop-up file explorer.  Navigate in your computer directory to the desired audio file and click "Open" in the bottom right to open that.


**Note: only `.wav` files are accepted at the moment.  A 24 or 32 bitrate is encouraged for the file.**

### Playing audio files
Once your audio file is loaded, you can click the corresponding play button to play and pause.  You can see the scrubber's corresponding progress through the waveform to the right.

### Visualizing the audio
There are two primary visualization methods within the Phonetics Visualizer: a waveform and a spectrogram

#### The Waveform

#### The Spectrogram

### Zooming in 
There are two ways to zoom in


## ⛏️ Built Using <a name = "built_using"></a>
- C++
- [Qt](https://www.qt.io/)
- [FFTW](https://www.fftw.org/)


## ✍️ Authors <a name = "authors"></a>
- [@jkwarren](https://github.com/jkwarren)
- [@mbamaca](https://github.com/mbamaca)
- [@terryluongo](https://github.com/terryluongo)
- [@jorredahl](https://github.com/jorredahl)
- Abraham Merino

See also the list of [contributors](https://github.com/kylelobo/The-Documentation-Compendium/contributors) who participated in this project.

## 🎉 Acknowledgements <a name = "acknowledgement"></a>
- Thank you to Professor Swenton for the plethora of help throughout the semester!
- Thank you to Professor Baird for the linguistics resources and Professor Abe for the Japenese resources!
