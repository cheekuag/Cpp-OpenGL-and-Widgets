#include "MainFrame.h"
#include "GLCanvas.h"
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/log.h>

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {

    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);

    canvas = new GLCanvas(this);

    // Load and scale icon image to 48x48 for button
    wxImage img("icon.png", wxBITMAP_TYPE_PNG);
    wxBitmap buttonBitmap;
    if (img.IsOk()) {
        wxImage scaled = img.Scale(48, 48, wxIMAGE_QUALITY_HIGH);
        buttonBitmap = wxBitmap(scaled);
    }
    else {
        wxLogError("Failed to load icon.png for button");
    }

    wxButton* toggleButton = new wxButton(this, wxID_ANY, wxEmptyString);

    if (buttonBitmap.IsOk()) {
        toggleButton->SetBitmap(buttonBitmap);
        toggleButton->SetMinSize(wxSize(48, 48));
        toggleButton->SetMaxSize(wxSize(48, 48));
        toggleButton->SetSize(wxSize(48, 48));
        toggleButton->SetBitmapMargins(wxSize(0, 0));  // optional: no padding
    }

    leftSizer->Add(toggleButton, 0, wxALL, 5);
    leftSizer->Add(canvas, 1, wxEXPAND);

    controlPanel = new wxPanel(this);
    controlPanel->SetBackgroundColour(*wxWHITE);

    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    slider = new wxSlider(controlPanel, wxID_ANY, 0, 0, 360);
    checkbox = new wxCheckBox(controlPanel, wxID_ANY, "Show Circle");

    panelSizer->Add(slider, 0, wxALL | wxEXPAND, 10);
    panelSizer->Add(checkbox, 0, wxALL, 10);

    controlPanel->SetSizer(panelSizer);
    controlPanel->Hide();

    mainSizer->Add(leftSizer, 1, wxEXPAND);
    mainSizer->Add(controlPanel, 0, wxEXPAND);

    SetSizer(mainSizer);

    slider->Bind(wxEVT_SLIDER, &MainFrame::OnSliderChanged, this);
    checkbox->Bind(wxEVT_CHECKBOX, &MainFrame::OnCheckBox, this);

    toggleButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        bool visible = controlPanel->IsShown();
        controlPanel->Show(!visible);
        this->Layout();
        });
}

void MainFrame::OnSliderChanged(wxCommandEvent& event) {
    canvas->SetRotation(slider->GetValue());
}

void MainFrame::OnCheckBox(wxCommandEvent& event) {
    canvas->SetShowCircle(checkbox->GetValue());
}
