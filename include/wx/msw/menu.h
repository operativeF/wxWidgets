/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/menu.h
// Purpose:     wxMenu, wxMenuBar classes
// Author:      Julian Smart
// Modified by: Vadim Zeitlin (wxMenuItem is now in separate file)
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MENU_H_
#define _WX_MENU_H_

#include "wx/private/menuradio.h"

#if wxUSE_ACCEL
    #include "wx/accel.h"
    #include "wx/dynarray.h"

    using wxAcceleratorArray = std::vector<wxAcceleratorEntry>;
#endif // wxUSE_ACCEL

class WXDLLIMPEXP_FWD_CORE wxFrame;

// ----------------------------------------------------------------------------
// Menu
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMenu : public wxMenuBase
{
public:
    // ctors & dtor
    wxMenu(const wxString& title, long style = 0)
        : wxMenuBase(title, style)
    { 
        InitNoCreate();

        // create the menu
        m_hMenu = (WXHMENU)CreatePopupMenu();
        if ( !m_hMenu )
        {
            wxLogLastError(wxT("CreatePopupMenu"));
        }

        // if we have a title, insert it in the beginning of the menu
        if ( !m_title.empty() )
        {
            const wxString aTitle = m_title;
            m_title.clear(); // so that SetTitle() knows there was no title before
            SetTitle(aTitle);
        }
    }

    wxMenu(long style = 0) : wxMenuBase(style)
    { 
        InitNoCreate();

        // create the menu
        m_hMenu = (WXHMENU)CreatePopupMenu();
        if ( !m_hMenu )
        {
            wxLogLastError(wxT("CreatePopupMenu"));
        }

        // if we have a title, insert it in the beginning of the menu
        if ( !m_title.empty() )
        {
            const wxString title = m_title;
            m_title.clear(); // so that SetTitle() knows there was no title before
            SetTitle(title);
        }
    }

    ~wxMenu() override;

    wxMenu(const wxMenu&) = delete;
    wxMenu& operator=(const wxMenu&) = delete;
    wxMenu(wxMenu&&) = default;
    wxMenu& operator=(wxMenu&&) = default;

    void Break() override;

    void SetTitle(const wxString& title) override;

    // MSW-only methods
    // ----------------

    // Create a new menu from the given native HMENU. Takes ownership of the
    // menu handle and will delete it when this object is destroyed.
    static wxMenu *MSWNewFromHMENU(WXHMENU hMenu) { return new wxMenu(hMenu); }

    // Detaches HMENU so that it isn't deleted when this object is destroyed.
    // Don't use this object after calling this method.
    WXHMENU MSWDetachHMENU() { WXHMENU m = m_hMenu; m_hMenu = nullptr; return m; }

    // Process WM_COMMAND.
    virtual bool MSWCommand(WXUINT param, WXWORD id);

    // implementation only from now on
    // -------------------------------

    // get the native menu handle
    WXHMENU GetHMenu() const { return m_hMenu; }

    // Return the start and end position of the radio group to which the item
    // at the given position belongs. Returns false if there is no radio group
    // containing this position.
    bool MSWGetRadioGroupRange(int pos, int *start, int *end) const;

#if wxUSE_ACCEL
    // called by wxMenuBar to build its accel table from the accels of all menus
    bool HasAccels() const { return !m_accels.empty(); }
    size_t GetAccelCount() const { return m_accels.size(); }
    void CopyAccels(std::vector<wxAcceleratorEntry>& accels) const;

    // called by wxMenuItem when its accels changes
    void UpdateAccel(wxMenuItem *item);
    void RemoveAccel(wxMenuItem *item);

    // helper used by wxMenu itself (returns an iterator to an accel in m_accels)
    std::vector<wxAcceleratorEntry>::const_iterator FindAccel(int id) const;

    // used only by wxMDIParentFrame currently but could be useful elsewhere:
    // returns a new accelerator table with accelerators for just this menu
    // (shouldn't be called if we don't have any accelerators)
    std::unique_ptr<wxAcceleratorTable> CreateAccelTable() const;
#endif // wxUSE_ACCEL

    // get the menu with given handle (recursively)
    wxMenu* MSWGetMenu(WXHMENU hMenu);

#if wxUSE_OWNER_DRAWN

    int GetMaxAccelWidth()
    {
        if (m_maxAccelWidth == -1)
            CalculateMaxAccelWidth();
        return m_maxAccelWidth;
    }

    void ResetMaxAccelWidth()
    {
        m_maxAccelWidth = -1;
    }

private:
    void CalculateMaxAccelWidth();

#endif // wxUSE_OWNER_DRAWN

protected:
    wxMenuItem* DoAppend(wxMenuItem *item) override;
    wxMenuItem* DoInsert(size_t pos, wxMenuItem *item) override;
    wxMenuItem* DoRemove(wxMenuItem *item) override;

private:
    // This constructor is private, use MSWNewFromHMENU() to use it.
    wxMenu(WXHMENU hMenu);

    // Common part of all ctors, it doesn't create a new HMENU.
    void InitNoCreate();

    // Common part of all ctors except of the one above taking a native menu
    // handler: calls InitNoCreate() and also creates a new menu.
    

    // common part of Append/Insert (behaves as Append is pos == (size_t)-1)
    bool DoInsertOrAppend(wxMenuItem *item, size_t pos = (size_t)-1);


    // This variable contains the description of the radio item groups and
    // allows to find whether an item at the given position is part of the
    // group and also where its group starts and ends.
    //
    // It is initially NULL and only allocated if we have any radio items.
    std::unique_ptr<wxMenuRadioItemsData> m_radioData;

    // if true, insert a break before appending the next item
    bool m_doBreak;

    // the menu handle of this menu
    WXHMENU m_hMenu;

#if wxUSE_ACCEL
    // the accelerators for our menu items
    wxAcceleratorArray m_accels;
#endif // wxUSE_ACCEL

#if wxUSE_OWNER_DRAWN
    // true if the menu has any ownerdrawn items
    bool m_ownerDrawn;

    // the max width of menu items bitmaps
    int m_maxBitmapWidth;

    // the max width of menu items accels
    int m_maxAccelWidth;
#endif // wxUSE_OWNER_DRAWN

public:
	wxClassInfo *GetClassInfo() const override;
	static wxClassInfo ms_classInfo;
	static wxObject* wxCreateObject();
};

