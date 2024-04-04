#include "FFT.hpp"

namespace FFT {

	void SlowDFT(std::vector<std::complex<double>>& data, int is) {
		int size{ static_cast<int>(data.size()) };
		auto dataBuf = data;
		for (int k{ 0 }; k < size; k++) {
			data[k] = 0;
			for (int n{ 0 }; n < size; n++) {
				data[k] += dataBuf[n] * exp(std::complex<double>(0, is * 2 * M_PI * k * n / size));
			}
		}
	}

	void fft(std::vector<std::complex<double>>& data, int is) {
		int size{ static_cast<int>(data.size()) };
		if (size == 1) return;
		if (size % 2 == 1) {
			SlowDFT(data, is);
			return;
		}

		std::vector<std::complex<double>> dataEven(size / 2);
		std::vector<std::complex<double>> dataOdd(size / 2);
		for (int i{ 0 }; i < size / 2; i++) {
			dataEven[i] = data[2 * i];
			dataOdd[i] = data[2 * i + 1];
		}

		fft(dataEven, is);
		fft(dataOdd, is);

		for (int k{ 0 }; k < size / 2; k++) {
			std::complex<double> oddTermExp = exp(std::complex<double>(0, is * 2 * M_PI * k / size));
			data[k] = dataEven[k] + oddTermExp * dataOdd[k];
			data[k + size / 2] = dataEven[k] - oddTermExp * dataOdd[k];
		}
	}

	void fft2D(std::vector<std::vector<std::complex<double>>>& data, int is) {
		int sizeDim1{ static_cast<int>(data.size()) };
		int sizeDim2{ static_cast<int>(data[0].size()) };
		std::vector<std::complex<double>> dataRow;
		std::vector<std::complex<double>> dataCol;

		for (int i{ 0 }; i < sizeDim1; i++) {
			dataRow.clear();
			dataRow = data[i];
			fft(dataRow, is);
			data[i] = dataRow;
		}

		for (int j{ 0 }; j < sizeDim2; j++) {
			dataCol.resize(sizeDim1);
			for (int k{ 0 }; k < sizeDim1; k++) {
				dataCol[k] = data[k][j];
			}
			fft(dataCol, is);

			for (int k{ 0 }; k < sizeDim1; k++) {
				data[k][j] = dataCol[k];
			}
		}
	}

	void ComputeSpectrogram(const std::vector<std::complex<double>>& data, std::vector<std::vector<double>>& spectrogram, int windowSize, int windowOverlap) {
		if (windowSize <= windowOverlap)
			return;
		std::vector<std::complex<double>> window(windowSize, { 0,0 });
		int size{ static_cast<int>(data.size()) };
		int windowStartPos{ 0 };
		while (windowStartPos < size) {
			for (int i{ 0 }; i < windowSize; i++) {
				if (windowStartPos + i < size)
					window[i] = data[windowStartPos + i];
				else
					window[i] = { 0,0 };
			}
			fft(window, -1);
			spectrogram.push_back(std::vector<double>(0));
			for (auto& val : window)
				spectrogram.back().push_back(sqrt(val.real() * val.real() + val.imag() * val.imag()));
			windowStartPos += windowSize - windowOverlap;
		}

	}

}