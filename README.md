# CS50 Final Project - Electric Guitar Recorder

Digital transformation has taken over the world in many areas. With the newcoming 5G and IoT technologies it will be crucial for musicians to relocate their workspace to fully digital systems. This project serves as the first checkpoint for this conversion as it allows you to record your electric guitar sound, then play it back via output speakers using terminal scripts.

As a musician and electronics engineering student I am very into digital audio workstations, software amplifiers and plugins. This projects lays the first stone to the audio signal processing. I am willing to improve the project in future and dive deeper into the digital signal processing aspects of it synchronously with my college courses.

This project has made under 'a hardware-based application for which you program some device' idea proposed by CS50.

Technologies used:
- C programming language
- PortAudio library
- ALSA
- libasound Linux package

## How to use this program

First of all you need an audio interface for your electric guitar to connect to the computer. I have used the Rocksmith Guitar Adapter to test and record my guitar audio. It allows you to record around 7 second and plays it afterwards. I have tested my code using Advanced Linux Sound Architecture API for sound card device driver on Ubuntu. It is possible to face some errors using it on Windows using ASIO, WASAPI like interfaces.

# Problems I have faced

I started the project using libusb library for cpp. It was quite challenging problem to solve audio transfers using libusb. PortAudio offers a great functionality with founding a solid framework to handle audio streams.
Even though PortAudio is rather complicated library offering abstract data structures and functions to do such handles, thanks to the example codes uploaded by developers it is apprehensible to grab the ideas.

Identifying the I/O devices is a little  mystery for me and I have set my device input by implementing its name into the code. Selecting sample rates is notable subject for both input and output devices. Dealing with the sample rates reminded me of Nyquist Rates for sampling that we discussed in my Digital Signal Processing lecture at Istanbul Technical University.

Rocksmith Cable has 1 channel for output and my spekakers has 2 channels. That arised a playback audio  of  x2 speed and x2 frequency (The notes that I play on my guitar pitched to the octave sound). Selecting right sample rates solved the problem.

# Future Improvements

It would be great to upgrade the program to the real time I/O. I would like to analyze the real time input's frequency domain by applying Fast Fourier Transform. Making of a tuner can be the first checkpoint.

# How to launch (ubuntu 16.04)
Clone the code: git clone https://github.com/topcuemre/cs50-final-project.git
- sudo apt-get install -y libasound2-dev is a dependency for ALSA
- gcc -o query query.c -lrt -lasound -lpthread -lportaudio


Emre Topcu 12/31/2021
