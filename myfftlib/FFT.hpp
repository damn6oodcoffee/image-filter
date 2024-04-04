#ifndef FFT_HPP
#define FFT_HPP

#include <complex>
#include <vector>
#define M_PI           3.14159265358979323846 

namespace FFT {

	/**
	 * Slow DFT implementation for arbitrary data size.
	 * 
	 * @data In/out parameter. Should contain data points to transform. Out goes transformed data.
	 * @is Direction of transform. Should be -1/1 (forward/inverse).
	 */
	void SlowDFT(std::vector<std::complex<double>>& data, int is);

	/**
	 * FFT implementation for data size 2^x.
	 *
	 * @data In/out parameter. Should contain data points to transform. Out goes transformed data.
	 * @is Direction of transform. Should be -1/1 (forward/inverse).
	 */
	void fft(std::vector<std::complex<double>>& data, int is);

	/**
	 * 2D FFT implementation for data size 2^x.
	 *
	 * @data In/out parameter. Should contain data points to transform. Out goes transformed data.
	 * @is Direction of transform. Should be -1/1 (forward/inverse).
	 */
	void fft2D(std::vector<std::vector<std::complex<double>>>& data, int is);

	/**
	 * Compute spectrogram for given data.
	 *
	 * @data In parameter. Should contain data points to transform.
	 * @spectrogram Out parameter for resulting spectrogram.
	 * @windowSize DFT window size.
	 * @windowOver Window overlap.
	 */
	void ComputeSpectrogram(const std::vector<std::complex<double>>& data,
							std::vector<std::vector<double>>& spectrogram, 
							int windowSize, 
							int windowOverlap);
}

#endif