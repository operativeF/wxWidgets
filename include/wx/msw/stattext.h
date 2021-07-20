/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/stattext.h
// Purpose:     wxStaticText class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATTEXT_H_
#define _WX_STATTEXT_H_

class WXDLLIMPEXP_CORE wxStaticText : public wxStaticTextBase
{
public:
    wxStaticText() = default;

    wxStaticText(wxWindow *parent,
                 wxWindowID id,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 std::string_view name = wxStaticTextNameStr)
    {
        Create(parent, id, label, pos, size, style, name);
    }

    wxStaticText(const wxStaticText&) = delete;
    wxStaticText& operator=(const wxStaticText&) = delete;
    wxStaticText(wxStaticText&&) = default;
    wxStaticText& operator=(wxStaticText&&) = default;

    [[maybe_unused]] bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                std::string_view name = wxStaticTextNameStr);

    // override some methods to resize the window properly
    void SetLabel(const wxString& label) override;
    bool SetFont( const wxFont &font ) override;

    WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle = nullptr) const override;

protected:
    // implement/override some base class virtuals
    void DoSetSize(int x, int y, int w, int h,
                           int sizeFlags = wxSIZE_AUTO) override;
    wxSize DoGetBestClientSize() const override;

    wxString WXGetVisibleLabel() const override;
    void WXSetVisibleLabel(const wxString& str) override;

public:
	wxClassInfo *GetClassInfo() const override;
	static wxClassInfo ms_classInfo;
	static wxObject* wxCreateObject();
};

#endif
    // _WX_STATTEXT_H_
