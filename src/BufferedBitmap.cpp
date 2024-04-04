#include "BufferedBitmap.hpp"
#include "wx\rawbmp.h"

BufferedBitmap::BufferedBitmap(wxWindow* parent, wxWindowID id, const wxBitmap& b, const wxPoint& pos, const wxSize& size, long style)
    : wxScrolled<wxWindow>(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE | wxVSCROLL | wxHSCROLL | style)
{
    this->SetBackgroundStyle(wxBG_STYLE_PAINT); // needed for windows
    this->Bind(wxEVT_PAINT, &BufferedBitmap::OnPaint, this);
    this->SetBitmap(b);
    this->offset = wxPoint(0, 0);
}

void BufferedBitmap::OnPaint(wxPaintEvent& evt)
{
   
    this->Refresh();
    wxAutoBufferedPaintDC dc(this);

    dc.Clear();
    DoPrepareDC(dc);
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    wxGraphicsBitmap preparedBitmap = gc->CreateBitmap(bitmap);


    if (gc)
    {
        // scaling consistent with wxStaticBitmap
        const wxSize drawSize = ToDIP(GetVirtualSize());
        const wxSize bmpSize = GetScaledBitmapSize();

        double w = bmpSize.GetWidth();
        double h = bmpSize.GetHeight();
        double r = std::sqrt(h * h + w * w) / 2.0;

        double x = (drawSize.GetWidth() - w) / 2;
        double y = (drawSize.GetHeight() - h) / 2;

        gc->Scale(1, -1);
        gc->Translate(0, -h);
        gc->Translate(offset.x - (w * zoomScale / 2 - w / 2), -offset.y - (h * zoomScale / 2 - h / 2));
        gc->Scale(zoomScale, zoomScale);
    
        gc->DrawBitmap(preparedBitmap, 0.5, 0.5, int(w), int(h));


        if (drawFilterArea) {
            wxPen redPen(wxColour("Red"), 0);
            gc->SetPen(redPen);
            gc->DrawEllipse(w / 2 - filterAreaRad * r, h / 2 - filterAreaRad * r, 2 * filterAreaRad * r, 2 * filterAreaRad * r);
        }
        
        delete gc;
    }
   

}

void BufferedBitmap::SetBitmap(const wxBitmap& bitmap)
{
    this->bitmap = bitmap;


    SetScrollRate(FromDIP(10), FromDIP(10));
    SetVirtualSize(FromDIP(GetScaledBitmapSize()));

    this->Refresh();
}

const wxBitmap& BufferedBitmap::GetBitmap() const
{
    return bitmap;
}

double BufferedBitmap::GetZoomMultiplier() const
{
    return pow(ZOOM_FACTOR, zoomLevel);
}

double BufferedBitmap::GetZoomPercentage() const
{
    return GetZoomMultiplier() * 100;
}

void BufferedBitmap::ZoomIn()
{
    auto centerPos = CalcUnscrolledPosition(wxPoint(GetClientSize().GetWidth() / 2, GetClientSize().GetHeight() / 2));

    zoomLevel++;

    CenterAfterZoom(centerPos, centerPos * ZOOM_FACTOR);
    SetVirtualSize(FromDIP(GetScaledBitmapSize()));

    this->Refresh();
}

void BufferedBitmap::ZoomOut()
{
    auto centerPos = CalcUnscrolledPosition(wxPoint(GetClientSize().GetWidth() / 2, GetClientSize().GetHeight() / 2));

    zoomLevel--;

    CenterAfterZoom(centerPos, centerPos * (1.0 / ZOOM_FACTOR));
    SetVirtualSize(FromDIP(GetScaledBitmapSize()));

    this->Refresh();
}

wxSize BufferedBitmap::GetScaledBitmapSize() const
{
    //const wxSize bmpSize = bitmap.GetSize();
    const wxSize bmpSize = GetSize();
    const double zoom = GetZoomMultiplier();
    return wxSize(bmpSize.GetWidth() * zoom, bmpSize.GetHeight() * zoom);
}

