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



#include "detection.h"

void init_detection(data_detection* G_Detection) {
	int j;
	G_Detection->studied_power[0] = 8;
	G_Detection->studied_power[1] = 9;
	G_Detection->studied_power[2] = 10;
	G_Detection->e_mean = 0;
	for (j = 0; j < 10; j++)
		G_Detection->ebuf[j] = 0;
	G_Detection->jener = 0;
	G_Detection->threshold_start = globalArgs.thresholdStart;
	G_Detection->threshold_end = globalArgs.thresholdEnd;
	G_Detection->tpause = -1;
	G_Detection->start_detection = 0;
	G_Detection->end_detection = 0;
	G_Detection->prev_start_detection = 0;
	G_Detection->prev_end_detection = 0;
	G_Detection->start_memo = 0;
	G_Detection->end_memo = 0;
	G_Detection->detection_state= -1;
	G_Detection->energy_ant = 0;
	G_Detection->nbrechenerg = 0;
	for (j = 0; j < 3; j++)
		G_Detection->nbrechenerg += (int) pow(2, G_Detection->studied_power[j] - DEPTH_TREE);
	for (j = 0; j < SIZE_MEMO_POWER; j++)
		G_Detection->power_buf[j] = 0;
	G_Detection->index_snr = 0;
	G_Detection->snr_power_ref = 0;
	G_Detection->snr_power_evt = 0;
	G_Detection->flag_create_wav = 0;
}

