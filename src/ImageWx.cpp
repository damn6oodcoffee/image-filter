#include "ImageWx.hpp"
#include "wx/wx.h"
#include <algorithm>
#include <string>

namespace Image {

    ResultCode RealGrayImageWx::LoadFromFile(std::string path) {
       
        wxImage image;
        if (!image.LoadFile(std::string(path))) {
            wxMessageBox("Failed to load image", "Error", wxOK | wxICON_ERROR);
            return ResultCode::error;
        }
        wxBitmap bitmap(image);

        wxNativePixelData rawBmp(bitmap);
        wxNativePixelData::Iterator p(rawBmp);

        wxSize matSize = bitmap.GetSize();
        m_mat = mat(matSize.y, std::vector<double>(matSize.x, 0));

        for (int i = 0; i < m_mat.size(); i++) {
            wxNativePixelData::Iterator rowStart = p;
            for (int j = 0; j < m_mat[0].size(); j++, p++) {
                // y coordinate index (i) has an offset since a bitmap pointer (p) starts from the top left pixel.
                m_mat[m_mat.size() - 1 - i][j] = 0.299 * p.Red() + 0.587 * p.Green() + 0.114 * p.Blue();
            }
            p = rowStart;
            p.OffsetY(rawBmp, 1);
        }
        return ResultCode::ok;
    }

    ResultCode RealGrayImageWx::SaveAsFile(std::string path) {
        return ResultCode::error;
    }

    ResultCode RealGrayImageWx::SetGrayImageMat(const mat& mat) {
        m_mat = mat;
        return ResultCode::ok;
    }

    ResultCode RealGrayImageWx::GetGrayImageMat(mat& mat) {
        mat = m_mat;
        return ResultCode::ok;
    }

    ResultCode RealGrayImageWx::GetWxBitmap(wxBitmap& bitmap) {
        if (m_mat.size() == 0 || m_mat[0].size() == 0) {
            bitmap = wxBitmap(1, 1);
            return ResultCode::error;
        }
            
  
        wxBitmap matBitmap(m_mat[0].size(), m_mat.size(), 24);
        wxNativePixelData rawBmp(matBitmap);
        wxNativePixelData::Iterator p(rawBmp);
    
        mat normalizedMat{ getNormalizedMat() };

        for (int i = 0; i < normalizedMat.size(); i++) {
            wxNativePixelData::Iterator rowStart = p;
            for (int j = 0; j < normalizedMat[0].size(); j++, p++) {
                
                p.Red() = int(normalizedMat[i][j] * 255);
                p.Green() = int(normalizedMat[i][j] * 255);
                p.Blue() = int(normalizedMat[i][j] * 255);
            }
            p = rowStart;
            p.OffsetY(rawBmp, 1);
        }
        bitmap = matBitmap;
        return ResultCode::ok;
    }

    ResultCode RealGrayImageWx::Reset() {
        m_mat = mat{};
        return ResultCode::ok;
    }
    
    mat RealGrayImageWx::getNormalizedMat() {
        double maxVal = getMax();
        double minVal = getMin();
        double normConst = abs(maxVal - minVal);
        mat normalizedMat{ m_mat };
        if (normConst == 0) 
            return normalizedMat;
        for (auto& row : normalizedMat) {
            for (auto& colElem : row) {
                colElem = (colElem - minVal) / normConst;
            }
        }
        return normalizedMat;
    }

    double RealGrayImageWx::getMax() {
        std::vector<double> maxEachRow{};
        for (auto& row : m_mat) {
            maxEachRow.push_back(*std::max_element(row.begin(), row.end()));
        }
        return *std::max_element(maxEachRow.begin(), maxEachRow.end());
    }   

    double RealGrayImageWx::getMin() {
        std::vector<double> minEachRow{};
        for (auto& row : m_mat) {
            minEachRow.push_back(*std::min_element(row.begin(), row.end()));
        }
        return *std::min_element(minEachRow.begin(), minEachRow.end());
    }

 


