#pragma once
#include "ImageWx.hpp"
#include <memory>

class ImageFilter {
public:
    enum class FilterPassMode {
        low,
        high,
    };

    enum class ResizeMode {
        zeroPadding,
        bilinear,
    };

    

    ImageFilter();
    ~ImageFilter() {};
    void LoadFromFile(std::string path);
    void ResizeImage(int width, int height, ResizeMode mode);
    void AddNoise(double percent);
    void ComputeFourierTransform();
    
    /**
    * Apply 'pass' filter mask of size 'maskSize' = [0.0,..,1.0]
    */
    void ApplyFilterMask(double maskSize, FilterPassMode pass);
    void ComputeInverseFourierTransform();
    void SaveAsFile(std::string path) {/*Not Implemented*/ };
    
  
    wxBitmap NoisyImageBmp();
    wxBitmap DFTImageBmp();
    wxBitmap LogDFTImageBmp();
    wxBitmap MaskedDFTImageBmp();
    wxBitmap LogMaskedDFTImageBmp();
    wxBitmap ProccessedImageBmp();
    

private:
    std::unique_ptr<Image::IRealGrayImageWx>    originalImg{};
    std::unique_ptr<Image::IRealGrayImageWx>    resizedImg{};
    std::unique_ptr<Image::IRealGrayImageWx>    noisyImg{};
    std::unique_ptr<Image::IComplexGrayImageWx> imgDFT{};
    std::unique_ptr<Image::IComplexGrayImageWx> imgDFTMasked{};
    std::unique_ptr<Image::IRealGrayImageWx>    processedImg{};

    template <typename T> T fftShift(const T& matrix);
    template <typename T> T ifftShift(const T& matrix);
    Image::mat logify(const Image::matComplex& mat);
    wxBitmap toWxBitmap(const Image::mat& mat);
    Image::mat generateMask(int width, int height, int maskSize, FilterPassMode pass);
};