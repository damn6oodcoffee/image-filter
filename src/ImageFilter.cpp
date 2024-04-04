#include "ImageFilter.hpp"
#include "FFT.hpp"
#include <cmath>
#include <algorithm>
#include <random>


ImageFilter::ImageFilter() {
    originalImg = std::make_unique<Image::RealGrayImageWx>();
    resizedImg = std::make_unique<Image::RealGrayImageWx>();
    noisyImg = std::make_unique<Image::RealGrayImageWx>();
    imgDFT = std::make_unique<Image::ComplexGrayImageWx>();
    imgDFTMasked = std::make_unique<Image::ComplexGrayImageWx>();
    processedImg = std::make_unique<Image::RealGrayImageWx>();
}

void ImageFilter::LoadFromFile(std::string path) {
    using namespace Image;
    originalImg->LoadFromFile(path);
    mat origMat{};
    originalImg->GetGrayImageMat(origMat);
    resizedImg->SetGrayImageMat(origMat);
    noisyImg->SetGrayImageMat(origMat);
    imgDFT->Reset();
    imgDFTMasked->Reset();
    processedImg->Reset();
    // TODO: notify mediator of "LoadFromFile" event
}

void ImageFilter::ResizeImage(int width, int height, ResizeMode mode) {
    using namespace Image;
    mat oldMat{};
    originalImg->GetGrayImageMat(oldMat);
    if (width <= 0 || height <= 0 || oldMat.size() == 0 || oldMat[0].size() == 0)
        return;
    mat resizedMat(height, std::vector<double>(width, 0.0));
    

    double dx { (static_cast<double>(oldMat[0].size()) - 1.0) / static_cast<double>(width) };
    double dy { (static_cast<double>(oldMat.size()) - 1.0) / static_cast<double>(height) };

    switch (mode) {
        case ResizeMode::zeroPadding: {
            for (int i = 0; i < width; i++) {
                for (int j = 0; j < height; j++) {
                    resizedMat[j][i] = ((j < oldMat.size()) && (i < oldMat[0].size())) ? oldMat[j][i] : 0;
                }
            }
            break;
        }
        case ResizeMode::bilinear: {
            int x{};
            int y{};
            double xDiff{};
            double yDiff{};

            double A{}, B{}, C{}, D{};
            int gray{};

            for (int i = 0; i < width; i++) {
                for (int j = 0; j < height; j++) {
                    x = int(i * dx);
                    y = int(j * dy);
                    xDiff = i * dx - x;
                    yDiff = j * dy - y;

                    A = oldMat[y][x];
                    B = oldMat[y][x + 1];
                    C = oldMat[y + 1][x + 1];
                    D = oldMat[y + 1][x];

                    gray = int(A * (1 - xDiff) * (1 - yDiff) + B * xDiff * (1 - yDiff) + D * yDiff * (1 - xDiff) + C * xDiff * yDiff);

                    resizedMat[j][i] = gray;
                }
            }
            break;
        }
    }

    //originalImg->SetGrayImageMat(resizedMat);
    resizedImg->SetGrayImageMat(resizedMat);
    noisyImg->SetGrayImageMat(resizedMat);
    imgDFT->Reset();
    imgDFTMasked->Reset();
    processedImg->Reset();
    // TODO: notify mediator of "ResizedImage" event
}

void ImageFilter::AddNoise(double percent) {
    using namespace Image;
    mat resizedMat{};
    resizedImg->GetGrayImageMat(resizedMat);
    if (resizedMat.size() == 0 || resizedMat[0].size() == 0)
        return;
    int height{ static_cast<int>(resizedMat.size()) };
    int width{ static_cast<int>(resizedMat[0].size()) };

    mat noiseMat(height, std::vector<double>(width, 0.0));
    mat noisedImgMat(height, std::vector<double>(width, 0.0));

    // Sample normal distribution
    std::random_device rd{};
    std::mt19937 gen{ rd() };
    std::normal_distribution<double> nd(0, 1);

    auto nd_sample{ [&nd, &gen] { return nd(gen); } };

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            noiseMat[i][j] = nd_sample();
        }
    }
   
    // Find scaling factor for the computed noise values.
    double signalEnergy{ 0 };
    double noiseEnergy{ 0 };
    double noiseScalingFactor{ 1 };

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            signalEnergy += resizedMat[i][j] * resizedMat[i][j];
            noiseEnergy += noiseMat[i][j] * noiseMat[i][j];
        }
    }

    noiseScalingFactor = sqrt((percent / 100.0) * signalEnergy / noiseEnergy);

    // Apply additive noise to the image.
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            noisedImgMat[i][j] = abs(resizedMat[i][j] + noiseScalingFactor * noiseMat[i][j]);
        }
    }

    noisyImg->SetGrayImageMat(noisedImgMat);
    imgDFT->Reset();
    imgDFTMasked->Reset();
    processedImg->Reset();
}

