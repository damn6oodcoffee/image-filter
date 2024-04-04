#include <vector>
#include <wx/gbsizer.h>
#include <wx/statline.h>
#include <wx/valnum.h>
#include "MainFrame.hpp"
#include <format>

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size) 
                : wxFrame(nullptr, wxID_ANY, title, pos, size), imgFilter()
{
   
    auto mainSizer = new wxBoxSizer(wxHORIZONTAL);
    auto imageGridSizer = new wxGridSizer(2, 2, wxSize(5,5));
    auto controlsGridBagSizer = new wxGridBagSizer();


    /* Setting up the image side of GUI */
    const int imgPanelCount{ 4 };
    wxStaticBoxSizer* staticBoxSizer[imgPanelCount];
    wxString labels[] = { "Input Image", "Fourier Transform", "Masked Fourier Transform", "Processed Image" };
    for (int i = 0; i < imgPanelCount; i++) {
        auto staticbox = new wxStaticBox(this, wxID_ANY, labels[i], wxDefaultPosition, wxDefaultSize);
        staticbox->SetForegroundColour(wxColour(85, 77, 86));
        staticBoxSizer[i] = new wxStaticBoxSizer(staticbox, wxVERTICAL);
    }

    for (int i = 0; i < imgPanelCount; i++) {
        imageGridSizer->Add(staticBoxSizer[i], 1, wxEXPAND | wxALL, FromDIP(5));
    }
    
    auto bmpSize{ wxSize(128, 128) };
    imgBitmap = new BufferedBitmap(this, wxID_ANY, wxBitmap(wxSize(1, 1)), wxDefaultPosition, bmpSize);
    dftBitmap = new BufferedBitmap(this, wxID_ANY, wxBitmap(wxSize(1, 1)), wxDefaultPosition, bmpSize);
    filteredImgBitmap = new BufferedBitmap(this, wxID_ANY, wxBitmap(wxSize(1, 1)), wxDefaultPosition, bmpSize);
    idftBitmap = new BufferedBitmap(this, wxID_ANY, wxBitmap(wxSize(1, 1)), wxDefaultPosition, bmpSize);
    dftBitmap->ShowFilterArea();

    staticBoxSizer[0]->Add(imgBitmap, 1, wxSHAPED | wxALL, FromDIP(5));
    staticBoxSizer[1]->Add(dftBitmap, 1, wxSHAPED | wxALL, FromDIP(5));
    staticBoxSizer[2]->Add(filteredImgBitmap, 1, wxSHAPED | wxALL, FromDIP(5));
    staticBoxSizer[3]->Add(idftBitmap, 1, wxSHAPED | wxALL, FromDIP(5));



    /* Setting up the controls side of GUI */

    loadImageButton = new wxButton(this, wxID_ANY, "Load...");
    loadImageButton->Bind(wxEVT_BUTTON, &MainFrame::OnOpenImage, this);
    imageNameTxtCtrl = new wxTextCtrl(this, wxID_ANY);
    imageNameTxtCtrl->SetEditable(0);
    auto loadImageTxt = new wxStaticText(this, wxID_ANY, "Image:");

    
    computeDFTButton = new wxButton(this, wxID_ANY, "Compute DFT");
    computeDFTButton->Bind(wxEVT_BUTTON, &MainFrame::OnComputeDFT, this);

    addNoiseButton = new wxButton(this, wxID_ANY, "Add Noise");
    addNoiseButton->Bind(wxEVT_BUTTON, &MainFrame::OnAddNoise, this);

    wxIntegerValidator<unsigned int> intValidator;
    intValidator.SetRange(1, 10000);

    wxFloatingPointValidator<double> floatValidator(nullptr, wxNUM_VAL_NO_TRAILING_ZEROES);
    floatValidator.SetRange(0.0, 100.0);

    auto noisePercentTxt = new wxStaticText(this, wxID_ANY, "Noise Power:    ", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    noisePercentTxtCtrl = new wxTextCtrl(this, wxID_ANY);
    noisePercentTxtCtrl->SetValue("50");
    noisePercentTxtCtrl->SetValidator(floatValidator);

    resizeWidthTxtCtrl = new wxTextCtrl(this, wxID_ANY);
    resizeHeightTxtCtrl = new wxTextCtrl(this, wxID_ANY);
    resizeWidthTxtCtrl->SetValidator(intValidator);
    resizeHeightTxtCtrl->SetValidator(intValidator);
    
    resizeButton = new wxButton(this, wxID_ANY, "Resize");
    wxString choices[2] = { wxString("Zero Padding"), wxString("Bilinear Interpolation") };
    resizeOptions = new wxRadioBox(this, wxID_ANY, "Resize Option", wxDefaultPosition, wxDefaultSize, 2, choices, 2, wxRA_SPECIFY_COLS);
    resizeOptions->Bind(wxEVT_RADIOBOX, &MainFrame::OnChangeResizeOption, this);
    resizeButton->Bind(wxEVT_BUTTON, &MainFrame::OnResizeImage, this);
    resizeWidthTxtCtrl->SetValue("0");
    resizeHeightTxtCtrl->SetValue("0");
    

    computeIDFTButton = new wxButton(this, wxID_ANY, "Compute IDFT", wxDefaultPosition, wxDefaultSize);
    computeIDFTButton->Bind(wxEVT_BUTTON, &MainFrame::OnComputeIDFT, this);
    choices[0] = wxString("Normal"); choices[1] = wxString("log2");
    dftScaleOptions = new wxRadioBox(this, wxID_ANY, "Scale", wxDefaultPosition, wxDefaultSize, 2, choices, 1, wxRA_SPECIFY_COLS);
    dftScaleOptions->SetSelection(1);
    dftScaleOptions->Bind(wxEVT_RADIOBOX, &MainFrame::OnChangeScaleOption, this);

    areaSlider = new wxSlider(this, wxID_ANY, 1, 0, 10, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
    areaSlider->SetRange(0, 100);
    areaSlider->Bind(wxEVT_SCROLL_THUMBTRACK, &MainFrame::OnAreaChange, this);
    areaSlider->Bind(wxEVT_SCROLL_THUMBRELEASE, &MainFrame::OnThumbRelease, this);
    choices[0] = wxString("Low"); choices[1] = wxString("High");
    filterPassMode = new wxRadioBox(this, wxID_ANY, "Filter Pass Mode", wxDefaultPosition, wxDefaultSize, 2, choices, 1, wxRA_SPECIFY_COLS);
    filterPassMode->Bind(wxEVT_RADIOBOX, &MainFrame::OnChangeScaleOption, this);

    controlsGridBagSizer->Add(loadImageTxt, wxGBPosition(0, 0), wxGBSpan(1, 1));
    controlsGridBagSizer->Add(imageNameTxtCtrl, wxGBPosition(1, 0), wxGBSpan(1, 2), wxEXPAND);
    controlsGridBagSizer->Add(loadImageButton, wxGBPosition(1, 2), wxGBSpan(1, 1), wxEXPAND | wxLEFT | wxRIGHT, FromDIP(5));
    controlsGridBagSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), wxGBPosition(2, 0), wxGBSpan(1, 3), wxEXPAND | wxALL, FromDIP(15));
    controlsGridBagSizer->Add(resizeOptions, wxGBPosition(3, 0), wxGBSpan(1, 3), wxEXPAND);
    controlsGridBagSizer->Add(resizeWidthTxtCtrl, wxGBPosition(4, 0), wxGBSpan(1, 1));
    controlsGridBagSizer->Add(resizeHeightTxtCtrl, wxGBPosition(4, 1), wxGBSpan(1, 1));
    controlsGridBagSizer->Add(resizeButton, wxGBPosition(4, 2), wxGBSpan(1, 1), wxEXPAND | wxLEFT | wxRIGHT, FromDIP(5));
    controlsGridBagSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), wxGBPosition(5, 0), wxGBSpan(1, 3), wxEXPAND | wxALL, FromDIP(15));
    controlsGridBagSizer->Add(noisePercentTxt, wxGBPosition(6, 0), wxGBSpan(1, 1), wxEXPAND | wxALIGN_CENTER_VERTICAL);
    controlsGridBagSizer->Add(noisePercentTxtCtrl, wxGBPosition(6, 1), wxGBSpan(1, 1));
    controlsGridBagSizer->Add(addNoiseButton, wxGBPosition(6, 2), wxGBSpan(1, 1), wxEXPAND | wxLEFT | wxRIGHT, FromDIP(5));
    controlsGridBagSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), wxGBPosition(7, 0), wxGBSpan(1, 3), wxEXPAND | wxALL, FromDIP(15));
    controlsGridBagSizer->Add(dftScaleOptions, wxGBPosition(8, 0), wxGBSpan(2, 1), wxEXPAND);
    controlsGridBagSizer->Add(filterPassMode, wxGBPosition(8, 1), wxGBSpan(2, 1), wxEXPAND);
    controlsGridBagSizer->Add(computeDFTButton, wxGBPosition(8, 2), wxGBSpan(1, 1), wxEXPAND | wxALL, FromDIP(5));
    controlsGridBagSizer->Add(computeIDFTButton, wxGBPosition(9, 2), wxGBSpan(1, 1), wxALL, FromDIP(5));
    controlsGridBagSizer->Add(areaSlider, wxGBPosition(10, 0), wxGBSpan(1, 3), wxEXPAND);


    mainSizer->Add(imageGridSizer, 3, wxSHAPED | wxALIGN_CENTER | wxALL, FromDIP(10));
    mainSizer->Add(controlsGridBagSizer, 0, wxEXPAND | wxRIGHT | wxTOP | wxBOTTOM | wxALIGN_LEFT, FromDIP(10));

    this->SetBackgroundColour(wxColour("white"));
    this->SetSizerAndFit(mainSizer);
}

