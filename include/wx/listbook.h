///////////////////////////////////////////////////////////////////////////////
// Name:        wx/listbook.h
// Purpose:     wxListbook: wxListCtrl and wxNotebook combination
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.08.03
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LISTBOOK_H_
#define _WX_LISTBOOK_H_

#include "wx/defs.h"

#if wxUSE_LISTBOOK

#include "wx/bookctrl.h"
#include "wx/containr.h"

class WXDLLIMPEXP_FWD_CORE wxListView;
class WXDLLIMPEXP_FWD_CORE wxListEvent;

wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_CORE, wxEVT_LISTBOOK_PAGE_CHANGED,  wxBookCtrlEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_CORE, wxEVT_LISTBOOK_PAGE_CHANGING, wxBookCtrlEvent );

// wxListbook flags
inline constexpr int wxLB_DEFAULT    = wxBK_DEFAULT;
inline constexpr int wxLB_TOP        = wxBK_TOP;
inline constexpr int wxLB_BOTTOM     = wxBK_BOTTOM;
inline constexpr int wxLB_LEFT       = wxBK_LEFT;
inline constexpr int wxLB_RIGHT      = wxBK_RIGHT;
inline constexpr int wxLB_ALIGN_MASK = wxBK_ALIGN_MASK;

// ----------------------------------------------------------------------------
// wxListbook
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxListbook : public wxNavigationEnabled<wxBookCtrlBase>
{
public:
    wxListbook() = default;

    wxListbook(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const std::string& name = "")
    {
        (void)Create(parent, id, pos, size, style, name);
    }

    wxListbook(const wxListbook&) = delete;
    wxListbook& operator=(const wxListbook&) = delete;
    wxListbook(wxListbook&&) = default;
    wxListbook& operator=(wxListbook&&) = default;

    // quasi ctor
    [[maybe_unused]] bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const std::string& name = "");


    // overridden base class methods
    bool SetPageText(size_t n, const std::string& strText) override;
    std::string GetPageText(size_t n) const override;
    int GetPageImage(size_t n) const override;
    bool SetPageImage(size_t n, int imageId) override;
    bool InsertPage(size_t n,
                            wxWindow *page,
                            const std::string& text,
                            bool bSelect = false,
                            int imageId = NO_IMAGE) override;
    int SetSelection(size_t n) override { return DoSetSelection(n, SetSelection_SendEvent); }
    int ChangeSelection(size_t n) override { return DoSetSelection(n); }
    int HitTest(const wxPoint& pt, long *flags = nullptr) const override;
    void SetImageList(wxImageList *imageList) override;

    bool DeleteAllPages() override;

    wxListView* GetListView() const { return (wxListView*)m_bookctrl; }

protected:
    wxWindow *DoRemovePage(size_t page) override;

    void UpdateSelectedPage(size_t newsel) override;

    wxBookCtrlEvent* CreatePageChangingEvent() const override;
    void MakeChangedEvent(wxBookCtrlEvent &event) override;

    // Get the correct wxListCtrl flags to use depending on our own flags.
    long GetListCtrlFlags() const;

    // event handlers
    void OnListSelected(wxListEvent& event);
    void OnSize(wxSizeEvent& event);

private:
    // this should be called when we need to be relaid out
    void UpdateSize();


    wxDECLARE_EVENT_TABLE();

public:
	wxClassInfo *GetClassInfo() const override;
	static wxClassInfo ms_classInfo;
	static wxObject* wxCreateObject();
};

// ----------------------------------------------------------------------------
// listbook event class and related stuff
// ----------------------------------------------------------------------------

// wxListbookEvent is obsolete and defined for compatibility only (notice that
// we use #define and not typedef to also keep compatibility with the existing
// code which forward declares it)
#define wxListbookEvent wxBookCtrlEvent
using wxListbookEventFunction = wxBookCtrlEventFunction;
#define wxListbookEventHandler(func) wxBookCtrlEventHandler(func)

#define EVT_LISTBOOK_PAGE_CHANGED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_LISTBOOK_PAGE_CHANGED, winid, wxBookCtrlEventHandler(fn))

#define EVT_LISTBOOK_PAGE_CHANGING(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_LISTBOOK_PAGE_CHANGING, winid, wxBookCtrlEventHandler(fn))

// old wxEVT_COMMAND_* constants
#define wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED    wxEVT_LISTBOOK_PAGE_CHANGED
#define wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING   wxEVT_LISTBOOK_PAGE_CHANGING

#endif // wxUSE_LISTBOOK

#endif // _WX_LISTBOOK_H_
