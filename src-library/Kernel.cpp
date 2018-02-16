// Programmer:    Ziemowit Laski <zlaski@ziemas.net>
// Creation Date: Tue, 06 Feb 2018 15:01:47 -0800
// Filename:      midifile/include/Kernel.cpp
// Syntax:        C++11
//
// Description:   Convolution kernels for digital filters.

// Based in part on "The Scientist and Engineer's Guide to
// Digital Signal Processing" By Steven W.Smith, Ph.D.
// retrieved from http://www.dspguide.com/

#include "Kernel.h"

#include <stdio.h>

#include <algorithm>

static void dc_unity_gain(int N, float *data) {
	float sum = 0.0f;
	for (int i = 0; i < N; ++i) {
		sum += data[i];
	}
	for (int i = 0; i < N; ++i) {
		data[i] /= sum;
	}
}

static void invert(int N, float *data) {
	int S = (N >> 1) - 1;
	for (int s = 0; s < S; ++s) {
		data[s] = -data[s];
	}
	data[S >> 1] += 1.f;  // add delta function
}

static void add(int N, float *dst, const float *src) {
	for (int i = 0; i < N; ++i) {
		dst[i] += src[i];
	}
}

static void mul(int N, float *dst, const float *src) {
	int H = N >> 1;  // N is even
	float *REX = dst, *IMX = dst + H;
	const float *REFR = src, *IMFR = src + H;
	for (int F = 1; F < H; ++F) {
		float TEMP = REX[F] * REFR[F] - IMX[F] * IMFR[F];
		IMX[F] = REX[F] * IMFR[F] + IMX[F] * REFR[F];
		REX[F] = TEMP;
	}
	REX[0] *= REFR[0];
	REX[H] *= REFR[H];
}

static void sinc_blackman_1(int N, float *data, float fractional_freq) {
	static const float a0 = 7938.f / 18608.f, a1 = -9240.f / 18608.f, a2 = 1430.f / 18608.f;
	int S = (N >> 1) - 1;
	double pf = Kernel::PIx2 * fractional_freq;
	int F = S >> 1;
	double PIx2N = Kernel::PIx2 / (S - 1);
	for (int i = 0; i < S; ++i) {
		int f = i - F;
		double sinc = (f ? sin(pf * f) / f : pf);
		double n = PIx2N * i;
		double blackman = a0 + a1 * cos(n) + a2 * cos(2.f * n);
		data[i] = (float)(sinc * blackman);
	}
	dc_unity_gain(N, data);
}

static void compute_complex_fft(int N, float *REX, float *IMX) {
	int NM1 = N - 1;
	int ND2 = N / 2;
	int M = (int)log2(N);
	int J = ND2;

	for (int I = 1; I <= N - 2; ++I) {
		if (I < J) {
			float TR = REX[J];
			float TI = IMX[J];
			REX[J] = REX[I];
			IMX[J] = IMX[I];
			REX[I] = TR;
			IMX[I] = TI;
		}
		int K = ND2;
		while (K <= J) {
			J = J - K;
			K = K / 2;
		}
		J = J + K;
	}

	for (int L = 1; L <= M; ++L) {
		int LE = (int)exp2(L);
		int	LE2 = LE / 2;
		float UR = 1.f;
		float UI = 0.f;
		float SR = (float)cos(Kernel::PIx2 / LE);
		float SI = -(float)sin(Kernel::PIx2 / LE);
		for (int J = 1; J <= LE2; ++J) {
			int JM1 = J - 1;
			for (int I = JM1; I <= NM1; I += LE) {
				int IP = I + LE2;
				float TR = REX[IP] * UR - IMX[IP] * UI;
				float TI = REX[IP] * UI + IMX[IP] * UR;
				REX[IP] = REX[I] - TR;
				IMX[IP] = IMX[I] - TI;
				REX[I] = REX[I] + TR;
				IMX[I] = IMX[I] + TI;
			}
			float TR = UR;
			UR = TR * SR - UI * SI;
			UI = TR * SI + UI * SR;
		}
	}
}

static void fft(int N, float *data, float *imag = nullptr) {
	float *REX = data, *IMX;
	std::vector<float> imx;
	if (imag) {
		IMX = imag;
	}
	else {
		imx.resize(N);
		IMX = imx.data();
	}

	int NH = N / 2 - 1;
	for (int I = 0; I <= NH; ++I) {
		int I2 = I << 1;
		REX[I] = REX[I2];
		IMX[I] = REX[I2 + 1];
	}
	compute_complex_fft(N / 2, REX, IMX);
	int NM1 = N - 1;
	int ND2 = N / 2;
	int N4 = N / 4 - 1;
	for (int I = 1; I <= N4; ++I) {
		int IM = ND2 - I;
		int IP2 = I + ND2;
		int IPM = IM + ND2;
		REX[IP2] = (IMX[I] + IMX[IM]) / 2;
		REX[IPM] = REX[IP2];
		IMX[IP2] = -(REX[I] - REX[IM]) / 2;
		IMX[IPM] = -IMX[IP2];
		REX[I] = (REX[I] + REX[IM]) / 2;
		REX[IM] = REX[I];
		IMX[I] = (IMX[I] - IMX[IM]) / 2;
		IMX[IM] = -IMX[I];
	}
	REX[N * 3 / 4] = IMX[N / 4];
	REX[ND2] = IMX[0];
	IMX[N * 3 / 4] = 0;
	IMX[ND2] = 0;
	IMX[N / 4] = 0;
	IMX[0] = 0;
	int L = (int)log2(N);
	int LE = (int)exp2(L);
	int LE2 = LE / 2;
	float UR = 1.f;
	float UI = 0.f;
	float SR = (float)cos(Kernel::PIx2 / LE);
	float SI = -(float)sin(Kernel::PIx2 / LE);
	for (int J = 1; J <= LE2; ++J) {
		int JM1 = J - 1;
		for (int I = JM1; I <= NM1; I += LE) {
			int IP = I + LE2;
			float TR = REX[IP] * UR - IMX[IP] * UI;
			float TI = REX[IP] * UI + IMX[IP] * UR;
			REX[IP] = REX[I] - TR;
			IMX[IP] = IMX[I] - TI;
			REX[I] = REX[I] + TR;
			IMX[I] = IMX[I] + TI;
		}
		float TR = UR;
		UR = TR * SR - UI * SI;
		UI = TR * SI + UI * SR;
	}

	if (!imag) {
		std::memcpy(REX + (N >> 1) + 1, IMX + 1, ((N >> 1) - 1) * sizeof(REX[0]));
	}
}

