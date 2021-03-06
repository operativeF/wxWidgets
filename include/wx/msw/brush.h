/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/brush.h
// Purpose:     wxBrush class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BRUSH_H_
#define _WX_BRUSH_H_

class WXDLLIMPEXP_FWD_CORE wxBrush;
class WXDLLIMPEXP_FWD_CORE wxColour;
class WXDLLIMPEXP_FWD_CORE wxBitmap;

// ----------------------------------------------------------------------------
// wxBrush
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBrush : public wxBrushBase
{
public:
    wxBrush() = default;
    wxBrush(const wxColour& col, wxBrushStyle style = wxBrushStyle::Solid);
    explicit wxBrush(const wxBitmap& stipple);
    ~wxBrush() override = default;

    void SetColour(const wxColour& col) override;
    void SetColour(unsigned char r, unsigned char g, unsigned char b) override;
    void SetStyle(wxBrushStyle style) override;
    void SetStipple(const wxBitmap& stipple) override;

    bool operator==(const wxBrush& brush) const;
    bool operator!=(const wxBrush& brush) const { return !(*this == brush); }

    wxColour GetColour() const override;
    wxBrushStyle GetStyle() const override;
    wxBitmap *GetStipple() const override;

    // return the HBRUSH for this brush
    WXHANDLE GetResourceHandle() const override;

protected:
    wxGDIRefData *CreateGDIRefData() const override;
    wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxBrush);
};

#endif // _WX_BRUSH_H_
