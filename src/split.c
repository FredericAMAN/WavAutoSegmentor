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
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <libgen.h>

#include "main.h"
#include "split.h"
#include "detection.h"

data_recording *data_wavsplit;
data_detection *G_Detection = NULL;
int frame_index;
short int *wav_split;

void split_init() {
	
	frame_index = 0;

// Init G_Detection 
	G_Detection = malloc(sizeof(data_detection)); // declared in wavsplit.h
	data_wavsplit = malloc(sizeof(data_recording));
	//data_wavsplit->recordedFrames = malloc(sizeof(short int));
	if (G_Detection == NULL) {
		fprintf(stderr, "Allocation Impossible <G_Detection>\n");
		exit(EXIT_FAILURE);
	}
	init_detection(G_Detection);
}


int split_process(char *file) {
	char file_name[2000];
	strcpy(file_name, file);

	FILE * infile = fopen(file_name, "rb");	// Open wave file in read mode
	FILE * infile2 = fopen(file_name, "rb");

	FILE * outfile_detect = NULL;
	FILE * outfile_split_wav = NULL;

	int count = 0;						// For counting number of frames in wave file.
	short int buff16[BUFFSIZE];				// short int used for 16 bit as input data format is 16 bit PCM audio
	int buffsize = 0;							// variable storing number of byes returned
	int nb_split = 0;
	int i = 0;
	int finished = 1;
	char str[100];
	char str_outfile_detect[200];
	char str_split_wav[200];
	char str_basename[200];
	char str_dirname[200];
	char str_copy_wav[200];
	char syst[1000];
	double mean_d = 0;

	header_file header;
	header.bits_per_sample = 0;
	header.num_channels = 0;
	header.data_size = 0;
	header.sample_rate = 0;

	strcpy(str_basename, basename(file));
	strcpy(str_dirname, dirname(file));

	printf ("\n**********************************\nInput file:\n");

	num_total++;
	if (infile) {
		header.sample_rate = lire_entete_wav(infile2, &header.bits_per_sample, &header.num_channels, &header.data_size);
		if (header.sample_rate != -1) {
			if (header.num_channels > 1) {
				fprintf (stderr, "Warning: The WAV file is not mono! <%s> is skipped\n", file_name);
				num_skipped++;
			}else {
				lire_entete_wav(infile, &header.bits_per_sample, &header.num_channels, &header.data_size);
				printf("\tName:\t\t\t%s\n\tFormat:\t\t\tWAV\n\tChannels:\t\t%d\n\tSample rate (Hz):\t%d\n\tBits per sample:\t%d\n\tDuration (s):\t\t%d\n", file_name, header.num_channels, header.sample_rate, header.bits_per_sample, header.data_size/header.sample_rate);
				G_Detection->state_detection = malloc(sizeof(short int)*(header.data_size+BUFFSIZE));
				G_Detection->is_creating_wav = malloc(sizeof(short int)*(header.data_size+BUFFSIZE));
				G_Detection->tab_split_wav = malloc(sizeof(short int)*(header.data_size+BUFFSIZE));
				data_wavsplit->recordedFrames = malloc(sizeof(short int)*header.data_size+BUFFSIZE);
				fread(data_wavsplit->recordedFrames, sizeof(short int), header.data_size, infile2);
				for (i=0; i<(header.data_size+BUFFSIZE); i++) {
					G_Detection->state_detection[i] = 0;
					G_Detection->is_creating_wav[i] = 0;
					G_Detection->tab_split_wav[i] = 0;
				}
				init_detection(G_Detection);
		
				printf("\nStart splitting...\n");


				while (!feof(infile)) {
					buffsize = fread(buff16, sizeof(short int), BUFFSIZE, infile); // Reading data in chunks of BUFFSIZE
					count++; // Incrementing Number of frames
					finished = detection(buff16, &frame_index, buffsize, G_Detection, header.sample_rate);
					frame_index += buffsize;

					G_Detection->is_creating_wav[frame_index] = (short int)(1.0 * 32768);
					if (finished == 0 && G_Detection->flag_create_wav == 1) {
						nb_split++;
						G_Detection->is_creating_wav[frame_index] = (short int)(0.5 * 32768);
						G_Detection->flag_create_wav = 0;
						strtok(str_basename, ".");
						if (frame_index != G_Detection->end_detection && G_Detection->detection_state != DETECTION_PAUSE) {
							G_Detection->start_memo = G_Detection->start_detection-globalArgs.timeBefore*header.sample_rate;
							G_Detection->end_memo = G_Detection->end_detection+globalArgs.timeAfter*header.sample_rate;
						} else {
							G_Detection->start_memo = G_Detection->prev_start_detection-globalArgs.timeBefore*header.sample_rate;
							G_Detection->end_memo = G_Detection->prev_end_detection+globalArgs.timeAfter*header.sample_rate;
						} 
						if (G_Detection->start_memo < 0) G_Detection->start_memo = 0;
						if (G_Detection->end_memo > header.data_size) G_Detection->end_memo = header.data_size;
						mean_d+=(1.*G_Detection->end_memo-G_Detection->start_memo)/header.sample_rate;
						G_Detection->snr_power_evt = calcul_SNR_evt(data_wavsplit->recordedFrames, G_Detection->start_memo, G_Detection->end_memo, G_Detection->snr_power_ref, buffsize);
						sprintf(str, "%s/%s_%d.wav", globalArgs.outputRep, str_basename, nb_split);
						split_wav(G_Detection->start_memo, G_Detection->end_memo, str, header.bits_per_sample, header.num_channels, header.sample_rate);
						sprintf(syst, "echo \"%s|%d|%d|%d|%d|%f|%f|%f|%f|%d\" >> %s/debug/split.txt", str, header.sample_rate, header.bits_per_sample, G_Detection->start_memo, G_Detection->end_memo, G_Detection->start_memo/16000., G_Detection->end_memo/16000., (G_Detection->end_memo-G_Detection->start_memo)/16000., G_Detection->snr_power_evt, (G_Detection->end_memo-G_Detection->start_memo)*(header.bits_per_sample/8), globalArgs.outputRep);				
						system(syst);
					} 
				}
				if (G_Detection->detection_state != DETECTION_DONE) {
					count++;
					nb_split++;
					strtok(str_basename, ".");
					G_Detection->start_memo = G_Detection->start_detection-globalArgs.timeBefore*header.sample_rate;
					G_Detection->end_memo = G_Detection->end_detection+globalArgs.timeAfter*header.sample_rate;
					if (G_Detection->start_memo < 0) G_Detection->start_memo = 0;
					if (G_Detection->end_memo > header.data_size) G_Detection->end_memo = header.data_size;
					mean_d+=(1.*G_Detection->end_memo-G_Detection->start_memo)/header.sample_rate;
					G_Detection->snr_power_evt = calcul_SNR_evt(data_wavsplit->recordedFrames, G_Detection->start_memo, G_Detection->end_memo, G_Detection->snr_power_ref, buffsize);
					sprintf(str, "%s/%s_%d.wav", globalArgs.outputRep, str_basename, nb_split);
					split_wav(G_Detection->start_memo, G_Detection->end_memo, str, header.bits_per_sample, header.num_channels, header.sample_rate);
					sprintf(syst, "echo \"%s|%d|%d|%d|%d|%f|%f|%f|%f|%d\" >> %s/debug/split.txt", str, header.sample_rate, header.bits_per_sample, G_Detection->start_memo, G_Detection->end_memo, G_Detection->start_memo/16000., G_Detection->end_memo/16000., (G_Detection->end_memo-G_Detection->start_memo)/16000., G_Detection->snr_power_evt, (G_Detection->end_memo-G_Detection->start_memo)*(header.bits_per_sample/8), globalArgs.outputRep);			
					system(syst);

				}
				mean_d/=nb_split;
				sprintf(str_outfile_detect, "%s/debug/%s_detect.wav", globalArgs.outputRep, str_basename);
				sprintf(str_split_wav, "%s/debug/%s_split.wav", globalArgs.outputRep, str_basename);
				sprintf(str_copy_wav, "%s/debug/%s.wav", globalArgs.outputRep, str_basename);
				outfile_detect = fopen(str_outfile_detect,"wb");
				ecrire_entete_wav(outfile_detect, &header.sample_rate, &header.bits_per_sample, &header.num_channels, header.data_size);
				fwrite(G_Detection->state_detection, sizeof(short int), (header.data_size+buffsize), outfile_detect); // Writing read data into output file
				outfile_split_wav = fopen(str_split_wav,"wb");
				ecrire_entete_wav(outfile_split_wav, &header.sample_rate, &header.bits_per_sample, &header.num_channels, header.data_size);
				fwrite(G_Detection->tab_split_wav, sizeof(short int), (header.data_size+buffsize), outfile_split_wav); // Writing read data into output file
				copy_file(file_name, str_copy_wav);
				printf("...End splitting\n");
				printf("\nOutput files:\n\tNumber of segments:\t%d\n\tDirectory:\t\t%s\n\tFormat:\t\t\tWAV\n\tChannels:\t\t%d\n\tSample rate (Hz):\t%d\n\tBits per sample:\t%d\n\tMean duration (s):\t%.2f\n", nb_split, globalArgs.outputRep, header.num_channels, header.sample_rate, header.bits_per_sample, mean_d);

				fclose(outfile_detect);	
				fclose(infile);
				fclose(infile2);
				fclose(outfile_split_wav);
				free(data_wavsplit->recordedFrames);
				data_wavsplit->recordedFrames = NULL;
				free(data_wavsplit);
				data_wavsplit = NULL;
				free(G_Detection->state_detection);
				G_Detection->state_detection = NULL;
				free(G_Detection->is_creating_wav);
				G_Detection->is_creating_wav = NULL;
				free(G_Detection->tab_split_wav);
				G_Detection->tab_split_wav = NULL;
				free(G_Detection);
				G_Detection = NULL;
				num_processed++;
			}
		} else {
			fprintf (stderr, "Warning: The file is not a valid wav file! <%s> is skipped\n", file_name);
			num_skipped++;			
		}

	} else {
		fprintf (stderr, "Warning: Can not open the file! <%s> is skipped\n", file_name);
		num_skipped++;	
	}


	return 0;
}

