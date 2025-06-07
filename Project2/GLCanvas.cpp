#include "GLCanvas.h"
#include <GL/gl.h>
#include <wx/sizer.h>

#include <wx/image.h>
#include <wx/dcclient.h>
#include <cmath>

BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
EVT_PAINT(GLCanvas::OnPaint)
EVT_SIZE(GLCanvas::OnResize)
EVT_LEFT_DOWN(GLCanvas::OnMouse)
END_EVENT_TABLE()

GLCanvas::GLCanvas(wxWindow* parent)
    : wxGLCanvas(parent, wxID_ANY, nullptr),
    m_context(new wxGLContext(this)),
    rotation(0.0f),
    showTriangle(true),
    showCircle(false),
    showPanel(false),
    iconTexture(0) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    LoadTexture();
}

GLCanvas::~GLCanvas() {
    if (iconTexture) glDeleteTextures(1, &iconTexture);
    delete m_context;
}

// Helper: Flip image data vertically (RGBA)
void FlipImageVertically(unsigned char* data, int width, int height) {
    int rowSize = width * 4; // 4 bytes per pixel (RGBA)
    unsigned char* tempRow = new unsigned char[rowSize];
    for (int i = 0; i < height / 2; ++i) {
        unsigned char* row1 = data + i * rowSize;
        unsigned char* row2 = data + (height - 1 - i) * rowSize;

        memcpy(tempRow, row1, rowSize);
        memcpy(row1, row2, rowSize);
        memcpy(row2, tempRow, rowSize);
    }
    delete[] tempRow;
}

void GLCanvas::LoadTexture() {
    wxImage image("icon.png", wxBITMAP_TYPE_PNG);
    if (!image.IsOk()) {
        wxLogError("Failed to load icon.png");
        return;
    }

    wxLogMessage("Image size: %d x %d", image.GetWidth(), image.GetHeight());

    if (!image.HasAlpha()) {
        image.InitAlpha();
        unsigned char* alpha = image.GetAlpha();
        int size = image.GetWidth() * image.GetHeight();
        for (int i = 0; i < size; ++i)
            alpha[i] = 255; // fully opaque
    }

    const unsigned char* rgb = image.GetData();
    const unsigned char* alpha = image.GetAlpha();

    int w = image.GetWidth();
    int h = image.GetHeight();

    // Create RGBA buffer
    unsigned char* rgba = new unsigned char[w * h * 4];
    for (int i = 0; i < w * h; ++i) {
        rgba[i * 4 + 0] = rgb[i * 3 + 0];
        rgba[i * 4 + 1] = rgb[i * 3 + 1];
        rgba[i * 4 + 2] = rgb[i * 3 + 2];
        rgba[i * 4 + 3] = alpha[i];
    }

    // Flip image vertically before uploading to OpenGL
    FlipImageVertically(rgba, w, h);

    if (iconTexture) glDeleteTextures(1, &iconTexture);
    glGenTextures(1, &iconTexture);
    glBindTexture(GL_TEXTURE_2D, iconTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] rgba;
}

void GLCanvas::OnResize(wxSizeEvent& event) {
    Refresh();
    event.Skip();
}

void GLCanvas::OnPaint(wxPaintEvent& event) {
    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC dc(this);
    Render();
    SwapBuffers();
}

void GLCanvas::OnMouse(wxMouseEvent& event) {
    int x = event.GetX(), y = event.GetY();
    if (x >= 10 && x <= 58 && y >= 10 && y <= 58) {
        showPanel = !showPanel;
        wxWindow* parent = GetParent();
        wxSizer* sizer = parent->GetSizer();
        if (sizer && sizer->GetItemCount() > 1) {
            wxWindow* panel = sizer->GetItem(1)->GetWindow();
            if (panel) {
                panel->Show(showPanel);
                parent->Layout();
            }
        }
        Refresh();
    }
    event.Skip();
}

void GLCanvas::Render() {
    int width, height;
    GetClientSize(&width, &height);

    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Ortho projection with origin top-left
    glOrtho(0, width, height, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (showTriangle) DrawTriangle();
    if (showCircle) DrawCircle();

    // Draw icon button quad at fixed size and position
    if (iconTexture) DrawTextureQuad(10, 10, 48, 48);
}

void GLCanvas::DrawTriangle() {
    int width, height;
    GetClientSize(&width, &height);

    glPushMatrix();
    glTranslatef(width / 2, height / 2, 0);
    glRotatef(rotation, 0, 0, 1);

    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(-50, 50);
    glColor3f(0, 1, 0);
    glVertex2f(50, 50);
    glColor3f(0, 0, 1);
    glVertex2f(0, -50);
    glEnd();

    glPopMatrix();
}

void GLCanvas::DrawCircle() {
    int width, height;
    GetClientSize(&width, &height);

    glPushMatrix();
    glTranslatef(width / 2, height / 2, 0);
    glRotatef(rotation, 0, 0, 1);

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.8f, 0.2f, 0.6f);
    glVertex2f(0, 0);
    for (int i = 0; i <= 360; ++i) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(cos(rad) * 50, sin(rad) * 50);
    }
    glEnd();

    glPopMatrix();
}

void GLCanvas::DrawTextureQuad(int x, int y, int w, int h) {
    if (iconTexture == 0) {
        // Fallback: draw a colored square if texture isn't available
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
        glEnd();
        return;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, iconTexture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor3f(1.0f, 1.0f, 1.0f); // Use texture's true colors

    glBegin(GL_QUADS);
    // Normal texture coords, since image data is flipped on upload
    glTexCoord2f(0, 0); glVertex2f(x, y);         // Top-left
    glTexCoord2f(1, 0); glVertex2f(x + w, y);     // Top-right
    glTexCoord2f(1, 1); glVertex2f(x + w, y + h); // Bottom-right
    glTexCoord2f(0, 1); glVertex2f(x, y + h);     // Bottom-left
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void GLCanvas::SetRotation(float angle) {
    rotation = angle;
    Refresh();
}

void GLCanvas::SetShowTriangle(bool show) {
    showTriangle = show;
    Refresh();
}

void GLCanvas::SetShowCircle(bool show) {
    showCircle = show;
    Refresh();
}
