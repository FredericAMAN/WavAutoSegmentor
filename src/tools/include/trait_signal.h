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

/*******************************************************************************
 *
 * FONCTIONS DE TRAITEMENT DU SIGNAL
 *
 * origine : M. Vacher - 15 avril 2002
 *
 ******************************************************************************/

#include <stdio.h> 
#include <math.h> 
#include <float.h>
#include <stdlib.h> 

/******************************************
 *
 * 	STRUCTURES
 *
 *****************************************/

/*******************************************************************************
 * structure utilisée par les transformées en ondelettes par Numerical Recipies
 *	modif 28 février 2003 : utilisation de double au lieu de float
 ******************************************************************************/
typedef struct wavefilt
{
	int ncof ;	// nombre de coefficients du filtre de Daubechies
	int ioff ;	// centre du filtre
	int joff ;	// centre du filtre
	double *cc ;	// premier tableau des coefficients du filtre
	double *cr ;	// deuxième tableau des coefficients du filtre
} Wavefilt ; //wfilt;		// Defining declaration of a structure.

/******************************************
 *
 * 	FONCTIONS
 *
 *****************************************/

/*
 * Calcul de la moyenne d'un bloc d'échantillons
 * (tableau de doubles)
 *
 */

//double d_moy_bloc ( double signal[] , int n );

/*
 * Calcul de l'écart type d'un bloc d'échantillons
 * (tableau de doubles)
 *
 */

//double d_sigma_bloc ( double signal[] , double moy , int n );

/*
 * Calcul de l'énergie totale d'un bloc d'échantillons
 * (tableau de doubles)
 */

//double d_energie_bloc ( double *signal , int n ) ;

/*
 * Calcul de l'énergie totale d'un bloc d'échantillons
 * (tableau d'entiers)
 */

//double i_energie_bloc ( int *signal , int n ) ;

/*
 * Calcul de l'énergie totale d'un bloc d'échantillons
 * (tableau d'entiers type short)
 */

//double s_energie_bloc ( short *signal , int n ) ;

/*
 * Détermination du maximum et du minimum d'un tableau de taille n et
 * des indices correspondants
 *
 */

//int MaxMin1D ( double x[] , int n , double *max , int *imax , 
//					double *min , int *imin );

/*
 * Fonction de correlation entre 2 tableaux de n1 et n2 entiers
 * utilisant la formule directe par intégration.
 *
 * a[] : 1° tableau
 * n1  : taille utile du 1° tableau
 * 
 * b[] : 2° tableau
 * n2  : taille utile du 2° tableau (doit être identique à n1)
 *
 * c[] : tableau résultat
 * nc  : taille utile du tableau résultat
 *       (doit être >= ( 2 * n1 ) - 1
 *       
 */

//int Correlate ( double a[] , int n1 , double b[] , int n2 , double c[] , int nc );

/*
 * Fonction renvoi d'erreur utilisée par les Numerical Recipes
 *
 */

//void nrerror(char *error_text);

/*
 * Allocation mémoire d'un vecteur d'indice n1 à nh
 *
 * allocates a double vector with range [n1..nh].
 *
 */

//double *vector(int n1,int nh);

/*
 * Libération mémoire d'un vecteur d'indice n1 à nh
 *
 * frees a double vector with range [n1..nh] allocated by vector() .
 *
 */

//void *free_vector(double *v,int n1,int nh);

/*******************************************************************************
 *
 *	Interface à la fonction WT1 "Tranformation en Ondelettes Rapide" 
 *	ou "DWT"
 *	"Discrete double wt1" ou DDWT1
 *	
 * 	Origine : Michel Vacher - 27 février 2003
 *	modif 28 février 2003 : utilisation de double au lieu de float
 * 	
 *	BUT : 	utiliser un vecteur démarrant à l'indice 0 et non 1 comme
 *		classiquement dans "Numerical Recipies"	
 * 
 ******************************************************************************/

void Ddwt1(double a[], unsigned long n, int isign,
	void (*wtstep)(double [], unsigned long, int)) ;

/*******************************************************************************
 *
 *	Fonction WT1 "Tranformation en Ondelettes Rapide" ou "DWT"
 *
 * 	Origine : Numerical Recipies in C 
 *	modif 28 février 2003 : utilisation de double au lieu de float
 * 
 ******************************************************************************/

void wt1(double a[], unsigned long n, int isign,
	void (*wtstep)(double [], unsigned long, int)) ;
//
// One-dimensionnal discrete wavelet transform. This routine implements the 
// pyramid algorithm, replacing a[1..n] by its wavelet transform (for isign=1),
// or performing the inverse operation (for isign=-1). Note that n MUST be an
// integer power of 2. The routine wtstep, whose actual name must be supplied in
// calling this routine, is the underlying wavelet filter. Examples of wstep
// are daub4 and (preceded by pwtset) pwt.
//

/*******************************************************************************
 *
 *	Fonction DAUB4 "Filtre de Daubechies - à 4 coefficients (p=2)"
 *
 * 	l'ondelette phi à p=2 moments nuls si H(omega) a un zéro d'ordre p=2
 * 	em omega=pi . (voir Mallat p. 246)
 * 		
 * 		p=2 => n=4 coefficients
 * 	
 * 	Origine : Numerical Recipies in C 
 *	modif 28 février 2003 : utilisation de double au lieu de float
 * 
 ******************************************************************************/

