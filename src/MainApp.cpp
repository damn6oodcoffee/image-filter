#include "MainApp.hpp"


wxIMPLEMENT_APP(MainApp);

bool MainApp::OnInit() {
	wxInitAllImageHandlers();
	MainFrame* frame = new MainFrame("Fourier Transform Test", wxDefaultPosition, wxDefaultSize);
	frame->Show(true);
	return true;
}

