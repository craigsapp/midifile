//
// Programmer:    Ziemowit Laski <zlaski@ziemas.net>
// Creation Date: Tue, 06 Feb 2018 15:01:47 -0800
// Filename:      midifile/include/Kernel.h
// Syntax:        C++11
//
// Description:   Convolution kernels for digital filters.

#ifndef _KERNEL_H_INCLUDED
#define _KERNEL_H_INCLUDED

#include <vector>

class Kernel : public std::vector<float> {
public:
	static constexpr double PI = 3.1415926535897932384626433832795;
	static constexpr double PIx2 = 6.283185307179586476925286766559;
	static constexpr double E = 2.71828182845904523536028747135266;

	Kernel(unsigned pSize = 8192);
	void resize(unsigned pSize);
	void sinc_blackman(float fractional_freq_low, float fractional_freq_high = 0.f);
	std::vector<float> fft_convolve(const std::vector<float> &x) const;
	std::vector<float> fft_envelope(const std::vector<float> &x) const;

};

#endif // #ifndef _KERNEL_H_INCLUDED