void split_finish() {

}

int split_wav(int start, int end, char str[], short int bits, short int stereo, int sample_rate) {
	int i = 0;
	FILE * outfile_split;
	outfile_split = fopen(str,"wb");
	
	wav_split = malloc(sizeof(short int)*(end-start));
	for (i=0; i<(end-start); i++) {
		wav_split[i] = data_wavsplit->recordedFrames[start+i];
	}
	ecrire_entete_wav(outfile_split, &sample_rate, &bits, &stereo, end-start);
	fwrite(wav_split, sizeof(short int), end-start, outfile_split);
	free(wav_split);
	wav_split = NULL;
	for (i=start; i<=end; i++) {
		G_Detection->tab_split_wav[i] = G_Detection->tab_split_wav[i] + 0.2 * 32768;
	}
	return end-start;
}

int lire_entete_wav(FILE *f, short int *bits, short int *stereo, int *nr_ech) {
	char var[10];
	int t,bps,longueur;
	short int type,t1;
	int freq,i;
	int t2; // 26 mars 2004 : la valeur lue dans t ne doit pas être 
		// effacée
	int tfact; // taille data du fact
	char temp ; // 26 mars 2004 : buffer de 1 char
	int coeff;
	
	/* RIFF Type Chunk - Tronon RIFF */
	fread(var,sizeof(char),4,f);//On lit les 4 lettres de début "RIFF"
	if((var[0]!='R')||(var[1]!='I')||(var[2]!='F')||(var[3]!='F'))
	{
		var[4]=0;
		return(-1);
	}
	// chane ASCII : "RIFF" lue
	//On lit la longueur du fichier en octets -8 et on la mémorise
	fread(&longueur,sizeof(int),1,f);
	//on doit lire WAVE
	fread(var,sizeof(char),4,f);
	if((var[0]!='W')||(var[1]!='A')||(var[2]!='V')||(var[3]!='E'))
	{
		var[4]=0;
		return(-1);
	}
	// chaîne ASCII : "WAVE" lue
	// Fin du tronon RIFF
	
	// Format Chunk "fmt" - Tronon format
	fread(var,sizeof(char),4,f);//On lit fmt
	if((var[0]!='f')||(var[1]!='m')||(var[2]!='t')||(var[3]!=' '))
	{
		var[4]=0;
		return(-1);
	}
	// chane ASCII : "fmt " lue
	// On lit la dimension du tronon
	fread(&t,sizeof(int),1,f);
	//On lit le type de fichier : 1==non compressé
	fread(&type,sizeof(short int),1,f);
	if(type!=1) 
	{
		return(-1);
	}
	fread(stereo,sizeof(short int),1,f);//On lit le nombre de canal audio
	// 1 = mono, 2 = stereo
	fread(&freq,sizeof(int),1,f);//On lit fréquence d'échantillonnage
	// exemple = 16000
	fread(&bps,sizeof(int),1,f);//On lit le nombre d'octets par seconde pour temps réel
	fread(&t1,sizeof(short int),1,f);//le nombre d'octets à sortir en même temps
	fread(bits,sizeof(short int),1,f);//le nombre d'octets utilisés pour coder le signal
	// rajout 26 mars 2004 : lecture des extrabits
	// si t>16, il y a des bits supplémentaires dans le header (=extrabits)
	for(i=1; i<=t-16 ; i++) fread(&temp,sizeof(char),1,f);
	// fin rajout
	// Fin du tronon Format "fmt"
	
	
	// Suppression éventuelle du tronon "fact" - Fact Chunck
	fread(var,sizeof(char),4,f);
	if((var[0]=='f')&&(var[1]=='a')&&(var[2]=='c')&&(var[3]=='t'))
	{
		// lecture taille donnée "fact"
		fread(&tfact,sizeof(int),1,f);
		// lecture des données "fact"
		for(i=1; i<=tfact ; i++) 
			fread(&temp,sizeof(char),1,f);
		// préparation lecture de "data"
		fread(var,sizeof(char),4,f);
	}
	// Data Chunck "data" - Tronon data
	if((var[0]!='d')||(var[1]!='a')||(var[2]!='t')||(var[3]!='a'))
	{
		var[4]=0;
		return(-1);
	}
	fread(&t2,sizeof(int),1,f);//On lit la dim en octets des données audio
	 
	t2=t2/(*stereo);
	coeff = *bits / 8;
	t2=t2 / coeff;
	*nr_ech=t2;//On calcule le nombre d'échantillons pour chaque canal
	// exemple : si *bits=16, *stereo=1
	// 		alors *nr_ech = t2 / 2. = t2 % 2
	
    	//	MessagePopup ("TEST","Avez-vous lu le message d'erreur?");
	return(freq);

}

