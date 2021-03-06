/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/helpbest.h
// Purpose:     Tries to load MS HTML Help, falls back to wxHTML upon failure
// Author:      Mattia Barbon
// Modified by:
// Created:     02/04/2001
// Copyright:   (c) Mattia Barbon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPBEST_H_
#define _WX_HELPBEST_H_

#if wxUSE_HELP && wxUSE_MS_HTML_HELP \
    && wxUSE_WXHTML_HELP && !defined(__WXUNIVERSAL__)

#include "wx/helpbase.h"
#include "wx/html/helpfrm.h"        // for wxHF_DEFAULT_STYLE

class WXDLLIMPEXP_HTML wxBestHelpController: public wxHelpControllerBase
{
public:
    wxBestHelpController(wxWindow* parentWindow = nullptr,
                         int style = wxHF_DEFAULT_STYLE)
        : wxHelpControllerBase(parentWindow),
          m_style(style)
    {
    }

    ~wxBestHelpController() override { delete m_helpController; }

    wxBestHelpController(const wxBestHelpController&) = delete;
    wxBestHelpController& operator=(const wxBestHelpController&) = delete;
    wxBestHelpController(wxBestHelpController&&) = default;
    wxBestHelpController& operator=(wxBestHelpController&&) = default;

    // Must call this to set the filename
    bool Initialize(const wxString& file) override;
    bool Initialize(const wxString& file, int WXUNUSED(server) ) override { return Initialize( file ); }

    // If file is "", reloads file given in Initialize
    bool LoadFile(const wxString& file = wxEmptyString) override
    {
        return m_helpController->LoadFile( GetValidFilename( file ) );
    }

    bool DisplayContents() override
    {
        return m_helpController->DisplayContents();
    }

    bool DisplaySection(int sectionNo) override
    {
        return m_helpController->DisplaySection( sectionNo );
    }

    bool DisplaySection(const wxString& section) override
    {
        return m_helpController->DisplaySection( section );
    }

    bool DisplayBlock(long blockNo) override
    {
        return m_helpController->DisplayBlock( blockNo );
    }

    bool DisplayContextPopup(int contextId) override
    {
        return m_helpController->DisplayContextPopup( contextId );
    }

    bool DisplayTextPopup(const wxString& text, const wxPoint& pos) override
    {
        return m_helpController->DisplayTextPopup( text, pos );
    }

    bool KeywordSearch(const wxString& k,
                               wxHelpSearchMode mode = wxHELP_SEARCH_ALL) override
    {
        return m_helpController->KeywordSearch( k, mode );
    }

    bool Quit() override
    {
        return m_helpController->Quit();
    }

    // Allows one to override the default settings for the help frame.
    void SetFrameParameters(const wxString& title,
                                    const wxSize& size,
                                    const wxPoint& pos = wxDefaultPosition,
                                    bool newFrameEachTime = false) override
    {
        m_helpController->SetFrameParameters( title, size, pos,
                                              newFrameEachTime );
    }

    // Obtains the latest settings used by the help frame and the help frame.
    wxFrame *GetFrameParameters(wxSize *size = nullptr,
                                        wxPoint *pos = nullptr,
                                        bool *newFrameEachTime = nullptr) override
    {
        return m_helpController->GetFrameParameters( size, pos,
                                                     newFrameEachTime );
    }

    /// Set the window that can optionally be used for the help window's parent.
    void SetParentWindow(wxWindow* win) override { m_helpController->SetParentWindow(win); }

    /// Get the window that can optionally be used for the help window's parent.
    wxWindow* GetParentWindow() const override { return m_helpController->GetParentWindow(); }

protected:
    // Append/change extension if necessary.
    wxString GetValidFilename(const wxString& file) const;

protected:
    enum HelpControllerType { wxUseNone, wxUseHtmlHelp, wxUseChmHelp };

    HelpControllerType m_helpControllerType{wxUseNone};
    wxHelpControllerBase* m_helpController{nullptr};
    int m_style;

    wxDECLARE_DYNAMIC_CLASS(wxBestHelpController);
};

#endif // wxUSE_HELP && wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP

#endif
    // _WX_HELPBEST_H_
