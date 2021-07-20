///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/headerctrl.h
// Purpose:     wxMSW native wxHeaderCtrl
// Author:      Vadim Zeitlin
// Created:     2008-12-01
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_HEADERCTRL_H_
#define _WX_MSW_HEADERCTRL_H_

#include "wx/compositewin.h"

class wxMSWHeaderCtrl;

// ----------------------------------------------------------------------------
// wxHeaderCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxHeaderCtrl : public wxCompositeWindow<wxHeaderCtrlBase>
{
public:
    wxHeaderCtrl() = default;

    wxHeaderCtrl(wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxHD_DEFAULT_STYLE,
                 std::string_view name = wxHeaderCtrlNameStr)
    {
        Create(parent, id, pos, size, style, name);
    }

    wxHeaderCtrl(const wxHeaderCtrl&) = delete;
    wxHeaderCtrl& operator=(const wxHeaderCtrl&) = delete;
    wxHeaderCtrl(wxHeaderCtrl&&) = default;
    wxHeaderCtrl& operator=(wxHeaderCtrl&&) = default;

    [[maybe_unused]] bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHD_DEFAULT_STYLE,
                std::string_view name = wxHeaderCtrlNameStr);

    // Window style handling.
    void SetWindowStyleFlag(long style) override;

protected:
    // Override wxWindow methods which must be implemented by a new control.
    wxSize DoGetBestSize() const override;

private:
    void DoSetCount(unsigned int count) override;
    unsigned int DoGetCount() const override;
    void DoUpdate(unsigned int idx) override;

    void DoScrollHorz(int dx) override;

    void DoSetColumnsOrder(const std::vector<int>& order) override;
    std::vector<int> DoGetColumnsOrder() const override;

    // Pure virtual method inherited from wxCompositeWindow.
    wxWindowList GetCompositeWindowParts() const override;    

    // Events.
    void OnSize(wxSizeEvent& event);

    // The native header control.
    wxMSWHeaderCtrl* m_nativeControl{nullptr};
    friend class wxMSWHeaderCtrl;
};

#endif // _WX_MSW_HEADERCTRL_H_

