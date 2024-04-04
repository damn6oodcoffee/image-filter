
#include "Image.hpp"
#include "wx\bitmap.h"
#include "wx\rawbmp.h"

namespace Image {

    class IWxBitmapLoader {
    public:
        virtual ResultCode GetWxBitmap(wxBitmap& bitmap) = 0;
    };

    class IRealGrayImageWx : public ILoader, public IResetter, public IRealGrayImage, public  IWxBitmapLoader {};

    class IComplexGrayImageWx : public ILoader, public IResetter, public IComplexGrayImage, public IWxBitmapLoader {};

    class RealGrayImageWx : public IRealGrayImageWx {
    public:
        RealGrayImageWx() {};
        ~RealGrayImageWx() {};
        ResultCode LoadFromFile(std::string path) override;
        ResultCode SaveAsFile(std::string path) override;
        ResultCode SetGrayImageMat(const mat& mat) override;
        ResultCode GetGrayImageMat(mat& mat) override;
        ResultCode GetWxBitmap(wxBitmap& bitmap) override;
        ResultCode Reset() override;
    private:
        mat getNormalizedMat();
        double getMax();
        double getMin();
        mat m_mat{};
    };

    class ComplexGrayImageWx : public IComplexGrayImageWx {
    public:
        ComplexGrayImageWx() {};
        ~ComplexGrayImageWx() {};
        ResultCode LoadFromFile(std::string path) override;
        ResultCode SaveAsFile(std::string path) override;
        ResultCode SetGrayImageComplexMat(const matComplex& mat) override;
        ResultCode GetGrayImageComplexMat(matComplex& mat) override;
        ResultCode GetWxBitmap(wxBitmap& bitmap) override;
        ResultCode Reset() override;
    private:
        matComplex getNormalizedMat();
        double getMax();
        double getMin();
        matComplex m_mat{};
    };
}