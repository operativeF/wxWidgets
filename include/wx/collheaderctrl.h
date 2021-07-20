/////////////////////////////////////////////////////////////////////////////
// Name:        wx/collheaderctrl.h
// Purpose:     wxCollapsibleHeaderCtrl
// Author:      Tobias Taschner
// Created:     2015-09-19
// Copyright:   (c) 2015 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLLAPSIBLEHEADER_CTRL_H_
#define _WX_COLLAPSIBLEHEADER_CTRL_H_

#include "wx/defs.h"

#if wxUSE_COLLPANE

#include "wx/control.h"

// class name
inline constexpr char wxCollapsibleHeaderCtrlNameStr[] = "collapsibleHeader";

//
// wxGenericCollapsibleHeaderCtrl
//

class WXDLLIMPEXP_CORE wxCollapsibleHeaderCtrlBase : public wxControl
{
public:
    wxCollapsibleHeaderCtrlBase() = default;

    wxCollapsibleHeaderCtrlBase(wxWindow *parent,
        wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxBORDER_NONE,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxASCII_STR(wxCollapsibleHeaderCtrlNameStr))
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    wxCollapsibleHeaderCtrlBase(const wxCollapsibleHeaderCtrlBase&) = delete;
    wxCollapsibleHeaderCtrlBase& operator=(const wxCollapsibleHeaderCtrlBase&) = delete;
    wxCollapsibleHeaderCtrlBase(wxCollapsibleHeaderCtrlBase&&) = default;
    wxCollapsibleHeaderCtrlBase& operator=(wxCollapsibleHeaderCtrlBase&&) = default;

    [[maybe_unused]] bool Create(wxWindow *parent,
        wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxBORDER_NONE,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxASCII_STR(wxCollapsibleHeaderCtrlNameStr))
    {
        if ( !wxControl::Create(parent, id, pos, size, style, validator, name.ToStdString()) )
            return false;

        SetLabel(label);

        return true;
    }

    virtual void SetCollapsed(bool collapsed = true) = 0;

    virtual bool IsCollapsed() const = 0;
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COLLAPSIBLEHEADER_CHANGED, wxCommandEvent);

#define wxCollapsibleHeaderChangedHandler(func) \
    wxEVENT_HANDLER_CAST(wxCommandEventFunction, func)

#define EVT_COLLAPSIBLEHEADER_CHANGED(id, fn) \
    wx__DECLARE_EVT1(wxEVT_COLLAPSIBLEHEADER_CHANGED, id, wxCollapsibleHeaderChangedHandler(fn))

// Currently there is only the native implementation, use it for all ports.

#include "wx/generic/collheaderctrl.h"

class WXDLLIMPEXP_CORE wxCollapsibleHeaderCtrl
    : public wxGenericCollapsibleHeaderCtrl
{
public:
    wxCollapsibleHeaderCtrl() = default;

    wxCollapsibleHeaderCtrl(wxWindow *parent,
        wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxBORDER_NONE,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxASCII_STR(wxCollapsibleHeaderCtrlNameStr))
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

   wxCollapsibleHeaderCtrl(const wxCollapsibleHeaderCtrl&) = delete;
   wxCollapsibleHeaderCtrl& operator=(const wxCollapsibleHeaderCtrl&) = delete;
   wxCollapsibleHeaderCtrl(wxCollapsibleHeaderCtrl&&) = default;
   wxCollapsibleHeaderCtrl& operator=(wxCollapsibleHeaderCtrl&&) = default;
};

#endif // wxUSE_COLLPANE

#endif // _WX_COLLAPSIBLEHEADER_CTRL_H_