void ecrire_entete_wav(FILE *f,int *freq_ech,short int *bits,short int *stereo,int nr_ech){
	char var[10];
	int longueur,t,bps;
	short int type,t1;

	int istereo,ibits;

	istereo=(int)(*stereo);
	ibits=(int)(*bits);
	
	longueur= nr_ech * istereo * (ibits/8)+44-8;

	sprintf(var,"RIFF");
	fwrite(var,sizeof(char),4,f);//Il faut commencer par RIFF
	fwrite(&longueur,sizeof(int),1,f);//On écrit la longueur du fichier
	sprintf(var,"WAVE");
	fwrite(var,sizeof(char),4,f);//puis WAVE
	sprintf(var,"fmt ");
	fwrite(var,sizeof(char),4,f);// fmt
	t=16;
	fwrite(&t,sizeof(int),1,f);//longueur du header
	type=1;
	fwrite(&type,sizeof(short int),1,f);// type PCM
	fwrite(stereo,sizeof(short int),1,f);
	fwrite(freq_ech,sizeof(int),1,f);
	bps=(*freq_ech)*(*stereo)*((*bits)/8);//On calcule le nombre d'octets par secondes
	fwrite(&bps,sizeof(int),1,f);
	t1=((*bits)/8)*(*stereo);//on calcule le nombre d'octets à sortir en meme temps
	fwrite(&t1,sizeof(short int),1,f);
	fwrite(bits,sizeof(short int),1,f);
	sprintf(var,"data");
	fwrite(var,sizeof(char),4,f);
	t=nr_ech*(*stereo)*((*bits)/8);//la longueur en octets des données 
	fwrite(&t,sizeof(int),1,f);
}