void MainFrame::OnOpenImage(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, _("Open Image"), "", "", "Image files (*.png; *jpg; *jpeg; *.bmp)|*.png; *jpg; *jpeg; *.bmp");

    if (openFileDialog.ShowModal() == wxID_CANCEL) 
        return;
    std::string path = static_cast<std::string>(openFileDialog.GetPath());
    imageNameTxtCtrl->SetValue(path);
    imgFilter.LoadFromFile(path);

    //
    wxBitmap bmp{ imgFilter.NoisyImageBmp() };
    imgBitmap->SetBitmap(bmp);
    resizeWidthTxtCtrl->SetValue(std::format("{}", bmp.GetWidth()));
    resizeHeightTxtCtrl->SetValue(std::format("{}", bmp.GetHeight()));
    // TODO: Open Image event to mediator
    dftBitmap->SetBitmap(wxBitmap(1, 1));
    filteredImgBitmap->SetBitmap(wxBitmap(1, 1));
    idftBitmap->SetBitmap(wxBitmap(1, 1));
}

void MainFrame::OnComputeDFT(wxCommandEvent& event) {
    imgFilter.ComputeFourierTransform();
    int sel{ dftScaleOptions->GetSelection() };
    changeScale(static_cast<scaleMode>(sel));
    //wxMessageBox( wxT("OnComputeDFT"), wxT("OnComputeDFT"), wxICON_INFORMATION);
    filteredImgBitmap->SetBitmap(wxBitmap(1, 1));
    idftBitmap->SetBitmap(wxBitmap(1, 1));
}