void ImageFilter::ComputeFourierTransform() {
    using namespace Image;
    matComplex dftMat{};
    mat noisyMat{};
    noisyImg->GetGrayImageMat(noisyMat);
    if (noisyMat.size() == 0 || noisyMat[0].size() == 0)
        return;

    dftMat = matComplex(noisyMat.size(), std::vector<std::complex<double>>(noisyMat[0].size(), { 0, 0 }));
    
    for (int i = 0; i < noisyMat.size(); i++) {
        for (int j = 0; j < noisyMat[0].size(); j++) {
            dftMat[i][j] = std::complex<double>(noisyMat[i][j], 0);
        }
    }

    FFT::fft2D(dftMat, 1);
    dftMat = fftShift(dftMat);
    imgDFT->SetGrayImageComplexMat(dftMat);
    imgDFTMasked->Reset();
    processedImg->Reset();
}

void ImageFilter::ApplyFilterMask(double maskSize, FilterPassMode pass) {
    using namespace Image;
    matComplex dftMat{};
    matComplex maskedMat{};
    imgDFT->GetGrayImageComplexMat(dftMat);
    if (dftMat.size() == 0 || dftMat[0].size() == 0)
        return;
    maskedMat = matComplex(dftMat.size(), std::vector<std::complex<double>>(dftMat[0].size(), { 0, 0 }));
    int height{ static_cast<int>(dftMat.size()) };
    int width{ static_cast<int>(dftMat[0].size()) };
    int size{ static_cast<int>(std::sqrt(static_cast<double>(height) * static_cast<double>(height) +
                                         static_cast<double>(width) * static_cast<double>(width)) * maskSize / 2.0) };
    mat mask{ generateMask(width, height, size, pass) };
    for (int i = 0; i < dftMat.size(); i++) 
        for (int j = 0; j < dftMat[0].size(); j++) 
            maskedMat[i][j] = dftMat[i][j] * mask[i][j];
    
    imgDFTMasked->SetGrayImageComplexMat(maskedMat);
}

void ImageFilter::ComputeInverseFourierTransform() {
    using namespace Image;
    matComplex dftMat{};
    matComplex idftMat{};
    imgDFTMasked->GetGrayImageComplexMat(dftMat);
 
    if (dftMat.size() == 0 || dftMat[0].size() == 0)
        return;

    dftMat = ifftShift(dftMat);
    idftMat = matComplex(dftMat.size(), std::vector<std::complex<double>>(dftMat[0].size(), { 0, 0 }));

    for (int i = 0; i < dftMat.size(); i++) {
        for (int j = 0; j < dftMat[0].size(); j++) {
            idftMat[i][j] = dftMat[i][j];
        }
    }

    FFT::fft2D(idftMat, -1);

    for (int i = 0; i < idftMat.size(); i++) {
        for (int j = 0; j < idftMat[0].size(); j++) {
            idftMat[i][j] /= static_cast<int>(idftMat.size()) * static_cast<int>(idftMat[0].size());
        }
    }
    
    mat idftMatReal(dftMat.size(), std::vector<double>(dftMat[0].size(), 0));
    for (int i = 0; i < idftMat.size(); i++) {
        for (int j = 0; j < idftMat[0].size(); j++) {
            idftMatReal[i][j] = std::max(0.0, idftMat[i][j].real());
        }
    }
    processedImg->SetGrayImageMat(idftMatReal);
}


template <typename T> T ImageFilter::fftShift(const T& matrix) {

    T shiftedMatrix;
    shiftedMatrix.resize(matrix.size());
    for (auto& row : shiftedMatrix) row.resize(matrix[0].size());

    int yhalf = matrix.size() / 2;
    int xhalf = matrix[0].size() / 2;
    int yodd = matrix.size() % 2;
    int xodd = matrix[0].size() % 2;

    for (int i = 0; i < xhalf + xodd; i++) 
        for (int j = 0; j < yhalf + yodd; j++) 
            shiftedMatrix[j][i] = matrix[j + yhalf][i + xhalf];
   
    for (int i = 0; i < xhalf; i++) 
        for (int j = 0; j < yhalf; j++) 
            shiftedMatrix[j + yhalf + yodd][i + xhalf + xodd] = matrix[j][i];
       
    for (int i = 0; i < xhalf; i++) 
        for (int j = 0; j < yhalf + yodd; j++) 
            shiftedMatrix[j][i + xhalf + xodd] = matrix[j + yhalf][i];
  
    for (int i = 0; i < xhalf + xodd; i++) 
        for (int j = 0; j < yhalf; j++) 
            shiftedMatrix[j + yhalf + yodd][i] = matrix[j][i + xhalf];
     
    //shiftedMatrix[yhalf + yodd][xhalf + xodd] = 0;
    return shiftedMatrix;
}