int detection(short int buffer[], int *frame_index, int samples_per_buffer, data_detection* G_Detection, int sample_rate) {
	int i = 0;
	int ip = 0;
	int lim_max = 0;
	int lim_min = 0;
	double energy_win = 0;
	double tmpenerg = 0;
	int ipuiss = 0; // indice de déplacement dans l'arbre
	int finished = 1;
	double signal_d[samples_per_buffer];
	//signal_d = malloc(samples_per_buffer * sizeof (double)); // entrée de la DWT
	double snr_power_buf = 0;
	double snr_power_ref = 0.00001;
	double tmax = 0;

	G_Detection->prev_end_detection = G_Detection->end_detection;
	if (signal_d == NULL) {
		fprintf(stderr, "Allocation Impossible <signal_d>");
		exit(EXIT_FAILURE);
	}
	// pour NDWT=2048 on considère 512*3 itérations si
	// coeffp=10 (règle de 3)

	// initialisation signal :
	for (i = 0; i < samples_per_buffer; i++) {
		signal_d[i] = ((double) buffer[i]/32768);
		snr_power_buf += signal_d[i] * signal_d[i];
	}
	// Initialisation de la DWT
	pwtset(12); // Daubechies à 12 coefficients,
	// fenêtre de 512 echantillons
	// Calcul de la DWT
	//transformée en ondelettes  
	Ddwt1(signal_d, samples_per_buffer, +1, pwt);

	finished = 1; // on continue l'enregistrement
	for (ip = 0; ip < (int) pow(2, DEPTH_TREE) && ip * 256 + 256 <= samples_per_buffer; ip++) {

		// boucle dans la fenêtre pour calculer l'énergie dans les 8 fenêtres glissantes puis faire
		// la détection, ip=numéro de la fenêtre glissante
		// for(ip=0;ip<8;ip++)		8=2**3
		energy_win = 0; // initialisation

		for (ipuiss = 0; ipuiss < NB_POWER; ipuiss++) {
			// balayage dans le profondeur de l'arbre
			lim_min = (int) pow(2, G_Detection->studied_power[ipuiss])
					  + ip * (int) pow(2, G_Detection->studied_power[ipuiss] - DEPTH_TREE);
			lim_max = (int) pow(2, G_Detection->studied_power[ipuiss])
					  +(ip + 1)*(int) pow(2, G_Detection->studied_power[ipuiss] - DEPTH_TREE) - 1;
			for (i = lim_min; i <= lim_max; i++)
				energy_win += signal_d[i] * signal_d[i];
				
		}

		tmpenerg = energy_win; // mémorisation
		energy_win += G_Detection->energy_ant; // somme avec fenêtre glissante précédente
		G_Detection->energy_ant = tmpenerg; // pour obtenir une fenêtre de 512 échantillons
		energy_win /= (double) G_Detection->nbrechenerg; // normalisation par la taille de la fenêtre

		tmax = globalArgs.timePending*pow(2, DEPTH_TREE)*sample_rate/samples_per_buffer+1;
		
		if (G_Detection->tpause != -1) { // si tpause!=-1, detection en cours
			if (energy_win > G_Detection->threshold_end) { // on reste en mode détection car signal>seuil
				G_Detection->detection_state = DETECTION_IN_PROGESS;
				G_Detection->tpause = 0; // On remet à 0 le tps de pause
				// On calcule ici l'instant où on a détecté la fin du signal,
				
				G_Detection->end_detection = *frame_index + ip * 256 + 256;
				// Traces
				for (i=0; i<256; i++){
					G_Detection->state_detection[*frame_index + ip * 256 + i] = 0.5 * 32768;
				}
			} else if (G_Detection->tpause >= tmax) { // on est sous le seuil, vérification de silence!
				// Traces
				G_Detection->detection_state = DETECTION_DONE;
				// durée supérieure au seuil fixé,
				// trop long pour être un silence, on sort de la
				// détection et on revient en mode normal!
				G_Detection->tpause = -1;

				// Calcul SNR
				// Puissance de référence ramenée à 2048 ech
				for (i = 0; i < SIZE_MEMO_POWER; i++){
					snr_power_ref += G_Detection->power_buf[i];
				}
				snr_power_ref /= SIZE_MEMO_POWER;
				G_Detection->snr_power_ref = snr_power_ref;

				// on arrête l'enregistrement
				finished = 0;
				//G_Detection->is_creating_wav[*frame_index + ip * 256] = 0.75 * 32768;
				G_Detection->flag_create_wav = 1;
			} else { // On incrémente le temps de pause...
				G_Detection->detection_state = DETECTION_PAUSE;
				G_Detection->tpause++;
				for (i=0; i<256; i++){
					G_Detection->state_detection[*frame_index + ip * 256 + i] = 0.25 * 32768;
				}
			}
		} else { // si tpause=-1, il faut voir si il y a détection
			// détection si on dépasse le seuil
			if (energy_win > globalArgs.thresholdStart + OV * G_Detection->e_mean) {

				// C'est le debut de la detection, Init EVT

				G_Detection->threshold_end = globalArgs.thresholdEnd + OV * G_Detection->e_mean;
				G_Detection->end_detection = *frame_index + ip * 256 + 256;
				G_Detection->detection_state = DETECTION_START;

				// Calcul instant de début
				G_Detection->prev_start_detection = G_Detection->start_detection;
				G_Detection->start_detection = *frame_index + ip * 256;
				G_Detection->tpause = 0;

				for (i=0; i<256; i++){
					G_Detection->state_detection[*frame_index + ip * 256 + i] = 0.5 * 32768;
				}
			}else {
				G_Detection->index_snr++;
				if (G_Detection->index_snr == SIZE_MEMO_POWER)
					G_Detection->index_snr = 0;
				G_Detection->power_buf[G_Detection->index_snr] = snr_power_buf;
				// on arrête l'enregistrement
				finished = 0; 
			}
		}
	
		// Gestion ebuf...
		// décalage du tableau existant
		if (G_Detection->jener == NB_MAX_MEAN) {
			for (i = 0; i < NB_MAX_MEAN - 1; i++) {
				G_Detection->ebuf[i] = G_Detection->ebuf[i + 1];
			}
			G_Detection->jener = NB_MAX_MEAN - 1; // On se remet sur l'avant derniere
		}
		// remplissage du buffer contenant les dernières énergies pour calcul
		// de la moyenne
		G_Detection->ebuf[G_Detection->jener] = energy_win;
		G_Detection->e_mean = 0;
		// calcul de la moyenne
		for (i = 0; i <= G_Detection->jener; i++) {
			G_Detection->e_mean += G_Detection->ebuf[i];
		}
		G_Detection->e_mean /= (G_Detection->jener + 1);
		G_Detection->jener++;
		
	}

	// Libération mémoire
	//if (signal_d) free(signal_d);

	return finished;
}


