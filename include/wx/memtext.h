///////////////////////////////////////////////////////////////////////////////
// Name:        wx/memtext.h
// Purpose:     wxMemoryText allows to use wxTextBuffer without a file
// Created:     14.11.01
// Author:      Morten Hanssen
// Copyright:   (c) 2001 Morten Hanssen
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MEMTEXT_H
#define _WX_MEMTEXT_H

#include "wx/defs.h"

// there is no separate setting for wxMemoryText, it's smallish anyhow
#if wxUSE_TEXTBUFFER

class WXDLLIMPEXP_BASE wxMemoryText : public wxTextBuffer
{
public:
    wxMemoryText() = default;
    wxMemoryText(const wxString& name) : wxTextBuffer(name) { }

    wxMemoryText(const wxMemoryText&) = delete;
    wxMemoryText& operator=(const wxMemoryText&) = delete;
    wxMemoryText(wxMemoryText&&) = default;
    wxMemoryText& operator=(wxMemoryText&&) = default;

protected:
    bool OnExists() const override
        { return false; }

    bool OnOpen(const wxString & WXUNUSED(strBufferName),
                        wxTextBufferOpenMode WXUNUSED(OpenMode)) override
        { return true; }

    bool OnClose() override
        { return true; }

    bool OnRead(const wxMBConv& WXUNUSED(conv)) override
        { return true; }

    bool OnWrite(wxTextFileType WXUNUSED(typeNew),
                         const wxMBConv& WXUNUSED(conv) = wxMBConvUTF8()) override
        { return true; }
};

#endif // wxUSE_TEXTBUFFER

#endif // _WX_MEMTEXT_H

