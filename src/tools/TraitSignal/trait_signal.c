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

#include "trait_signal.h"

// Déjà déclarés dans trait_signal.h :
// #include <math.h>
// #include <values.h>
// #include <stdio.h>


/*
 * Fonction renvoi d'erreur utilisée par les Numerical Recipes
 *
 * 26 avril 2002
 *
 */

void nrerror(char *error_text)
{
	void exit();

	fprintf(stderr,"Numerical Recipes run-time error\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system..\n");
	exit(1);
}

/*
 * Allocation mémoire d'un vecteur d'indice n1 à nh
 *
 * allocates a double vector with range [n1..nh].
 *
 */

double *vector(int n1,int nh)
{
	double *v;

	v=(double*)malloc( (unsigned) (nh-n1+1)*sizeof(double) );
	if (!v ) nrerror("allocation failure in vector()");
	return (v-n1);
	
}

/*
 * Libération mémoire d'un vecteur d'indice n1 à nh
 *
 * frees a double vector with range [n1..nh] allocated by vector() .
 *
 */

void *free_vector(double *v,int n1,int nh)
{
	free( (char*) (v+n1) );
	return NULL;
}

/*******************************************************************************
 *
 *	Interface à la fonction WT1 "Tranformation en Ondelettes Rapide" 
 *	ou "DWT"
 *	"Discrete double wt1" ou DDWT1
 *	
 * 	Origine : Michel Vacher - 27 février 2003
 * 
 *	modif 28 février 2003 : utilisation de double au lieu de float
 * 	
 *	BUT : 	utiliser un vecteur démarrant à l'indice 0 et non 1 comme
 *		classiquement dans "Numerical Recipies"	
 * 
 ******************************************************************************/

void Ddwt1(double a[], unsigned long n, int isign,
	void (*wtstep)(double [], unsigned long, int))
{
	double *b;

	b = a - 1 ;
	
	wt1(b,n,isign,wtstep);
	return;

}
	
/*******************************************************************************
 *
 *	Fonction WT1 "Tranformation en Ondelettes Rapide" ou "DWT"
 *
 *	modif 28 février 2003 : utilisation de double au lieu de float
 * 	
 * 	Origine : Numerical Recipies in C 
 * 
 ******************************************************************************/

void wt1(double a[], unsigned long n, int isign,
	void (*wtstep)(double [], unsigned long, int))
//
// One-dimensionnal discrete wavelet transform. This routine implements the 
// pyramid algorithm, replacing a[1..n] by its wavelet transform (for isign=1),
// or performing the inverse operation (for isign=-1). Note that n MUST be an
// integer power of 2. The routine wtstep, whose actual name must be supplied in
// calling this routine, is the underlying wavelet filter. Examples of wstep
// are daub4 and (preceded by pwtset) pwt.
//
{
	unsigned long nn;

	if (n < 4) return;
	if (isign >= 0) {
		for (nn=n;nn>=4;nn>>=1) (*wtstep)(a,nn,isign);
		// Start at largest hierarchy, and work toward smallest.
	} else {
		for (nn=4;nn<=n;nn<<=1) (*wtstep)(a,nn,isign);
		// Start at smallest hierarchy, and work toward largest.
	}
}

/*******************************************************************************
 *
 *	Fonction PWTSET "Initialisation d'un filtre de Daubechies avec n=4, 
 *	12 ou 20 coefficients"
 *	
 *	l'ondelette phi à p moments nuls si H(omega) a un zéro d'ordre p
 * 	em omega=pi . (voir Mallat p. 246)
 * 	
 * 		p=2 =>  n=4 coefficients
 * 		p=6 =>  n=12 coefficients
 *		p=10 => n=20 coefficients
 *		
 *	modif 28 février 2003 : utilisation de double au lieu de float
 * 	
 * 	Origine : Numerical Recipies in C 
 * 
 ******************************************************************************/

/*
typedef struct {
	int ncof,ioff,joff;
	float *cc,*cr;
} wavefilt;

*/
Wavefilt wfilt;		// Defining declaration of a structure.