template <typename T> T ImageFilter::ifftShift(const T& matrix) {

    T shiftedMatrix;
    shiftedMatrix.resize(matrix.size());
    for (auto& row : shiftedMatrix) row.resize(matrix[0].size());

    int yhalf = matrix.size() / 2;
    int xhalf = matrix[0].size() / 2;
    int yodd = matrix.size() % 2;
    int xodd = matrix[0].size() % 2;

    for (int i = 0; i < xhalf; i++)
        for (int j = 0; j < yhalf; j++)
            shiftedMatrix[j][i] = matrix[j + yhalf + yodd][i + xhalf + xodd];

    for (int i = 0; i < xhalf + xodd; i++)
        for (int j = 0; j < yhalf + yodd; j++)
            shiftedMatrix[j + yhalf][i + xhalf] = matrix[j][i];

    for (int i = 0; i < xhalf + xodd; i++)
        for (int j = 0; j < yhalf; j++)
            shiftedMatrix[j][i + xhalf] = matrix[j + yhalf + yodd][i];

    for (int i = 0; i < xhalf; i++)
        for (int j = 0; j < yhalf + yodd; j++)
            shiftedMatrix[j + yhalf][i] = matrix[j][i + xhalf + xodd];

    //shiftedMatrix[yhalf + yodd][xhalf + xodd] = 0;
    return shiftedMatrix;
}


wxBitmap ImageFilter::NoisyImageBmp() {
    wxBitmap bmp(1,1);
    noisyImg->GetWxBitmap(bmp);
    return bmp;
}

wxBitmap ImageFilter::DFTImageBmp() {
    wxBitmap bmp(1, 1);
    imgDFT->GetWxBitmap(bmp);
    return bmp;
}

wxBitmap ImageFilter::LogDFTImageBmp() {
    using namespace Image;
    matComplex compMat{};
    imgDFT->GetGrayImageComplexMat(compMat);
    wxBitmap bmp(1, 1);
    if (compMat.empty() || compMat[0].empty())
        return bmp;
    mat logMat{ logify(compMat) };
    return toWxBitmap(logMat);
}

wxBitmap ImageFilter::MaskedDFTImageBmp() {
    wxBitmap bmp(1, 1);
    imgDFTMasked->GetWxBitmap(bmp);
    return bmp;
}

wxBitmap ImageFilter::LogMaskedDFTImageBmp() {
    using namespace Image;
    matComplex compMat{};
    imgDFTMasked->GetGrayImageComplexMat(compMat);
    wxBitmap bmp(1, 1);
    if (compMat.empty() || compMat[0].empty())
        return bmp;
    mat logMat{ logify(compMat) };
    return toWxBitmap(logMat);
}

wxBitmap ImageFilter::ProccessedImageBmp() {
    wxBitmap bmp(1, 1);
    processedImg->GetWxBitmap(bmp);
    return bmp;
}

Image::mat ImageFilter::logify(const Image::matComplex& mat) {
    Image::mat logMat{};
    logMat = Image::mat(mat.size(), std::vector<double>(mat[0].size(), 0));
    for (int i = 0; i < mat.size(); i++) {
        for (int j = 0; j < mat[0].size(); j++) {
            double val{ 1.0 + std::sqrt(mat[i][j].real() * mat[i][j].real() + mat[i][j].imag() * mat[i][j].imag()) };
            logMat[i][j] = std::log2(val);
        }
    }
    return logMat;
}

wxBitmap ImageFilter::toWxBitmap(const Image::mat& mat) {
    // Temporary image for bitmap conversion.
    std::unique_ptr<Image::IRealGrayImageWx> tempImage{ std::make_unique<Image::RealGrayImageWx>() };
    wxBitmap bmp(1, 1);
    tempImage->SetGrayImageMat(mat);
    tempImage->GetWxBitmap(bmp);
    return bmp;
}

Image::mat ImageFilter::generateMask(int width, int height, int maskSize, FilterPassMode pass) {
    Image::mat mask(height, std::vector<double>(width, 0.0));
    //double centerX{ static_cast<double>(width) / 2 };
    //double centerY{ static_cast<double>(height) / 2 };
    int centerX{ width / 2 };
    int centerY{ height / 2 };
    switch (pass) {
        case FilterPassMode::low: {
            for (int i{ 0 }; i < width; i++)
                for (int j{ 0 }; j < height; j++)
                    if (static_cast<double>(i - centerX) * static_cast<double>(i - centerX) +
                        static_cast<double>(j - centerY) * static_cast<double>(j - centerY) <
                        static_cast<double>(maskSize) * static_cast<double>(maskSize))
                        mask[j][i] = 1.0;
                    else
                        mask[j][i] = 0.0;
            break;
        }
        case FilterPassMode::high: {
            for (int i{ 0 }; i < width; i++)
                for (int j{ 0 }; j < height; j++)
                    if (static_cast<double>(i - centerX) * static_cast<double>(i - centerX) +
                        static_cast<double>(j - centerY) * static_cast<double>(j - centerY) <
                        static_cast<double>(maskSize) * static_cast<double>(maskSize))
                        mask[j][i] = 0.0;
                    else
                        mask[j][i] = 1.0;
            break;
        }
    }
    return mask;
}