void BufferedBitmap::CenterAfterZoom(wxPoint previousCenter, wxPoint currentCenter)
{
    wxPoint pixelsPerUnit;
    GetScrollPixelsPerUnit(&pixelsPerUnit.x, &pixelsPerUnit.y);

    auto delta = currentCenter - previousCenter;

    auto destX = GetViewStart().x + delta.x / pixelsPerUnit.x;
    auto destY = GetViewStart().y + delta.y / pixelsPerUnit.y;

    Scroll(destX, destY);
}


wxBEGIN_EVENT_TABLE(BufferedBitmap, wxScrolled<wxWindow>)
EVT_MOTION(BufferedBitmap::OnMouseMove)
EVT_LEFT_DOWN(BufferedBitmap::OnLMouseDown)
EVT_LEFT_UP(BufferedBitmap::OnLMouseUp)
EVT_LEAVE_WINDOW(BufferedBitmap::OnWindowLeave)
EVT_MOUSEWHEEL(BufferedBitmap::OnMouseZoom)
wxEND_EVENT_TABLE()


void BufferedBitmap::OnMouseMove(wxMouseEvent& evt) {
    if (!bLMouseDown) return;
    auto curPt = evt.GetPosition();
    rawOffset.x += (curPt.x - prevPt.x);
    rawOffset.y += (curPt.y - prevPt.y);
    offset.x += curPt.x - prevPt.x;
    offset.y += curPt.y - prevPt.y;
    prevPt = curPt;
    Refresh();
}

void BufferedBitmap::OnLMouseDown(wxMouseEvent& evt) {
    bLMouseDown = true;
    prevPt = evt.GetPosition();
}

void BufferedBitmap::OnLMouseUp(wxMouseEvent& evt) {
    bLMouseDown = false;
}

void BufferedBitmap::OnWindowLeave(wxMouseEvent& evt) {
    bLMouseDown = false;
}

void BufferedBitmap::OnMouseZoom(wxMouseEvent& evt) {
    int delta = evt.GetWheelRotation();
    wxPoint mousePos = evt.GetPosition();
    wxSize bmpSize = GetSize();
    // Setting origin to the center of the screen and inverting y axis.
    mousePos.y = bmpSize.y - mousePos.y - bmpSize.y / 2;
    mousePos.x = mousePos.x - bmpSize.x / 2;    
    // Shift needed to zoom to the cursor position.
    wxSize shiftToCursor(0,0);

    wxDouble zoomPrev = zoomScale;
    if (delta > 0) {
        zoomScale *= 1.25;

        shiftToCursor.x = -(mousePos.x * zoomScale / zoomPrev - mousePos.x);
        shiftToCursor.y = (mousePos.y * zoomScale / zoomPrev - mousePos.y);

        offset.x = (offset.x) * zoomScale / zoomPrev + shiftToCursor.x;
        offset.y = (offset.y) * zoomScale / zoomPrev + shiftToCursor.y;
    }
    else {

        shiftToCursor.x = -(mousePos.x * zoomScale / zoomPrev - mousePos.x);
        shiftToCursor.y = (mousePos.y * zoomScale / zoomPrev - mousePos.y);

        zoomScale = (zoomScale / 1.25) > 1.0 ? (zoomScale / 1.25) : 1.0;
        offset.x = (offset.x) * zoomScale / zoomPrev + shiftToCursor.x;
        offset.y = (offset.y) * zoomScale / zoomPrev + shiftToCursor.y;
    }
    Refresh();
}

void BufferedBitmap::SetCircleAreaSize(double rad) {
    filterAreaRad = rad;
    Refresh();
}

void BufferedBitmap::ShowFilterArea(bool enable) {
    drawFilterArea = enable;
    Refresh();
}