void MainFrame::OnComputeIDFT(wxCommandEvent& event) {
    int sel{ filterPassMode->GetSelection() };
    double maskSize{ static_cast<double>(areaSlider->GetValue()) / 100.0 };
    switch (sel) {
        case 0: {
            imgFilter.ApplyFilterMask(maskSize, ImageFilter::FilterPassMode::low);
            break;
        }
        case 1: {
            imgFilter.ApplyFilterMask(maskSize, ImageFilter::FilterPassMode::high);
            break;
        }
    }
    imgFilter.ComputeInverseFourierTransform();
    sel = dftScaleOptions->GetSelection();
    changeScale(static_cast<scaleMode>(sel));
    wxBitmap bmp{ imgFilter.ProccessedImageBmp() };
    idftBitmap->SetBitmap(bmp);
    //wxMessageBox( wxT("OnComputeIDFT"), wxT("OnComputeIDFT"), wxICON_INFORMATION);
}


void MainFrame::OnAddNoise(wxCommandEvent& event) {
    double percent{};
    if (!noisePercentTxtCtrl->GetValue().ToDouble(&percent)) {
        return;
    }
    imgFilter.AddNoise(percent);
    wxBitmap bmp{ imgFilter.NoisyImageBmp() };
    imgBitmap->SetBitmap(bmp);
    //wxMessageBox( wxT("OnAddNoise"), wxT("OnAddNoise"), wxICON_INFORMATION);
}

