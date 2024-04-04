#pragma once

#include "wx\wx.h"
#include "wx\listctrl.h"
#include "BufferedBitmap.hpp"
#include <vector>
#include <string>
#include "ImageFilter.hpp"

class MainFrame : public wxFrame
{
public:
	
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	enum scaleMode{
		normal,
		log2,
	};

private:
	ImageFilter imgFilter{};

	BufferedBitmap* imgBitmap{};
	BufferedBitmap* dftBitmap{};
	BufferedBitmap* filteredImgBitmap{};
	BufferedBitmap* idftBitmap{};


	wxTextCtrl* noisePercentTxtCtrl{};
	wxTextCtrl* resizeWidthTxtCtrl{};
	wxTextCtrl* resizeHeightTxtCtrl{};
	wxTextCtrl* imageNameTxtCtrl{};

	wxButton* loadImageButton{};
	wxButton* resizeButton{};
	wxButton* addNoiseButton{};
	wxButton* computeDFTButton{};
	wxButton* computeIDFTButton{};

	wxRadioBox* resizeOptions{};
	wxRadioBox* dftScaleOptions{};
	wxRadioBox* filterPassMode{};

	wxSlider* areaSlider{};

	void OnComputeDFT(wxCommandEvent& event);
	void OnComputeIDFT(wxCommandEvent& event);
	void OnAddNoise(wxCommandEvent& event);
	void OnChangeResizeOption(wxCommandEvent& event);
	void OnChangeScaleOption(wxCommandEvent& event);
	void OnAreaChange(wxScrollEvent& event);
	void OnThumbRelease(wxScrollEvent& event);
	void OnOpenImage(wxCommandEvent& event);
	void OnResizeImage(wxCommandEvent& event);
	void changeScale(scaleMode mode);
};