// ----------------------------------------------------------------------------
// Menu Bar (a la Windows)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMenuBar : public wxMenuBarBase
{
public:
    wxMenuBar();
        // FIXME: Unused under MSW
    wxMenuBar(long style);
        // menubar takes ownership of the menus arrays but copies the titles
    wxMenuBar(size_t n, wxMenu *menus[], const wxString titles[], long style = 0);
    ~wxMenuBar() override;

    wxMenuBar(const wxMenuBar&) = delete;
    wxMenuBar& operator=(const wxMenuBar&) = delete;
    wxMenuBar(wxMenuBar&&) = default;
    wxMenuBar& operator=(wxMenuBar&&) = default;

    // menubar construction
    bool Append( wxMenu *menu, const wxString &title ) override;
    bool Insert(size_t pos, wxMenu *menu, const wxString& title) override;
    wxMenu *Replace(size_t pos, wxMenu *menu, const wxString& title) override;
    wxMenu *Remove(size_t pos) override;

    void EnableTop( size_t pos, bool flag ) override;
    bool IsEnabledTop(size_t pos) const override;
    void SetMenuLabel( size_t pos, const wxString& label ) override;
    wxString GetMenuLabel( size_t pos ) const override;

    // implementation from now on
    WXHMENU Create();
    void Detach() override;
    void Attach(wxFrame *frame) override;

#if wxUSE_ACCEL
    // update the accel table (must be called after adding/deleting a menu)
    void RebuildAccelTable();
#endif // wxUSE_ACCEL

        // get the menu handle
    WXHMENU GetHMenu() const { return m_hMenu; }

    // if the menubar is modified, the display is not updated automatically,
    // call this function to update it (m_menuBarFrame should be !NULL)
    void Refresh();

    // To avoid compile warning
    void Refresh( bool eraseBackground,
                          const wxRect *rect = (const wxRect *) nullptr ) override { wxWindow::Refresh(eraseBackground, rect); }

    // Get a top level menu position or wxNOT_FOUND from its handle.
    int MSWGetTopMenuPos(WXHMENU hMenu) const;

    // Get a top level or sub menu with given handle (recursively).
    wxMenu* MSWGetMenu(WXHMENU hMenu) const;

protected:
    WXHMENU       m_hMenu{nullptr};

    // Return the MSW position for a wxMenu which is sometimes different from
    // the wxWidgets position.
    int MSWPositionForWxMenu(wxMenu *menu, int wxpos);

public:
	wxClassInfo *GetClassInfo() const override;
	static wxClassInfo ms_classInfo;
	static wxObject* wxCreateObject();
};

#endif // _WX_MENU_H_
