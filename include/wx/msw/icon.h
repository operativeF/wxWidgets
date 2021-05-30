/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/icon.h
// Purpose:     wxIcon class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ICON_H_
#define _WX_ICON_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/msw/gdiimage.h"

// ---------------------------------------------------------------------------
// icon data
// ---------------------------------------------------------------------------

// notice that although wxIconRefData inherits from wxBitmapRefData, it is not
// a valid wxBitmapRefData
class WXDLLIMPEXP_CORE wxIconRefData : public wxGDIImageRefData
{
public:
    wxIconRefData() = default;
    ~wxIconRefData() override { Free(); }

    void Free() override;
};

// ---------------------------------------------------------------------------
// Icon
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxIcon : public wxGDIImage
{
public:
    // ctors
        // default
    wxIcon() = default;

        // from raw data
    wxIcon(const char bits[], int width, int height);

        // from XPM data
    wxIcon(const char* const* data) { CreateIconFromXpm(data); }
        // from resource/file
    wxIcon(const wxString& name,
           wxBitmapType type = wxICON_DEFAULT_TYPE,
           int desiredWidth = -1, int desiredHeight = -1);

    wxIcon(const wxIconLocation& loc);

    ~wxIcon() override;

    virtual bool LoadFile(const wxString& name,
                          wxBitmapType type = wxICON_DEFAULT_TYPE,
                          int desiredWidth = -1, int desiredHeight = -1);

    bool CreateFromHICON(WXHICON icon);

    // implementation only from now on
    wxIconRefData *GetIconData() const { return (wxIconRefData *)m_refData; }

    WXHICON GetHICON() const { return (WXHICON)GetHandle(); }
    bool InitFromHICON(WXHICON icon, int width, int height);

    // create from bitmap (which should have a mask unless it's monochrome):
    // there shouldn't be any implicit bitmap -> icon conversion (i.e. no
    // ctors, assignment operators...), but it's ok to have such function
    void CopyFromBitmap(const wxBitmap& bmp);

protected:
    wxGDIImageRefData *CreateData() const override
    {
        return new wxIconRefData;
    }

    wxObjectRefData *CloneRefData(const wxObjectRefData *data) const override;

    // create from XPM data
    void CreateIconFromXpm(const char* const* data);

private:
    wxDECLARE_DYNAMIC_CLASS(wxIcon);
};

#endif
    // _WX_ICON_H_
