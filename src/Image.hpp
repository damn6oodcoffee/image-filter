#pragma once
#include <vector>
#include <complex>
#include <string_view>

namespace Image {

    using mat = std::vector<std::vector<double>>;
    using matComplex = std::vector<std::vector<std::complex<double>>>;

    using matRgb = std::vector<std::vector<double[3]>>;
    using matComplexRgb = std::vector<std::vector<std::complex<double>[3]>>;


    enum class ResultCode {
        ok,
        error,
        invalidInput,
    };


    class ILoader {
    public:
        virtual ResultCode LoadFromFile(std::string path) = 0;
        virtual ResultCode SaveAsFile(std::string path) = 0;
    };

    class IResetter {
    public:
        virtual ResultCode Reset() = 0;
    };

    class IRealGrayImage {
    public:
        virtual ResultCode GetGrayImageMat(mat& mat) = 0;
        virtual ResultCode SetGrayImageMat(const mat& mat) = 0;
    };


    class IComplexGrayImage {
    public:
        virtual ResultCode GetGrayImageComplexMat(matComplex& mat) = 0;
        virtual ResultCode SetGrayImageComplexMat(const matComplex& mat) = 0;
    };

    class IRealRgbImage {
    public:
        virtual ResultCode GetRgbImageMat(matRgb& mat) = 0;
        virtual ResultCode SetRgbImageMat(const matRgb& mat) = 0;
    };


    class IComplexRgbImage {
    public:
        virtual ResultCode GetRgbImageComplexMat(matComplexRgb& mat) = 0;
        virtual ResultCode SetRgbImageComplexMat(const matComplexRgb& mat) = 0;
    };





}
