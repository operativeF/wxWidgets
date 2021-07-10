/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/radiobut.h
// Purpose:     wxRadioButton class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBUT_H_
#define _WX_RADIOBUT_H_

#include "wx/msw/ownerdrawnbutton.h"

class WXDLLIMPEXP_CORE wxRadioButton : public wxMSWOwnerDrawnButton<wxRadioButtonBase>
{
public:
    // ctors and creation functions
    wxRadioButton() = default;

    wxRadioButton(wxWindow *parent,
                  wxWindowID id,
                  const wxString& label,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxASCII_STR(wxRadioButtonNameStr))
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    wxRadioButton(const wxRadioButton&) = delete;
    wxRadioButton& operator=(const wxRadioButton&) = delete;
    wxRadioButton(wxRadioButton&&) = default;
    wxRadioButton& operator=(wxRadioButton&&) = default;
    ~wxRadioButton() = default;

    [[maybe_unused]] bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxRadioButtonNameStr));

    // implement the radio button interface
    void SetValue(bool value) override;
    bool GetValue() const override;

    // implementation only from now on
    bool MSWCommand(WXUINT param, WXWORD id) override;
    void Command(wxCommandEvent& event) override;

    bool HasTransparentBackground() override { return true; }

    WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;

protected:
    wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }
    wxSize DoGetBestSize() const override;

    // Implement wxMSWOwnerDrawnButtonBase methods.
    int MSWGetButtonStyle() const override;
    void MSWOnButtonResetOwnerDrawn() override;
    int MSWGetButtonCheckedFlag() const override;
    void
        MSWDrawButtonBitmap(wxDC& dc, const wxRect& rect, int flags) override;


private:    
    // we need to store the state internally as the result of GetValue()
    // sometimes gets out of sync in WM_COMMAND handler
    bool m_isChecked{false};

public:
	wxClassInfo *GetClassInfo() const override;
	static wxClassInfo ms_classInfo;
	static wxObject* wxCreateObject();
};

#endif // _WX_RADIOBUT_H_