void pwtset(int n)
// Initializing routine for pwt, here implementing the Daubechies wavelet 
// filters with 4, 12, and 20 coefficients, as selected by the input value n.
// Further wavelet filters can be included in the obvious manner. This routine
// must be called (once) before the first use of pwt.
// (For the case n=4, the specific routine daub4 is considerably faster than 
// pwt.)
//
{
	// void nrerror(char error_text[]);
	int k;
	double sig = -1.0;
	static double c4[5]={0.0,0.4829629131445341,0.8365163037378079,
			0.2241438680420134,-0.1294095225512604};
	static double c12[13]={0.0,0.111540743350, 0.494623890398, 0.751133908021,
		0.315250351709,-0.226264693965,-0.129766867567,
		0.097501605587, 0.027522865530,-0.031582039318,
		0.000553842201, 0.004777257511,-0.001077301085};
	static double c20[21]={0.0,0.026670057901, 0.188176800078, 0.527201188932,
		0.688459039454, 0.281172343661,-0.249846424327,
		-0.195946274377, 0.127369340336, 0.093057364604,
		-0.071394147166,-0.029457536822, 0.033212674059,
		0.003606553567,-0.010733175483, 0.001395351747,
		0.001992405295,-0.000685856695,-0.000116466855,
		0.000093588670,-0.000013264203};
	static double c4r[5],c12r[13],c20r[21];

	wfilt.ncof=n;
	if (n == 4) {
		wfilt.cc=c4;
		wfilt.cr=c4r;
	}
	else if (n == 12) {
		wfilt.cc=c12;
		wfilt.cr=c12r;
	}
	else if (n == 20) {
		wfilt.cc=c20;
		wfilt.cr=c20r;
	}
	else nrerror("unimplemented value n in pwtset");
	for (k=1;k<=n;k++) {
		wfilt.cr[wfilt.ncof+1-k]=sig*wfilt.cc[k];
		sig = -sig;
	}
	wfilt.ioff = wfilt.joff = -(n >> 1);
	//
	// This values center the "support" of the wavelets at each level. 
	// Alternatively, the "peaks" of the wavelets can be approximately 
	// centered by the choice ioff=-2 and joff=-n+2.
	// Note that daub4 and pwtset with n=4 use different default centerings.
	// 
}

/*******************************************************************************
 *
 *	Fonction PWT "Filtre de Daubechies - 4, 12, 20 coefficient"
 *
 *	modif 28 février 2003 : utilisation de double au lieu de float
 * 	
 * 	Origine : Numerical Recipies in C 
 * 
 ******************************************************************************/

// #define NRANSI
// #include "nrutil.h"

/*
typedef struct {
	int ncof,ioff,joff;
	float *cc,*cr;
} wavefilt;

extern wavefilt wfilt;	Defined in pwtset
*/

void pwt(double a[], unsigned long n, int isign)
// 
// Partial wavelet transform : applies an arbitrary wavelet filter to data 
// vector a[1..n] (for isign=1) or applies its transpose (for isign=-1). Used
// hierachically by routines wt1 and wtn.
// The actual filter is determined by a preceding (and required) call to pwtset,
// which initializes the structure wfilt.
// 
{
	double ai,ai1,*wksp;
	unsigned long i,ii,j,jf,jr,k,n1,ni,nj,nh,nmod;

	if (n < 4) return;
	wksp=vector(1,n);
	nmod=wfilt.ncof*n;	// A positive constant equal to 0 mod n.
	n1=n-1;			// Mask all bits, since n is a power of 2.	
	nh=n >> 1;			
	for (j=1;j<=n;j++) wksp[j]=0.0;
	if (isign >= 0) {	// Apply filter.
		for (ii=1,i=1;i<=n;i+=2,ii++) {
			ni=i+nmod+wfilt.ioff; // Pointer to be incremented and
			nj=i+nmod+wfilt.joff; // wrapped-around.
			for (k=1;k<=wfilt.ncof;k++) {
				jf=n1 & (ni+k); // We use bitwise and to wrap-
				jr=n1 & (nj+k); // around the pointers.
				wksp[ii] += wfilt.cc[k]*a[jf+1];
				wksp[ii+nh] += wfilt.cr[k]*a[jr+1];
			}
		}
	} else {		// Apply tranpose filter.
		for (ii=1,i=1;i<=n;i+=2,ii++) {
			ai=a[ii];
			ai1=a[ii+nh];
			ni=i+nmod+wfilt.ioff; // See comments above.
			nj=i+nmod+wfilt.joff;
			for (k=1;k<=wfilt.ncof;k++) {
				jf=(n1 & (ni+k))+1;
				jr=(n1 & (nj+k))+1;
				wksp[jf] += wfilt.cc[k]*ai;
				wksp[jr] += wfilt.cr[k]*ai1;
			}
		}
	}
	for (j=1;j<=n;j++) a[j]=wksp[j];	// Copy the results back from
	free_vector(wksp,1,n);			// workspace.
}
// #undef NRANSI