void MainFrame::OnResizeImage(wxCommandEvent& event) {
    int sel{ resizeOptions->GetSelection() };
    int width{}, height{};
    if (!resizeWidthTxtCtrl->GetValue().ToInt(&width)) {
        return;
    }
    if (!resizeHeightTxtCtrl->GetValue().ToInt(&height)) {
        return;
    }

    switch (sel) {
        case 0: {
            imgFilter.ResizeImage(width, height, ImageFilter::ResizeMode::zeroPadding);
            break;
        }
        case 1: {
            imgFilter.ResizeImage(width, height, ImageFilter::ResizeMode::bilinear);
            break;
        }
    }
  
    //
    wxBitmap bmp{ imgFilter.NoisyImageBmp() };
    imgBitmap->SetBitmap(bmp);
    resizeWidthTxtCtrl->SetValue(std::format("{}", bmp.GetWidth()));
    resizeHeightTxtCtrl->SetValue(std::format("{}", bmp.GetHeight()));
    // TODO: Resize Image event to mediator
    // 
    // wxMessageBox( wxT("OnResizeImage"), wxT("OnResizeImage"), wxICON_INFORMATION);
    dftBitmap->SetBitmap(wxBitmap(1,1));
    filteredImgBitmap->SetBitmap(wxBitmap(1, 1));
    idftBitmap->SetBitmap(wxBitmap(1, 1));
}


void MainFrame::OnChangeResizeOption(wxCommandEvent& event) {

    //wxMessageBox( wxT("OnChangeResizeOption"), wxT("OnChangeResizeOption"), wxICON_INFORMATION);
}
void MainFrame::OnChangeScaleOption(wxCommandEvent& event) {
    int sel{ dftScaleOptions->GetSelection() };
    changeScale(static_cast<scaleMode>(sel));
    //wxMessageBox( wxT("OnChangeScaleOption"), wxT("OnChangeScaleOption"), wxICON_INFORMATION);
}

void MainFrame::changeScale(scaleMode mode) {
    switch (mode) {
        case scaleMode::normal: {
            wxBitmap bmp{ imgFilter.DFTImageBmp() };
            dftBitmap->SetBitmap(bmp);
            bmp = imgFilter.MaskedDFTImageBmp();
            filteredImgBitmap->SetBitmap(bmp);
            break;
        }
        case scaleMode::log2: {
            wxBitmap bmp{ imgFilter.LogDFTImageBmp() };
            dftBitmap->SetBitmap(bmp);
            bmp = imgFilter.LogMaskedDFTImageBmp();
            filteredImgBitmap->SetBitmap(bmp);
            break;
        }
    }
}

void MainFrame::OnAreaChange(wxScrollEvent& event) {
    double maskSize{ static_cast<double>(areaSlider->GetValue()) / 100.0 };
    dftBitmap->SetCircleAreaSize(maskSize);
}

void MainFrame::OnThumbRelease(wxScrollEvent& event) {

}

