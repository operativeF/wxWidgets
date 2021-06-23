/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/dcmemory.h
// Purpose:     wxMemoryDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCMEMORY_H_
#define _WX_DCMEMORY_H_

#include "wx/dcmemory.h"
#include "wx/msw/dc.h"

class WXDLLIMPEXP_CORE wxMemoryDCImpl: public wxMSWDCImpl
{
public:
    wxMemoryDCImpl( wxMemoryDC *owner );
    wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap );
    wxMemoryDCImpl( wxMemoryDC *owner, wxDC *dc ); // Create compatible DC

   wxMemoryDCImpl(const wxMemoryDCImpl&) = delete;
   wxMemoryDCImpl& operator=(const wxMemoryDCImpl&) = delete;
   wxMemoryDCImpl(wxMemoryDCImpl&&) = default;
   wxMemoryDCImpl& operator=(wxMemoryDCImpl&&) = default;

    // override some base class virtuals
    void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
    wxSize DoGetSize() const override;
    void DoSelect(const wxBitmap& bitmap) override;

    wxBitmap DoGetAsBitmap(const wxRect* subrect) const override
    { return subrect == nullptr ? GetSelectedBitmap() : GetSelectedBitmap().GetSubBitmapOfHDC(*subrect, GetHDC() );}

protected:
    // create DC compatible with the given one or screen if dc == NULL
    bool CreateCompatible(wxDC *dc);

    wxDECLARE_CLASS(wxMemoryDCImpl);
};

#endif
    // _WX_DCMEMORY_H_
