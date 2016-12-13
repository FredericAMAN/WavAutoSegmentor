WavAutoSegmentor
================
WavAutoSegmentor is dedicated to segmentation of WAV audio files.  

It implements sound activity detection algorithm based on changes of energy level.  

This algorithm applies adaptive thresholds on the energy level of the three highest frequency coefficients of discrete wavelet transform.

Originally, this application was developed by the LIG laboratory in Grenoble to segment automatically recordings of speech corpora in order to process them by an automatic speech recognition system.

# Installing dependencies

This application requires:
* libc6

Dependencies installation on Linux (Ubuntu 16.04)
---------------------

```bash
$ sudo apt-get update
$ sudo apt-get install libc6
```

# Installing the application 

Installation on Linux
---------------------

```bash
$ cd WavAutoSegmentor
$ make
```

# Running the application

Run on Linux
---------------------

```bash
$ ./WavAutoSegmentor
```

You can use the following options:

```bash
$ ./WavAutoSegmentor-h
Usage: WavAutoSegmentor [-i <str>] [-o <str>] [-s <double>] [-e <double>] [-b <double>] [-a <double>] [-p <double>]
	-i	<default: data/in>	    Directory of the input WAV files (mono channel)
	-o	<default: data/out>	    Directory of the output segmented WAV files
	-s	<default: 0.0000005000>	Threshold to start the segments detection
	-e	<default: 0.0000005000>	Threshold to end the segments detection
	-b	<default: 0.000>	    Duration of added samples before the beginning of the detection (seconds)
	-a	<default: 0.000>	    Duration of added samples after the end of the detection (seconds)
	-w	<default: 0.250>	    Maximum duration of a silence in the signal, at which point detection stops (seconds)
	-v				            Display the version of WavAutoSegmentor
	-h				            This help message
```

Example of running with options:
```bash
$ ./WavAutoSegmentor -i data/in/data-test -o data/out/data-test -s 0.0001 -e 0.0001 -b 0.2 -a 0.2 -w 1
```

It should display something like:

```bash
WavAutoSegmentor v1.0
	-i Input directory:                 data/in/data-test   <default: data/in>
	-o Output directory:                data/out/data-test  <default: data/out>
	-s Threshold start:                 0.0001000000        <default: 0.0000005000>
	-e Threshold end:                   0.0001000000        <default: 0.0000005000>
	-b Added duration before detection: 0.200               <default: 0.000>
	-a Added duration after detection:  0.200               <default: 0.000>
	-w Maximum duration of a silence:   1.000               <default: 0.250>

**********************************
Input file:
	Name:               data/in/data-test/test1.wav
	Format:             WAV
	Channels:           1
	Sample rate (Hz):   16000
	Bits per sample:    16
	Duration (s):       275

Start splitting...
...End splitting

Output files:
	Number of segments:	48
	Directory:          data/out/data-test/25-11-2016_09:30:15.350
	Format:             WAV
	Channels:           1
	Sample rate (Hz):   16000
	Bits per sample:    16
	Mean duration (s):  1.48

**********************************
Input file:
	Name:               data/in/data-test/test2.wav
	Format:             WAV
	Channels:           1
	Sample rate (Hz):   16000
	Bits per sample:    16
	Duration (s):       206

Start splitting...
...End splitting

Output files:
	Number of segments:	36
	Directory:          data/out/data-test/25-11-2016_09:30:15.350
	Format:             WAV
	Channels:           1
	Sample rate (Hz):   16000
	Bits per sample:    16
	Mean duration (s):  1.57

**********************************
Number of input files processed: 2/2 (0/2 skipped)

```

In this example, the application has processed two files _test1.wav_ and _test2.wav_, resulting respectively in 48 and 36 segments.

Format of the input audio files
---------------------
The only input files format supported by the application is the **WAVE** format.  

WAV files must be **mono channel**; stereo files are not supported for the moment.  

The application process all the WAV mono files found in the folder defined with the option -i (or in data/in by default).

Results of the segmentation
---------------------
The cut segments are created in a sub-folder named with date and time and created by the application (e.g. 25-11-2016_09:30:15.350) in the folder defined by the option -o (or in data/out by default).

Optimization of the segmentation
---------------------

Optimization with the optional parameters:
* -s and -e: if the input signal has a high average amplitude, you can increase the thresholds (options -s and -e), or decrease them in the opposite case. The thresholds have to be lower than 1  
* -b and -a: if the detection cuts the beginning or the end of the segments, you can increase the values (in seconds) of the options -b and -a  
* -w: if the detection cuts inside the sentences, you can increase the value (in seconds) of the option -w  

In the output directory, there is a folder called "debug" (e.g. in data/out/data-test/25-11-2016_09:30:15.350/debug) where are the following files:
* *file*.wav: copy of the input original WAV file
* *file*_detect.wav: displays where detections (rectangular values 0.5) and pauses (rectangular values 0.25) took place
* *file*_split.wav: displays where the input WAV was cut to create segment files (rectangular values 0.25) in the output folder  

These debug files aim to be displayed in [Audacity](http://audacity.fr/) to help you to adjust the options (-s, -e, -b, -a or -w) until you get the wanted segmentation.

![Alt text](./doc/audacity.png?raw=true)

In the figure above (with options -s 0.0001 -e 0.0001 -b 0.2 -a 0.2 -w 1):
* (1): example of occurring detection
* (2): example of pause between 2 detections. A new detection occurs before reaching the value defined by the option -w
* (3): example of ending detection: no new detection when the value defined by the option -w is reached

The file split.txt in the debug folder contains information about:
* name: name of the segment WAV file
* fs: samples rate of the segment WAV file
* bit rate: samples rate of the segment WAV file
* start index: sample index in the input WAV file of the start cut
* end index: sample index in the input WAV file of the end cut
* start time: time in the input WAV file of the start cut
* end time: time in the input WAV file of the end cut
* duration: duration of the segment WAV file
* SNR: signal to noise ratio of the segment WAV file
* data octets: number of octets of data in the segment WAV file (doesn't take into account the 44 octets of the WAV file header)

The file log.txt in the debug folder is a copy of the standard output.

N.B.: If overlaps between segments occur (namely if the -b and -a options have large values and segments are very close), the rectangular values in detect.wav and split.wav add up.


