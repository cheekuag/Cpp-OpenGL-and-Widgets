#include <wx/wx.h>
#include "MainFrame.h"

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        wxInitAllImageHandlers();  // Enable image loading
        MainFrame* frame = new MainFrame("GLCanvas with Controls");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