    ResultCode ComplexGrayImageWx::LoadFromFile(std::string path) {
        wxImage image;
        if (!image.LoadFile(std::string(path))) {
            wxMessageBox("Failed to load image", "Error", wxOK | wxICON_ERROR);
            return ResultCode::error;
        }
        wxBitmap bitmap(image);

        wxNativePixelData rawBmp(bitmap);
        wxNativePixelData::Iterator p(rawBmp);

        wxSize matSize = bitmap.GetSize();
        m_mat = matComplex(matSize.y, std::vector<std::complex<double>>(matSize.x, {0.0, 0.0}));

        for (int i = 0; i < m_mat.size(); i++) {
            wxNativePixelData::Iterator rowStart = p;
            for (int j = 0; j < m_mat[0].size(); j++, p++) {
                // y coordinate index (i) has an offset since a bitmap pointer (p) starts from the top left pixel.
                m_mat[m_mat.size() - 1 - i][j] = {0.299 * p.Red() + 0.587 * p.Green() + 0.114 * p.Blue(), 0.0};
            }
            p = rowStart;
            p.OffsetY(rawBmp, 1);
        }
        return ResultCode::ok;
    }

    ResultCode ComplexGrayImageWx::SaveAsFile(std::string path) {
        return ResultCode::error;
    }

    ResultCode ComplexGrayImageWx::SetGrayImageComplexMat(const matComplex& mat) {
        m_mat = mat;
        return ResultCode::ok;
    }

    ResultCode ComplexGrayImageWx::GetGrayImageComplexMat(matComplex& mat) {
        mat = m_mat;
        return ResultCode::ok;
    }

    ResultCode ComplexGrayImageWx::GetWxBitmap(wxBitmap& bitmap) {
        if (m_mat.size() == 0 || m_mat[0].size() == 0) {
            bitmap = wxBitmap(1, 1);
            return ResultCode::error;
        }

        wxBitmap matBitmap(m_mat[0].size(), m_mat.size(), 24);
        wxNativePixelData rawBmp(matBitmap);
        wxNativePixelData::Iterator p(rawBmp);
    
        matComplex normalizedMat{ getNormalizedMat() };

        for (int i = 0; i < normalizedMat.size(); i++) {
            wxNativePixelData::Iterator rowStart = p;
            for (int j = 0; j < normalizedMat[0].size(); j++, p++) {
                double matVal = normalizedMat[i][j].real()*normalizedMat[i][j].real() +
                                normalizedMat[i][j].imag()*normalizedMat[i][j].imag();
                p.Red() = int(matVal * 255);
                p.Green() = int(matVal * 255);
                p.Blue() = int(matVal * 255);
            }
            p = rowStart;
            p.OffsetY(rawBmp, 1);
        }
        bitmap = matBitmap;
        return ResultCode::ok;
    }

    matComplex ComplexGrayImageWx::getNormalizedMat() {
        double maxVal{ getMax() };
        double minVal{ getMin() };
        double normConst{ abs(maxVal - minVal) };
        matComplex normalizedMat{ m_mat };
        if (normConst == 0) 
            return normalizedMat;
        for (auto& row : normalizedMat) {
            for (auto& colElem : row) {
                colElem /= normConst;
            }
        }
        return normalizedMat;
    }

    ResultCode ComplexGrayImageWx::Reset() {
        m_mat = matComplex{};
        return ResultCode::ok;
    }

    double ComplexGrayImageWx::getMax() {
        double maxVal{ m_mat[0][0].real()*m_mat[0][0].real() + m_mat[0][0].imag()*m_mat[0][0].imag() };
        for (auto& row : m_mat) {
            for (auto& el : row) {
                maxVal = std::max(maxVal, sqrt(el.real()*el.real() + el.imag()*el.imag()));
            }
        }
        return maxVal;
    }   

    double ComplexGrayImageWx::getMin() {
        double minVal{ m_mat[0][0].real()*m_mat[0][0].real() + m_mat[0][0].imag()*m_mat[0][0].imag() };
        for (auto& row : m_mat) {
            for (auto& el : row) {
                minVal = std::min(minVal, sqrt(el.real()*el.real() + el.imag()*el.imag()));
            }
        }
        return minVal;
    }

   

}