int copy_file(char const * const source, char const * const destination) { 
    FILE* fSrc; 
    FILE* fDest; 
    char buffer[512]; 
    int NbLus; 
  
    if ((fSrc = fopen(source, "rb")) == NULL) 
    { 
        return 1; 
    } 
  
    if ((fDest = fopen(destination, "wb")) == NULL) 
    { 
        fclose(fSrc); 
        return 2; 
    } 
  
    while ((NbLus = fread(buffer, 1, 512, fSrc)) != 0) 
        fwrite(buffer, 1, NbLus, fDest); 
  
    fclose(fDest); 
    fclose(fSrc); 
  
    return 0; 
}

double calcul_SNR_evt(short int* signal, int start, int end, double snr_power_ref, int samples_per_buffer) {

	// Energie signal
	double ener_signal;	// énergie signal
	double power_signal;	// puissance signal
	double SNR;		// rapport signal/bruit
	int iech;		// échantillon courant

	//printf("%d %d %d\n", header.data_size, start, end);
	// Init
	iech = 0;
	ener_signal = 0;

	// Calcul énergie

	for (iech = start; iech < end; iech++)
	{
		ener_signal = ener_signal + (signal[iech] * signal[iech]); 
	}

	if ( ener_signal <= 0.0)
		return (0);

	// Normalisation de la puissance
	power_signal = ener_signal * samples_per_buffer / (end-start);
	SNR = power_signal / snr_power_ref;
	SNR = 10. * log(SNR) ; // ATTENTION :
	SNR = SNR / log(10.) ; // passage en log base 10

	return(SNR);
}

