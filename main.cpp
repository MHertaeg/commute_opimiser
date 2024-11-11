// wxWidgets "Hello World" Program
 
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#include <wx/dcbuffer.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
 
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};
 
class MyFrame : public wxFrame
{
public:
    MyFrame();
 
private:
    void OnPaint(wxPaintEvent& event);
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnMouseWheel(wxMouseEvent& event);   // For zooming
    void OnMouseDown(wxMouseEvent& event);    // For initiating panning
    void OnMouseMove(wxMouseEvent& event);    // For panning the map
    void OnMouseUp(wxMouseEvent& event);      // For ending panning

    wxBitmap m_backgroundImage;  // Bitmap to store the background image

    double m_zoomLevel = 1.0;                // Zoom factor
    wxPoint m_panOffset = wxPoint(0, 0);     // Offset for panning
    wxPoint m_lastMousePos;                  // Stores the last mouse position for panning

    bool m_isPanning = false;                // Flag for panning mode
};
 
enum
{
    ID_Hello = 1
};
 
wxIMPLEMENT_APP(MyApp);
 
bool MyApp::OnInit()
{
    wxImage::AddHandler(new wxPNGHandler);  // Add PNG image handler

    MyFrame *frame = new MyFrame();
    frame->SetClientSize(800, 600);
    frame->Show(true);
    return true;
}
 
MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "Michael's Commute Optimiser")
{
    // Set the background style to wxBG_STYLE_PAINT
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    // Load the background image (change path to your image file)
    m_backgroundImage.LoadFile("C:/Users/michael.h_chemwatch/source/repos/commute/map.png", wxBITMAP_TYPE_PNG);

    Bind(wxEVT_PAINT, &MyFrame::OnPaint, this);
    Bind(wxEVT_MOUSEWHEEL, &MyFrame::OnMouseWheel, this);
    Bind(wxEVT_LEFT_DOWN, &MyFrame::OnMouseDown, this);
    Bind(wxEVT_MOTION, &MyFrame::OnMouseMove, this);
    Bind(wxEVT_LEFT_UP, &MyFrame::OnMouseUp, this);


    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar( menuBar );
 
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
 
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}
 
void MyFrame::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);

    // Clear the background with black color
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawRectangle(0, 0, GetClientSize().x, GetClientSize().y);  // Fill entire window with black

    if (m_backgroundImage.IsOk())
    {
        // Scale the image according to zoom level
        wxImage scaledImage = m_backgroundImage.ConvertToImage();
        scaledImage.Rescale(m_backgroundImage.GetWidth() * m_zoomLevel, m_backgroundImage.GetHeight() * m_zoomLevel);

        wxBitmap scaledBitmap(scaledImage);

        // Draw the scaled and panned image
        dc.DrawBitmap(scaledBitmap, m_panOffset.x, m_panOffset.y, false);
    }
}

void MyFrame::OnMouseWheel(wxMouseEvent& event)
{
    int rotation = event.GetWheelRotation();
    double zoomFactor = 1.1;  // Change zoom factor here if needed

    if (rotation > 0)
        m_zoomLevel *= zoomFactor;
    else if (rotation < 0)
        m_zoomLevel /= zoomFactor;

    Refresh();  // Repaint to apply zoom
}

void MyFrame::OnMouseDown(wxMouseEvent& event)
{
    m_isPanning = true;
    m_lastMousePos = event.GetPosition();
    CaptureMouse();  // Capture mouse to track movement outside window bounds
}

void MyFrame::OnMouseMove(wxMouseEvent& event)
{
    if (m_isPanning)
    {
        wxPoint currentMousePos = event.GetPosition();
        wxPoint delta = currentMousePos - m_lastMousePos;

        m_panOffset += delta;  // Update the pan offset by delta
        m_lastMousePos = currentMousePos;

        Refresh();  // Repaint to apply pan
    }
}

void MyFrame::OnMouseUp(wxMouseEvent& event)
{
    if (m_isPanning)
    {
        m_isPanning = false;
        ReleaseMouse();
    }
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
 
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a good tool",
                 "About Commute Optimiser", wxOK | wxICON_INFORMATION);
}
 
void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello!");
}

int main(int argc, char** argv) {
    wxApp::SetInstance(new MyApp());
    return wxEntry(argc, argv);
}