// 
// C0 = ( 1+sqrt(3) ) / ( 4 sqrt(2) )
// #define C0 0.4829629131445341
// C1 = ( 3+sqrt(3) ) / ( 4 sqrt(2) )
// #define C1 0.8365163037378079
// C2 = ( 3-sqrt(3) ) / ( 4 sqrt(2) )
// #define C2 0.2241438680420134
// C3 = ( 1-sqrt(3) ) / ( 4 sqrt(2) )
// #define C3 -0.1294095225512604

//void daub4(double a[], unsigned long n, int isign) ;
//
// Applies the Daubechies 4-coefficient wavelet filter to data vector a[1..n]
// (for isign=1) or applies its transpose (for isign=-1). Used hierarchically by
// routines wt1 and wtn.
//

/*******************************************************************************
 *
 *	Fonction PWTSET "Initialisation d'un filtre de Daubechies avec n=4, 
 *	12 ou 20 coefficients"
 *
 * 	l'ondelette phi à p moments nuls si H(omega) a un zéro d'ordre p
 * 	em omega=pi . (voir Mallat p. 246)
 * 	
 * 		p=2 =>  n=4 coefficients
 * 		p=6 =>  n=12 coefficients
 *		p=10 => n=20 coefficients
 * 		
 * 	Origine : Numerical Recipies in C 
 *	modif 28 février 2003 : utilisation de double au lieu de float
 * 
 ******************************************************************************/

void pwtset(int n) ;
// Initializing routine for pwt, here implementing the Daubechies wavelet 
// filters with 4, 12, and 20 coefficients, as selected by the input value n.
// Further wavelet filters can be included in the obvious manner. This routine
// must be called (once) before the first use of pwt.
// (For the case n=4, the specific routine daub4 is considerably faster than 
// pwt.)
//

/*******************************************************************************
 *
 *	Fonction PWT "Filtre de Daubechies - 4, 12, 20 coefficient"
 *
 * 	Origine : Numerical Recipies in C 
 *	modif 28 février 2003 : utilisation de double au lieu de float
 * 
 ******************************************************************************/

void pwt(double a[], unsigned long n, int isign) ;
//void nrpwt(float a[], unsigned long n, int isign)
// 
// Partial wavelet transform : applies an arbitrary wavelet filter to data 
// vector a[1..n] (for isign=1) or applies its transpose (for isign=-1). Used
// hierachically by routines wt1 and wtn.
// The actual filter is determined by a preceding (and required) call to pwtset,
// which initializes the structure wfilt.
//

/*******************************************************************************
 *
 *	Fonction WTN "Tranformation en Ondelettes Rapide" ou "DWT"
 *	pour un signal de dimension n=ndim
 *
 * 	Origine : Numerical Recipies in C 
 *	modif 28 février 2003 : utilisation de double au lieu de float
 * 
 ******************************************************************************/

//void wtn(double a[], unsigned long nn[], int ndim, int isign,
//	void (*wtstep)(double [], unsigned long, int)) ;

/*******************************************************************************
 *
 * 	Fenêtre de Hamming sur un signal temporel
 * 	(utilisation prévue avant une FFT)
 *
 * 	Origine : 	remplacement fonction bibliothèque LabWindows/CVI
 * 			stage de Mihai Bobu mars-juin 2007
 * 	Inclusion dans la librairie et tests finaux :
 *			Michel Vacher - 11 juillet 2007
 *
 ******************************************************************************/

//int HamWin ( 	double y[], 		// tableau d'entrée à modifier	
//		int framewidth ) ;	// taille du tableau
		
/*******************************************************************************
 *
 * 	Auto Power Spectrum
 *
 * 	Origine : 	remplacement fonction bibliothèque LabWindows/CVI
 * 			stage de Mihai Bobu mars-juin 2007
 * 	Inclusion dans la librairie et tests finaux :
 *			Michel Vacher - 11 juillet 2007
 *
 * *****************************************************************************
 *
 * APS = ( FFT(X) . FFT*(X) ) / N**2
 *	X signal temporel
 *	N nombre de points du signal temporel
 *
 ******************************************************************************/

/*int AutoPowerSpectrum (	double data[],	// données temporelles
			int framewidth,	// taille du tableau
			double dt,		// écart entre temporel entre
						// 2 échantillons successifs
			double autopow[],	// sortie power spectrum
			double *df ) ;	// fréquence d'échantillonnage
			// dt et df ne sont pas utilisés dans le calcul !
*/
/*******************************************************************************
* 
* Function fftc1 modified from http://www.jjj.de/fft/ffteasyjj.c
*
* Do a Fourier transform of an array of N ecomplex numbers separated by steps of (ecomplex) size skip.
* The array f should be of length 2N*skip and N must be a power of 2.
* Forward determines whether to do a forward transform (1) or an inverse one(-1)
*
 ******************************************************************************/

void fftc1(double f[], int N, int forward);

