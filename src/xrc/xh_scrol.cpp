/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_scrol.cpp
// Purpose:     XRC resource for wxScrollBar
// Author:      Brian Gavin
// Created:     2000/09/09
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_SCROLLBAR

#include "wx/xrc/xh_scrol.h"

#ifndef WX_PRECOMP
    #include "wx/scrolbar.h"
#endif

wxIMPLEMENT_DYNAMIC_CLASS(wxScrollBarXmlHandler, wxXmlResourceHandler);

wxScrollBarXmlHandler::wxScrollBarXmlHandler()
 
{
    XRC_ADD_STYLE(wxSB_HORIZONTAL);
    XRC_ADD_STYLE(wxSB_VERTICAL);
    AddWindowStyles();
}

wxObject *wxScrollBarXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(control, wxScrollBar)

    control->Create(m_parentAsWindow,
                    GetID(),
                    GetPosition(), GetSize(),
                    GetStyle(),
                    wxDefaultValidator,
                    std::string(GetName()));

    control->SetScrollbar(GetLong( wxT("value"), 0),
                          GetLong( wxT("thumbsize"),1),
                          GetLong( wxT("range"), 10),
                          GetLong( wxT("pagesize"),1));

    SetupWindow(control);
    CreateChildren(control);

    return control;
}

bool wxScrollBarXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxScrollBar"));
}

#endif // wxUSE_XRC && wxUSE_SCROLLBAR
