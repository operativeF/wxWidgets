///////////////////////////////////////////////////////////////////////////////
// Name:        wx/listctrl.h
// Purpose:     wxListCtrl class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.12.99
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LISTCTRL_H_BASE_
#define _WX_LISTCTRL_H_BASE_

#include "wx/defs.h" // headers should include this before first wxUSE_XXX check

#if wxUSE_LISTCTRL

#include "wx/listbase.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

inline constexpr char wxListCtrlNameStr[] = "listCtrl";

// ----------------------------------------------------------------------------
// include the wxListCtrl class declaration
// ----------------------------------------------------------------------------

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    #include "wx/msw/listctrl.h"
#elif defined(__WXQT__) && !defined(__WXUNIVERSAL__)
    #include "wx/qt/listctrl.h"
#else
    #include "wx/generic/listctrl.h"
#endif

// ----------------------------------------------------------------------------
// wxListView: a class which provides a better API for list control
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxListView : public wxListCtrl
{
public:
    wxListView() = default;
    wxListView( wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLC_REPORT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString &name = wxASCII_STR(wxListCtrlNameStr))
    {
        Create(parent, winid, pos, size, style, validator, name);
    }

    wxListView(const wxListView&) = delete;
    wxListView& operator=(const wxListView&) = delete;
    wxListView(wxListView&&) = default;
    wxListView& operator=(wxListView&&) = default;

    // focus/selection stuff
    // ---------------------

    // [de]select an item
    void Select(long n, bool on = true)
    {
        SetItemState(n, on ? wxLIST_STATE_SELECTED : 0, wxLIST_STATE_SELECTED);
    }

    // focus and show the given item
    void Focus(long index)
    {
        SetItemState(index, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
        EnsureVisible(index);
    }

    // get the currently focused item or -1 if none
    long GetFocusedItem() const
    {
        return GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
    }

    // get first and subsequent selected items, return -1 when no more
    long GetNextSelected(long item) const
        { return GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); }
    long GetFirstSelected() const
        { return GetNextSelected(-1); }

    // return true if the item is selected
    bool IsSelected(long index) const
        { return GetItemState(index, wxLIST_STATE_SELECTED) != 0; }

    // columns
    // -------

    void SetColumnImage(int col, int image)
    {
        wxListItem item;
        item.SetMask(wxLIST_MASK_IMAGE);
        item.SetImage(image);
        SetColumn(col, item);
    }

    void ClearColumnImage(int col) { SetColumnImage(col, -1); }

public:
	wxClassInfo *GetClassInfo() const override;
	static wxClassInfo ms_classInfo;
	static wxObject* wxCreateObject();
};

#endif // wxUSE_LISTCTRL

#endif
    // _WX_LISTCTRL_H_BASE_
