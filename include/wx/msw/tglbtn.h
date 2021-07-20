/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/tglbtn.h
// Purpose:     Declaration of the wxToggleButton class, which implements a
//              toggle button under wxMSW.
// Author:      John Norris, minor changes by Axel Schlueter
// Modified by:
// Created:     08.02.01
// Copyright:   (c) 2000 Johnny C. Norris II
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOGGLEBUTTON_H_
#define _WX_TOGGLEBUTTON_H_

#include "wx/bitmap.h"

// Checkbox item (single checkbox)
class WXDLLIMPEXP_CORE wxToggleButton : public wxToggleButtonBase
{
public:
    wxToggleButton() = default;
    wxToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   std::string_view name = wxToggleButtonNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    wxToggleButton(const wxToggleButton&) = delete;
    wxToggleButton& operator=(const wxToggleButton&) = delete;
    wxToggleButton(wxToggleButton&&) = default;
    wxToggleButton& operator=(wxToggleButton&&) = default;

    [[maybe_unused]] bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                std::string_view name = wxToggleButtonNameStr);

    void SetValue(bool value) override;
    bool GetValue() const override;

    bool MSWCommand(WXUINT param, WXWORD id) override;
    void Command(wxCommandEvent& event) override;

    // returns true if the platform should explicitly apply a theme border
    bool CanApplyThemeBorder() const override { return false; }

protected:
    wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle = nullptr) const override;

    bool MSWIsPushed() const override;

    // current state of the button (when owner-drawn)
    bool m_state{false};

public:
	wxClassInfo *GetClassInfo() const override;
	static wxClassInfo ms_classInfo;
	static wxObject* wxCreateObject();
};

//-----------------------------------------------------------------------------
// wxBitmapToggleButton
//-----------------------------------------------------------------------------


class WXDLLIMPEXP_CORE wxBitmapToggleButton: public wxToggleButton
{
public:
    // construction/destruction
    wxBitmapToggleButton() = default;
    wxBitmapToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   std::string_view name = wxToggleButtonNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    // Create the control
    [[maybe_unused]] bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                std::string_view name = wxToggleButtonNameStr);

    // deprecated synonym for SetBitmapLabel()
    wxDEPRECATED_INLINE( void SetLabel(const wxBitmap& bitmap),
       SetBitmapLabel(bitmap); )
    // prevent virtual function hiding
    void SetLabel(const wxString& label) override { wxToggleButton::SetLabel(label); }

private:
    wxDECLARE_DYNAMIC_CLASS(wxBitmapToggleButton);
};

#endif // _WX_TOGGLEBUTTON_H_