/*******************************************************************************
 *
 * 	Auto Power Spectrum
 *
 * 	Origine : 	programme d'affichage du module du spectre (MV)
 *			adaptation pour power spectrum
 * 			but : remplacement fonction bibliothèque LabWindows/CVI
 * 			(stage de Mihai Bobu mars-juin 2007)
 * 			La fonction CVI affiche son résultat dans un tableau de
 * 			taille supérieure à N/2, les éléments au delà de N/2 ne
 * 			sont pas significatifs du fait des limitations dues à la
 * 			FFT, ils ont en général des valeurs proches de zéro mais
 * 			aussi des NaN...
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

int AutoPowerSpectrum (	double data[],	// données temporelles
			int framewidth,	// taille du tableau
			double dt,		// écart entre temporel entre
						// 2 échantillons successifs
			double autopow[],	// sortie power spectrum
			double *df ) 	// df=1/(n.dt)
					// cad la résolution spectrale
			// dt et df ne sont pas utilisés dans le calcul !
{
	// Variables
	//double *dspectre;
	double *spectre;
	double *module;
	double *hmodule;
	int i,di;
	// Allocation dynamique des tableaux
	// -> mémoire double pour FFT (partie réelle et imaginaire)
	spectre=(double*)malloc((framewidth*2)*sizeof(double));		
	// pour module spectre
	module=(double*)malloc(framewidth*sizeof(double));			
	// pour module spectre freq>=0
	hmodule=(double*)malloc((framewidth/2)*sizeof(double));		
	

	// initialisation du spectre
	i=0;
	while(i<framewidth*2) {
		spectre[i] = 0.0 ;
		i++;
	}

	//*df=frequence/framewidth;

	// La partie imaginaire du signal temporel est nulle
	//
	// Copie de data[] dans partie réelle de spectre[]
	// alternance réel-imag-réel-imag-...
	// (correction bug 2 septembre 2004)
	i=0;
	di=0;
	while(i<framewidth*2-2) 
	{
		spectre[i]=data[di++];
		// spectre[i]=sin((double)i++ /.5 +5.);
		i++;
		spectre[i]=0.0;
		i++;
	}

	// Calcul de la FFT , début à l'indice 1 imposé par Numerical Recipies
	//dspectre=spectre-1;
	//fftc1(dspectre,framewidth,+1);
	fftc1(spectre,framewidth,+1);

	// Calcul du module du spectre pour chaque fréquence
	// (positive ou négative)
	// Nous sommes dans le cas FFT(X) avec X fonction réelle :
	// donc qq soit n, C(-n) = C(n)*
	//			- partie réelle paire
	//			- partie imaginaire impaire
	i=0; di=0;
	while(i<framewidth/2) 
	{
		// partie réelle
		module[i]=spectre[di]*spectre[di];
		di++;
		// partie imaginaire
		module[i]=module[i]+spectre[di]*spectre[di];
		di++;i++;
	}

	// f0 = 0 : cas particulier de la fréquence nulle
	i=0;
	hmodule[i]=module[i];
	i++;
	// cas des autres fréquences
	while(i<framewidth/2)
	{
		hmodule[i]=2. * module[i];
		// multiplication par 2 à cause de la symétrie du spectre
		// sur les parties réelles et imaginaires
		// i <---> 512-i
		i++;
	}
	// Le power spectrum est défini sur framewidth/2 points seulement

	// Copie en sortie
	i=0;
	while(i<framewidth/2) 
	{
		// pour conformité avec la fonction AutoPowerSpectrum
		// de LabwindowsCVI
		autopow[i]=hmodule[i]/(framewidth*framewidth) ;
		i++;
	}
	// Power spectrum non défini au delà, on modifie pas le tableau car on
	// ne connait pas sa taille réelle.

	// Libération de la mémoire
	free(spectre);
	free(module);
	free(hmodule);
	return(0);
}

/*
* Function fftc1 extracted from http://www.jjj.de/fft/ffteasyjj.c
*/

