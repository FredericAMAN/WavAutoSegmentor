/*
     Copyright (C) 2016 Laboratoire d'Informatique de Grenoble (www.liglab.fr),
     Centre National de la Recherche Scientifique (www.cnrs.fr).
     The initial author of the original code is Frédéric Aman.
     Contributors: Michel Vacher
     
     This file is part of WavAutoSegmentor.

     WavAutoSegmentor is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     WavAutoSegmentor is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with WavAutoSegmentor. If not, see <http://www.gnu.org/licenses/>.

     WavAutoSegmentor is a software project initiated by the French Centre National 
     de la Recherche Scientifique (CNRS) and the Laboratoire d'Informatique de 
     Grenoble  (LIG) - France (https://www.liglab.fr/).
     WavAutoSegmentor is software to automatically cut WAVE files by detecting 
     audio activity in the signal.

     Contact Frédéric Aman <frederic.aman@gmail.com> for more information 
     about the licence or the use of WavAutoSegmentor.
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "main.h"
#include "split.h"


/**
 * @file wavsplit.c
 * @author Frederic Aman <frederic.aman@imag.fr> 
 */

void display_usage() {
	printf("Usage: WavAutoSegmentor [-i <str>] [-o <str>] [-s <double>] [-e <double>] [-b <double>] [-a <double>] [-p <double>]\n");
	printf("\t-i\t<default: data/in>\tDirectory of the input WAV files (mono channel)\n");
	printf("\t-o\t<default: data/out>\tDirectory of the output segmented WAV files\n");
	printf("\t-s\t<default: 0.0000005000>\tThreshold to start the segments detection\n");
	printf("\t-e\t<default: 0.0000005000>\tThreshold to end the segments detection\n");
	printf("\t-b\t<default: 0.000>\tDuration of added samples before the beginning of the detection (seconds)\n");
	printf("\t-a\t<default: 0.000>\tDuration of added samples after the end of the detection (seconds)\n");
	printf("\t-w\t<default: 0.250>\tMaximum duration of a silence in the signal, at which point detection stops (seconds)\n");
	printf("\t-v\t\t\t\tDisplay the version of WavAutoSegmentor\n");
	printf("\t-h\t\t\t\tThis help message\n");
}


int main(int argc, char **argv) {

	struct tm* ptm;
	struct timeval tv;
	char timestamp[200];
	char out_dir[200];
	char out_debug_dir[200];
	char out_d[200];
	char syst[1000];

	int index = 0;
	int opt = 0;

	opterr = 0;

	globalArgs.inputRep = malloc(2000*sizeof(char)); 
	globalArgs.outputRep = malloc(2000*sizeof(char)); 

	globalArgs.inputRep = "data/in";
	globalArgs.outputRep = "data/out";
	globalArgs.thresholdStart = 0.0000005;
	globalArgs.thresholdEnd = 0.0000005;
	globalArgs.timeBefore = 0;
	globalArgs.timeAfter = 0;
	globalArgs.timePending = 0.25;
	num_processed = 0;
	num_skipped = 0;
	num_total = 0;

	while ((opt = getopt (argc, argv, "i:o:s:e:b:a:w:hv")) != -1) {
		switch (opt) {
			case 'i':
				globalArgs.inputRep = optarg;
				break;
			case 'o':
				globalArgs.outputRep = optarg;
				break;
			case 's':
				globalArgs.thresholdStart = atof(optarg);
				break;
			case 'e':
				globalArgs.thresholdEnd = atof(optarg);
				break;
			case 'b':
				globalArgs.timeBefore = atof(optarg);
				break;
			case 'a':
				globalArgs.timeAfter = atof(optarg);
				break;
			case 'w':
				globalArgs.timePending = atof(optarg);
				break;
			case 'h':
				display_usage();
				return EXIT_SUCCESS;
			case 'v':
				printf ("WavAutoSegmentor v1.0\n");
				return EXIT_SUCCESS;
			case '?':
				if (optopt == 'i' || optopt == 'o' || optopt == 's' || optopt == 'e' || optopt == 'b' || optopt == 'a' || optopt == 'p') {
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				} else if (isprint (optopt)) {
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				} else {
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				}
				display_usage();
				return EXIT_SUCCESS;
			default:
				return EXIT_SUCCESS;
		}
	}

	for (index = optind; index < argc; index++) {
		printf ("Non-option argument %s\n", argv[index]);
	}

	strcpy(out_d, globalArgs.outputRep);
	gettimeofday (&tv, NULL);
	ptm = localtime(&tv.tv_sec);
	strftime(timestamp, 200*sizeof(char), "%d-%m-%Y_%H:%M:%S", ptm);
	sprintf(timestamp,"%s.%03ld", timestamp, tv.tv_usec/1000);
	sprintf(out_dir, "%s/%s", globalArgs.outputRep, timestamp);
	sprintf(out_debug_dir, "%s/debug", out_dir);
	globalArgs.outputRep = out_dir;

	mkdir(globalArgs.outputRep, S_IRUSR | S_IWUSR | S_IXUSR);
	mkdir(out_debug_dir, S_IRUSR | S_IWUSR | S_IXUSR);

	//strcpy(globalArgs.outputRep, out_dir);


	printf("WavAutoSegmentor v1.0\n");
	printf("\t-i Input directory:\t\t\t%s\t\t<default: data/in>\n\t-o Output directory:\t\t\t%s\t<default: data/out>\n\t-s Threshold start:\t\t\t%.10f\t<default: 0.0000005000>\n\t-e Threshold end:\t\t\t%.10f\t<default: 0.0000005000>\n\t-b Added duration before detection:\t%.3f\t\t<default: 0.000>\n\t-a Added duration after detection:\t%.3f\t\t<default: 0.000>\n\t-w Maximum duration of a silence:\t%.3f\t\t<default: 0.250>\n", globalArgs.inputRep, out_d, globalArgs.thresholdStart, globalArgs.thresholdEnd, globalArgs.timeBefore, globalArgs.timeAfter, globalArgs.timePending);

	sprintf(syst, "echo \"name|fs|bit rate|start index|end index|start time|end time|duration|SNR|data octets\" >> %s/debug/split.txt", globalArgs.outputRep);				
	system(syst);

	DIR * rep = opendir(globalArgs.inputRep);
	if (rep != NULL) {
		struct dirent * ent;
         
		while ((ent = readdir(rep)) != NULL) {
			char file[2000];
			sprintf(file, "%s/%s", globalArgs.inputRep, ent->d_name);
			struct stat *st = malloc(sizeof(struct stat));
			if(stat(file, st)==0){/* ignore if directory */
				if(S_ISDIR(st->st_mode)==0){
					split_init();
					split_process(file);
				} 
			}
		}
		closedir(rep);
	}
	printf("\n**********************************\nNumber of input files processed: %d/%d (%d/%d skipped)\n", num_processed, num_total, num_skipped, num_total);

	return EXIT_SUCCESS;
}


