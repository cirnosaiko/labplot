/***************************************************************************
    File                 : nsl_conv.c
    Project              : LabPlot
    Description          : NSL discrete convolution functions
    --------------------------------------------------------------------
    Copyright            : (C) 2018 by Stefan Gerlach (stefan.gerlach@uni.kn)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#include "nsl_conv.h"
#include "nsl_common.h"
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include "backend/nsl/nsl_stats.h"

const char* nsl_conv_direction_name[] = {i18n("forward (convolution)"), i18n("backward (deconvolution)")};
const char* nsl_conv_method_name[] = {i18n("auto"), i18n("direct"), i18n("FFT")};
const char* nsl_conv_type_name[] = {i18n("linear (zero-padded)"), i18n("circular")};

int nsl_conv_convolution_direction(double s[], size_t n, double r[], size_t m, nsl_conv_direction_type dir, nsl_conv_method_type method, nsl_conv_type_type type,
		int normalize, int wrap, double out[]) {
	if (dir == nsl_conv_direction_forward)
		return nsl_conv_convolution(s, n, r, m, method, type, normalize, wrap, out);
	else
		return nsl_conv_deconvolution(s, n, r, m, type, normalize, wrap, out);
}

int nsl_conv_convolution(double s[], size_t n, double r[], size_t m, nsl_conv_method_type method, nsl_conv_type_type type, int normalize, int wrap, double out[]) {
	if (method == nsl_conv_method_direct || (method == nsl_conv_method_auto && GSL_MAX_INT(n,m) <= NSL_CONV_METHOD_BORDER)) {
		if (type == nsl_conv_type_linear)
			return nsl_conv_linear_direct(s, n, r, m, normalize, wrap, out);
		else if (type == nsl_conv_type_circular)
			return nsl_conv_circular_direct(s, n, r, m, normalize, wrap, out);
	} else {
		return nsl_conv_fft_type(s, n, r, m, nsl_conv_direction_forward, type, out);
	}

	return 0;
}

int nsl_conv_deconvolution(double s[], size_t n, double r[], size_t m, nsl_conv_type_type type, int normalize, int wrap, double out[]) {
	/* only supported by FFT method */
	return nsl_conv_fft_type(s, n, r, m, nsl_conv_direction_backward, type, out);
}

int nsl_conv_linear_direct(double s[], size_t n, double r[], size_t m, int normalize, int wrap, double out[]) {
	size_t i, j, wi = 0;
	double norm = 1;
	if (normalize)
		norm = cblas_dnrm2(m, r, 1);
	if (wrap)
		nsl_stats_maximum(r, m, &wi);

	for (j = 0; j < n + m - 1; j++) {
		int index;
		double res = 0;
		for (i = 0; i < n; i++) {
			index = j - i;
			if (index >= 0 && index < (int)m)
				res += s[i] * r[index]/norm;
		}
		index = j - wi;
		if (index < 0)
			index += n+m-1;
		out[index] = res;
	}

	return 0;
}

int nsl_conv_circular_direct(double s[], size_t n, double r[], size_t m, int normalize, int wrap, double out[]) {
	size_t i, j, size = GSL_MAX(n,m), wi = 0;
	double norm = 1;
	if (normalize)
		norm = cblas_dnrm2(m, r, 1);
	if (wrap)
		nsl_stats_maximum(r, m, &wi);

	for (j = 0; j < size; j++) {
		int index;
		double res = 0;
		for (i = 0; i < n; i++) {
			index = j-i;
			if (index < 0)
				index += size;
			if (index < (int)m)
				res += s[i]*r[index]/norm;
		}
		index = j - wi;
		if (index < 0)
			index += size;
		out[index] = res;
	}

	return 0;
}

int nsl_conv_fft_type(double s[], size_t n, double r[], size_t m, nsl_conv_direction_type dir, nsl_conv_type_type type, double out[]) {
	size_t i, size;
	if (type == nsl_conv_type_linear)
		size = n + m - 1;
	else	// circular
		size = GSL_MAX(n, m);

	// zero-padded arrays
	double *stmp = (double*)malloc(size*sizeof(double));
	double *rtmp = (double*)malloc(size*sizeof(double));
	if (stmp == NULL || rtmp == NULL) {
		puts("ERROR: zero-padding data");
		return -1;
	}
	for (i = 0; i < n; i++)
		stmp[i] = s[i];
	for (i = n; i < size; i++)
		stmp[i] = 0;
	for (i = 0; i < m; i++)
		rtmp[i] = r[i];
	for (i = m; i < size; i++)
		rtmp[i] = 0;

	int status = nsl_conv_fft(stmp, rtmp, size, dir, out);
	free(stmp);
	free(rtmp);

	return status;
}

/* TODO: implement
int nsl_conv_fft_FFTW(double s[], double r[], size_t n, nsl_conv_direction_type dir, double out[]) {
} */

int nsl_conv_fft_GSL(double s[], double r[], size_t n, nsl_conv_direction_type dir, double out[]) {
	/* TODO */
}

int nsl_conv_fft(double s[], double r[], size_t n, nsl_conv_direction_type dir, double out[]) {
	/* TODO: enable if implemented */
/* #ifdef HAVE_FFTW3
	return nsl_conv_fft_FFTW(s, r, n, dir, out);
#else */
	return nsl_conv_fft_GSL(s, r, n, dir, out);
/* #endif */
}

/* adapted from SciDAVis */
/* TODO: implement own mixed-radix version using GSL and FFTW */
int nsl_conv_linear_fft_old(double s[], size_t n, double r[], size_t m, nsl_conv_direction_type dir) {

	double* res = (double*)malloc(n * sizeof(double));
	if (res == NULL) {
		printf("nsl_conv_convolution(): ERROR allocating memory!\n");
		return -1;
	}
	memset(res, 0, n * sizeof(double));

	size_t i, m2 = m/2;
	for (i = 0; i < m2; i++) {	// store the response in wrap around order (see Numerical Recipes)
		res[i] = r[m2+i];
		res[n-m2+i] = r[i];
	}

	if (m2%2 == 1)
		res[m2] = r[m-1];

	/* FFT both */
	/* TODO: use mixed radix versions */
	gsl_fft_real_radix2_transform(res, 1, n);
	gsl_fft_real_radix2_transform(s, 1, n);

	double re, im, size;
	for (i = 0; i < n/2; i++) {	/* multiply/divide FFTs */
		if (i == 0 || i == n/2 - 1) {
			if (dir == nsl_conv_direction_forward)
				s[i] = res[i]*s[i];
			else {
				if (res[i] > 0)
					s[i] = s[i]/res[i];
				else
					s[i] = 0.;
			}
		} else {
			if (dir == nsl_conv_direction_forward) {
				re = res[i]*s[i] - res[n-i]*s[n-i];
				im = res[i]*s[n-i] + res[n-i]*s[i];
			} else {
				size = res[i]*res[i] + res[n-i]*res[n-i];
				re = res[i]*s[i] + res[n-i]*s[n-i];
				im = res[i]*s[n-i] - res[n-i]*s[i];
				if (size > 0) {
					re /= size;
					im /= size;
				}
			}

			s[i] = re;
			s[n-i] = im;
		}
	}
	free(res);

	/* inverse FFT */
	/* TODO: use mixed radix version */
	gsl_fft_halfcomplex_radix2_inverse(s, 1, n);

	return 0;
}