static void ifft(int N, float *data) {
	int H = N >> 1;  // N is even
	std::vector<float> imx(N);
	float *REX = data, *IMX = imx.data();
	std::memcpy(IMX + 1, REX + H + 1, (H - 1) * sizeof(IMX[0]));
	for (int K = H + 1; K < N; ++K) {
		// NEGATIVE FREQUENCY GENERATION 
		REX[K] = REX[N - K];
		IMX[K] = -IMX[N - K];
	}
	for (int K = 0; K < N; ++K) {
		REX[K] += IMX[K];
	}
	fft(N, REX, IMX);  // forward FFT

	for (int I = 0; I < N; ++I) {
		REX[I] = (REX[I] + IMX[I]) / N;
	}
}

static std::vector<float> fft_convolve_1(int N, const float *data, const std::vector<float> &x) {
	const int X = x.size();   // length of signal vector
	const int H = (N >> 1) - 1;  // length of actual filter in 'data'
	const int L = N - H + 1;  // input segment size per iteration (since N = H + L - 1)
	const int S = X / L;      // number of whole segments in input
	const int Y0 = X + N - 1;  // initial (padded) length of output vector
	const int Y = X + (N >> 1) - 2;  // final (truncated) length of output vector
	const int O = N - L;      // size of overlap window

	std::vector<float> D(data, data + N), o(O), y(Y0), y_img(N);
	float *filt = D.data();
	fft(N, filt);

	int Yo = 0;
	const float *x0 = x.data();
	float *y0 = y.data(), *o0 = o.data();
	for (int s = 0; s <= S && Yo <= X; ++s, Yo += L, x0 += L, y0 += L) {
		// copy in next segment (will have zeroes after it)
		int Lo = std::min(L, X - Yo);
		std::memcpy(y0, x0, Lo * sizeof(y[0]));
		if (Lo < L) {
			// last (fractional) segment
			std::memset(y0 + Lo, 0, (L - Lo) * sizeof(y0[0]));
		}
		fft(N, y0);
		mul(N, y0, filt);
		ifft(N, y0);

		// add in previous overlap
		for (int Oo = 0; Oo < O; ++Oo) {
			y0[Oo] += o0[Oo];
		}
		// save overlap for next iteration
		std::memcpy(o0, y0 + L, O * sizeof(o0[0]));
	}

	y.resize(Y);
	return y;
}

static void sinc_blackman_2(int N, float *data, float fractional_freq_low, float fractional_freq_high) {
	sinc_blackman_1(N, data, fractional_freq_low);  // low-pass

	if (fractional_freq_high > fractional_freq_low) {
		std::vector<float> B(N);
		sinc_blackman_1(N, B.data(), fractional_freq_high); // low-pass
		invert(N, B.data()); // high-pass
		add(N, data, B.data());  // band-reject
		invert(N, data);  // band-pass
	}
}

static std::vector<float> fft_envelope_1(int N, const float *data, const std::vector<float> &x) {
	std::vector<float> TMP = x;
	const int X = x.size();

	for (int i = 0; i < X; ++i) {
		float a = abs(TMP[i]);
		TMP[i] = a;  // flip negative values
	}

	return fft_convolve_1(N, data, TMP);
}

Kernel::Kernel(unsigned pSize) {
	resize(pSize);
}

void Kernel::resize(unsigned pSize) {
	if (pSize >= 4 && ((pSize ^ (pSize - 1)) == (pSize << 1) - 1)) {
		std::vector<float>::resize(pSize);
	}
	else {
		fprintf(stderr, "Kernel size must be a power of 2");
		exit(1);
	}
}

void Kernel::sinc_blackman(float fractional_freq_low, float fractional_freq_high) {
	// low-pass or band-pass
	sinc_blackman_2((int)size(), data(), fractional_freq_low, fractional_freq_high);
}

std::vector<float> Kernel::fft_convolve(const std::vector<float> &x) const { 
	return fft_convolve_1(size(), data(), x);
}

std::vector<float> Kernel::fft_envelope(const std::vector<float> &x) const {
	return fft_envelope_1(size(), data(), x);
}

