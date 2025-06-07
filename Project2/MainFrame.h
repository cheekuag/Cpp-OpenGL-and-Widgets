#pragma once
#include <wx/wx.h>

class GLCanvas;
class wxSlider;
class wxCheckBox;

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);
private:
    GLCanvas* canvas;
    wxPanel* controlPanel;
    wxSlider* slider;
    wxCheckBox* checkbox;

    void OnSliderChanged(wxCommandEvent& event);
    void OnCheckBox(wxCommandEvent& event);
};
