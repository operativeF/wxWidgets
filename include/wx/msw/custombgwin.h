///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/custombgwin.h
// Purpose:     wxMSW implementation of wxCustomBackgroundWindow
// Author:      Vadim Zeitlin
// Created:     2011-10-10
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_CUSTOMBGWIN_H_
#define _WX_MSW_CUSTOMBGWIN_H_

#include "wx/bitmap.h"
#include "wx/brush.h"

template <class W>
class wxCustomBackgroundWindow : public W,
                                 public wxCustomBackgroundWindowBase
{
public:
    using BaseWindowClass = W;

    wxCustomBackgroundWindow() = default;

    ~wxCustomBackgroundWindow() override { delete m_backgroundBrush; }

    wxCustomBackgroundWindow(const wxCustomBackgroundWindow<W>&)  = delete;
	wxCustomBackgroundWindow& operator=(const wxCustomBackgroundWindow<W>&) = delete;
    wxCustomBackgroundWindow(wxCustomBackgroundWindow<W>&&)  = default;
	wxCustomBackgroundWindow& operator=(wxCustomBackgroundWindow<W>&&) = default;

protected:
    void DoSetBackgroundBitmap(const wxBitmap& bmp) override
    {
        delete m_backgroundBrush;
        m_backgroundBrush = bmp.IsOk() ? new wxBrush(bmp) : nullptr;

        // Our transparent children should use our background if we have it,
        // otherwise try to restore m_inheritBgCol to some reasonable value: true
        // if we also have non-default background colour or false otherwise.
        BaseWindowClass::m_inheritBgCol = bmp.IsOk()
                                            || BaseWindowClass::UseBgCol();
    }

    WXHBRUSH MSWGetCustomBgBrush() override
    {
        if ( m_backgroundBrush )
            return (WXHBRUSH)m_backgroundBrush->GetResourceHandle();

        return BaseWindowClass::MSWGetCustomBgBrush();
    }

    wxBrush *m_backgroundBrush{nullptr};
};

#endif // _WX_MSW_CUSTOMBGWIN_H_
