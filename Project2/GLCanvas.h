#pragma once
#include <wx/glcanvas.h>

class GLCanvas : public wxGLCanvas {
public:
    GLCanvas(wxWindow* parent);
    ~GLCanvas();

    void SetRotation(float angle);
    void SetShowTriangle(bool show);
    void SetShowCircle(bool show);

private:
    wxGLContext* m_context;
    GLuint iconTexture = 0;
    float rotation = 0;
    bool showTriangle = true;
    bool showCircle = false;
    bool showPanel = false;

    void LoadTexture();
    void Render();
    void DrawTriangle();
    void DrawCircle();
    void DrawTextureQuad(int x, int y, int w, int h);

    void OnPaint(wxPaintEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnMouse(wxMouseEvent& event);

    wxDECLARE_EVENT_TABLE();
};