/*
FFTEASY consists of the four C functions fftc1, fftcn, fftr1, and
fftrn. FFTEASY is free. I am not in any way, shape, or form expecting
to make money off of these routines. I wrote them because I needed
them for some work I was doing and I'm putting them out on the
Internet in case other people might find them useful. Feel free to
download them, incorporate them into your code, modify them, translate
the comment lines into Swahili, or whatever else you want. What I do
want is the following:
1) Leave this notice (i.e. this entire paragraph beginning with
``FFTEASY consists of...'' and ending with my email address) in with
the code wherever you put it. Even if you're just using it in-house in
your department, business, or wherever else I would like these credits
to remain with it. This is partly so that people can...
2) Give me feedback. Did FFTEASY work great for you and help your
work?  Did you hate it? Did you find a way to improve it, or translate
it into another programming language? Whatever the case might be, I
would love to hear about it. Please let me know at the email address
below.
3) Finally, insofar as I have the legal right to do so I forbid you
to make money off of this code without my consent. In other words if
you want to publish these functions in a book or bundle them into
commercial software or anything like that contact me about it
first. I'll probably say yes, but I would like to reserve that right.

For any comments or questions you can reach me at
gfelder@physics.stanford.edu.
*/


struct ecomplex
{
	double real;
	double imag;
};

/* 
Do a Fourier transform of an array of N ecomplex numbers separated by steps of (ecomplex) size skip.
The array f should be of length 2N*skip and N must be a power of 2.
Forward determines whether to do a forward transform (1) or an inverse one(-1)
*/
void fftc1(double f[], int N, int forward) {
	
	int skip = 1;
	int b, index1, index2, trans_size, trans; 
	double pi2 = 4.*asin(1.); 
	double pi2n, cospi2n, sinpi2n; /* Used in recursive formulas for Re(W^b) and Im(W^b) */
	struct ecomplex wb; /* wk = W^k = e^(2 pi i b/N) in the Danielson-Lanczos formula for a transform of length N */
	struct ecomplex temp1, temp2; /* Buffers for implementing recursive formulas */
	struct ecomplex *c = (struct ecomplex *)f; /* Treat f as an array of N ecomplex numbers */

	/* Place the elements of the array c in bit-reversed order */
	for(index1= 1, index2= 0; index1 < N; index1++) { /* Loop through all elements of c */
		for(b= N / 2; index2 >= b; b/= 2) /* To find the next bit reversed array index subtract leading 1's from index2 */
		index2-= b; 
		index2+= b; /* Next replace the first 0 in index2 with a 1 and this gives the correct next value */
		if(index2>index1) {/* Swap each pair only the first time it is found */
			temp1 = c[index2 * skip]; 
			c[index2 * skip] = c[index1 *skip]; 
			c[index1 * skip] = temp1; 
		}
	}

		/* Next perform successive transforms of length 2, 4, ..., N using the Danielson-Lanczos formula */
	for(trans_size= 2; trans_size <= N; trans_size*= 2) { /* trans_size = size of transform being computed */
		pi2n = forward * pi2 / (double)trans_size; /* +- 2 pi/trans_size */
		cospi2n = cos(pi2n); /* Used to calculate W^k in D-L formula */
		sinpi2n = sin(pi2n); 
		wb.real = 1.; /* Initialize W^b for b= 0 */
		wb.imag = 0.; 
		for(b= 0; b < trans_size / 2; b++) { /* Step over half of the elements in the transform */
			for(trans= 0; trans < N / trans_size; trans++) { /* Iterate over all transforms of size trans_size to be computed */
				index1= (trans * trans_size + b) * skip; /* Index of element in first half of transform being computed */
				index2= index1 + trans_size / 2 * skip; /* Index of element in second half of transform being computed */
				temp1= c[index1]; 
				temp2= c[index2]; 
				c[index1].real= temp1.real + wb.real * temp2.real - wb.imag * temp2.imag; /* Implement D-L formula */
				c[index1].imag= temp1.imag + wb.real * temp2.imag + wb.imag * temp2.real; 
				c[index2].real= temp1.real - wb.real * temp2.real + wb.imag * temp2.imag; 
				c[index2].imag= temp1.imag - wb.real * temp2.imag - wb.imag * temp2.real; 
			}
			temp1= wb; 
			wb.real= cospi2n * temp1.real - sinpi2n * temp1.imag; /* Real part of e^(2 pi i b/trans_size) used in D-L formula */
			wb.imag= cospi2n * temp1.imag + sinpi2n * temp1.real; /* Imaginary part of e^(2 pi i b/trans_size) used in D-L formula */
		}
	}

	/* For an inverse transform divide by the number of grid points */
	if(forward<0.)
	for(index1= 0; index1 < skip * N; index1+= skip)
	{
		c[index1].real/= N; 
		c[index1].imag/= N; 
	}
}

