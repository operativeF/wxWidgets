///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/gdiimage.cpp
// Purpose:     wxGDIImage implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.11.99
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/msw/private.h"

#include "wx/msw/gdiimage.h"

#if wxUSE_WXDIB
#include "wx/msw/dib.h"
#endif

// By default, use PNG resource handler if we can, i.e. have support for
// loading PNG images in the library. This symbol could be predefined as 0 to
// avoid doing this if anybody ever needs to do it for some reason.
#if !defined(wxUSE_PNG_RESOURCE_HANDLER)
#define wxUSE_PNG_RESOURCE_HANDLER wxUSE_LIBPNG && wxUSE_IMAGE
#endif

#if wxUSE_PNG_RESOURCE_HANDLER
    #include "wx/image.h"
    #include "wx/utils.h"       // For wxLoadUserResource()
#endif

#include "wx/file.h"
#include "wx/stringutils.h"

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxGDIImageHandlerList)

#include <boost/nowide/convert.hpp>

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// all image handlers are declared/defined in this file because the outside
// world doesn't have to know about them (but only about wxBITMAP_TYPE_XXX ids)

class WXDLLEXPORT wxBMPFileHandler : public wxBitmapHandler
{
public:
    wxBMPFileHandler() : wxBitmapHandler("Windows bitmap file", "bmp",
                                         wxBITMAP_TYPE_BMP)
    {
    }

    bool LoadFile(wxBitmap *bitmap,
                          std::string_view name, wxBitmapType flags,
                          int desiredWidth, int desiredHeight) override;
    bool SaveFile(const wxBitmap *bitmap,
                          std::string_view name, wxBitmapType type,
                          const wxPalette *palette = nullptr) const override;
};

class WXDLLEXPORT wxBMPResourceHandler: public wxBitmapHandler
{
public:
    wxBMPResourceHandler() : wxBitmapHandler("Windows bitmap resource",
                                             "",
                                             wxBITMAP_TYPE_BMP_RESOURCE)
    {
    }

    bool LoadFile(wxBitmap *bitmap,
                          std::string_view name, wxBitmapType flags,
                          int desiredWidth, int desiredHeight) override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxBMPResourceHandler);
};

class WXDLLEXPORT wxIconHandler : public wxGDIImageHandler
{
public:
    wxIconHandler(const std::string& name, const std::string& ext, wxBitmapType type)
        : wxGDIImageHandler(name, ext, type)
    {
    }

    // creating and saving icons is not supported
    bool Create(wxGDIImage *WXUNUSED(image),
                        const void* WXUNUSED(data),
                        wxBitmapType WXUNUSED(flags),
                        int WXUNUSED(width),
                        int WXUNUSED(height),
                        int WXUNUSED(depth) = 1) override
    {
        return false;
    }

    bool Save(const wxGDIImage *WXUNUSED(image),
                      std::string_view WXUNUSED(name),
                      wxBitmapType WXUNUSED(type)) const override
    {
        return false;
    }

    bool Load(wxGDIImage *image,
                      std::string_view name,
                      wxBitmapType flags,
                      int desiredWidth, int desiredHeight) override
    {
        wxIcon *icon = wxDynamicCast(image, wxIcon);
        wxCHECK_MSG( icon, false, wxT("wxIconHandler only works with icons") );

        return LoadIcon(icon, name, flags, desiredWidth, desiredHeight);
    }

protected:
    virtual bool LoadIcon(wxIcon *icon,
                          std::string_view name, wxBitmapType flags,
                          int desiredWidth = -1, int desiredHeight = -1) = 0;
};

class WXDLLEXPORT wxICOFileHandler : public wxIconHandler
{
public:
    wxICOFileHandler() : wxIconHandler("ICO icon file",
                                       "ico",
                                       wxBITMAP_TYPE_ICO)
    {
    }

protected:
    bool LoadIcon(wxIcon *icon,
                          std::string_view name, wxBitmapType flags,
                          int desiredWidth = -1, int desiredHeight = -1) override;
};

class WXDLLEXPORT wxICOResourceHandler: public wxIconHandler
{
public:
    wxICOResourceHandler() : wxIconHandler("ICO resource",
                                           "ico",
                                           wxBITMAP_TYPE_ICO_RESOURCE)
    {
    }

protected:
    bool LoadIcon(wxIcon *icon,
                          std::string_view name, wxBitmapType flags,
                          int desiredWidth = -1, int desiredHeight = -1) override;
};

#if wxUSE_PNG_RESOURCE_HANDLER

class WXDLLEXPORT wxPNGResourceHandler : public wxBitmapHandler
{
public:
    wxPNGResourceHandler() : wxBitmapHandler("Windows PNG resource",
                                             std::string(),
                                             wxBITMAP_TYPE_PNG_RESOURCE)
    {
    }

    bool LoadFile(wxBitmap *bitmap,
                          std::string_view name, wxBitmapType flags,
                          int desiredWidth, int desiredHeight) override;
};

#endif // wxUSE_PNG_RESOURCE_HANDLER

// ============================================================================
// implementation
// ============================================================================

wxGDIImageHandlerList wxGDIImage::ms_handlers;

// ----------------------------------------------------------------------------
// wxGDIImage functions forwarded to wxGDIImageRefData
// ----------------------------------------------------------------------------

bool wxGDIImage::FreeResource(bool WXUNUSED(force))
{
    if ( !IsNull() )
    {
        GetGDIImageData()->Free();
        GetGDIImageData()->m_handle = nullptr;
    }

    return true;
}

WXHANDLE wxGDIImage::GetResourceHandle() const
{
    return GetHandle();
}

// ----------------------------------------------------------------------------
// wxGDIImage handler stuff
// ----------------------------------------------------------------------------

void wxGDIImage::AddHandler(wxGDIImageHandler *handler)
{
    ms_handlers.Append(handler);
}

void wxGDIImage::InsertHandler(wxGDIImageHandler *handler)
{
    ms_handlers.Insert(handler);
}

bool wxGDIImage::RemoveHandler(const wxString& name)
{
    wxGDIImageHandler *handler = FindHandler(name);
    if ( handler )
    {
        ms_handlers.DeleteObject(handler);
        return true;
    }
    else
        return false;
}

wxGDIImageHandler *wxGDIImage::FindHandler(const wxString& name)
{
    wxGDIImageHandlerList::compatibility_iterator node = ms_handlers.GetFirst();
    while ( node )
    {
        wxGDIImageHandler *handler = node->GetData();
        if ( handler->GetName() == name )
            return handler;
        node = node->GetNext();
    }

    return nullptr;
}

wxGDIImageHandler *wxGDIImage::FindHandler(const wxString& extension,
                                           long type)
{
    wxGDIImageHandlerList::compatibility_iterator node = ms_handlers.GetFirst();
    while ( node )
    {
        wxGDIImageHandler *handler = node->GetData();
        if ( (handler->GetExtension() == extension) &&
             (type == -1 || handler->GetType() == type) )
        {
            return handler;
        }

        node = node->GetNext();
    }
    return nullptr;
}

wxGDIImageHandler *wxGDIImage::FindHandler(long type)
{
    wxGDIImageHandlerList::compatibility_iterator node = ms_handlers.GetFirst();
    while ( node )
    {
        wxGDIImageHandler *handler = node->GetData();
        if ( handler->GetType() == type )
            return handler;

        node = node->GetNext();
    }

    return nullptr;
}

void wxGDIImage::CleanUpHandlers()
{
    wxGDIImageHandlerList::compatibility_iterator node = ms_handlers.GetFirst();
    while ( node )
    {
        wxGDIImageHandler *handler = node->GetData();
        wxGDIImageHandlerList::compatibility_iterator next = node->GetNext();
        delete handler;
        ms_handlers.Erase( node );
        node = next;
    }
}

void wxGDIImage::InitStandardHandlers()
{
    AddHandler(new wxBMPResourceHandler);
    AddHandler(new wxBMPFileHandler);
    AddHandler(new wxICOFileHandler);
    AddHandler(new wxICOResourceHandler);
#if wxUSE_PNG_RESOURCE_HANDLER
    AddHandler(new wxPNGResourceHandler);
#endif // wxUSE_PNG_RESOURCE_HANDLER
}

// ----------------------------------------------------------------------------
// wxBitmap handlers
// ----------------------------------------------------------------------------

bool wxBMPResourceHandler::LoadFile(wxBitmap *bitmap,
                                    std::string_view name, wxBitmapType WXUNUSED(flags),
                                    int WXUNUSED(desiredWidth),
                                    int WXUNUSED(desiredHeight))
{
    // TODO: load colourmap.
    HBITMAP hbmp = ::LoadBitmapW(wxGetInstance(), boost::nowide::widen(name).c_str());
    if ( hbmp == nullptr )
    {
        // it's probably not found
        wxLogError(wxT("Can't load bitmap '%s' from resources! Check .rc file."),
            std::string(name));

        return false;
    }

    int w, h, d;
    BITMAP bm;
    if (::GetObjectW(hbmp, sizeof(BITMAP), &bm))
    {
        w = bm.bmWidth;
        h = bm.bmHeight;
        d = bm.bmBitsPixel;
    }
    else
    {
        wxLogLastError(wxT("GetObject(HBITMAP)"));
        w = h = d = 0;
    }

    bitmap->InitFromHBITMAP((WXHBITMAP)hbmp, w, h, d);

    // use 0xc0c0c0 as transparent colour by default
    bitmap->SetMask(new wxMask(*bitmap, *wxLIGHT_GREY));

    return true;
}

bool wxBMPFileHandler::LoadFile(wxBitmap *bitmap,
                                std::string_view name, wxBitmapType WXUNUSED(flags),
                                int WXUNUSED(desiredWidth),
                                int WXUNUSED(desiredHeight))
{
    wxCHECK_MSG( bitmap, false, wxT("NULL bitmap in LoadFile") );

#if wxUSE_WXDIB
    // Try loading using native Windows LoadImage() first.
    // FIXME: Widen string at the constructor wxDIB, not here.
    wxDIB dib(name);
    if ( dib.IsOk() )
        return bitmap->CopyFromDIB(dib);
#endif // wxUSE_WXDIB

    // Some valid bitmap files are not supported by LoadImage(), e.g. those
    // with negative height. Try to use our own bitmap loading code which does
    // support them.
#if wxUSE_IMAGE
    // FIXME: Widen string at the constructor wxImage, not here.
    wxImage img(boost::nowide::widen(name).c_str(), wxBITMAP_TYPE_BMP);
    if ( img.IsOk() )
    {
        *bitmap = wxBitmap(img);
        return true;
    }
#endif // wxUSE_IMAGE

    return false;
}

bool wxBMPFileHandler::SaveFile(const wxBitmap *bitmap,
                                std::string_view name,
                                wxBitmapType WXUNUSED(type),
                                const wxPalette * WXUNUSED(pal)) const
{
#if wxUSE_WXDIB
    wxCHECK_MSG( bitmap, false, wxT("NULL bitmap in SaveFile") );

    wxDIB dib(*bitmap);

    return dib.Save(name);
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------
// wxIcon handlers
// ----------------------------------------------------------------------------

bool wxICOFileHandler::LoadIcon(wxIcon *icon,
                                std::string_view name,
                                wxBitmapType WXUNUSED(flags),
                                int desiredWidth, int desiredHeight)
{
    icon->UnRef();

    HICON hicon = nullptr;

    // Parse the filename: it may be of the form "filename;n" in order to
    // specify the nth icon in the file.
    //
    // For the moment, ignore the issue of possible semicolons in the
    // filename.
    int iconIndex = 0;
    std::string nameReal(name);
    std::string_view strIconIndex = wx::utils::AfterLast(name, ';');

    if (strIconIndex != name)
    {
        iconIndex = std::atoi(strIconIndex.data());
        nameReal = wx::utils::BeforeLast(name, ';');
    }

#if 0
    // If we don't know what size icon we're looking for,
    // try to find out what's there.
    // Unfortunately this doesn't work, because ExtractIconEx
    // will scale the icon to the 'desired' size, even if that
    // size of icon isn't explicitly stored. So we would have
    // to parse the icon file outselves.
    if ( desiredWidth == -1 &&
         desiredHeight == -1)
    {
        // Try loading a large icon first
        if ( ::ExtractIconEx(nameReal, iconIndex, &hicon, NULL, 1) == 1)
        {
        }
        // Then try loading a small icon
        else if ( ::ExtractIconEx(nameReal, iconIndex, NULL, &hicon, 1) == 1)
        {
        }
    }
    else
#endif
    // were we asked for a large icon?
    const wxWindow* win = wxApp::GetMainTopWindow();
    if ( desiredWidth == wxGetSystemMetrics(SM_CXICON, win) &&
         desiredHeight == wxGetSystemMetrics(SM_CYICON, win) )
    {
        // get the specified large icon from file
        if ( !::ExtractIconExW(boost::nowide::widen(nameReal).c_str(), iconIndex, &hicon, nullptr, 1) )
        {
            // it is not an error, but it might still be useful to be informed
            // about it optionally
            wxLogTrace(wxT("iconload"),
                       wxT("No large icons found in the file '%s'."),
                       std::string(name));
        }
    }
    else if ( desiredWidth == wxGetSystemMetrics(SM_CXSMICON, win) &&
              desiredHeight == wxGetSystemMetrics(SM_CYSMICON, win) )
    {
        // get the specified small icon from file
        if ( !::ExtractIconExW(boost::nowide::widen(nameReal).c_str(), iconIndex, nullptr, &hicon, 1) )
        {
            wxLogTrace(wxT("iconload"),
                       wxT("No small icons found in the file '%s'."),
                       std::string(name));
        }
    }
    //else: not standard size, load below

    if ( !hicon )
    {
        // take any size icon from the file by index
        hicon = ::ExtractIconW(wxGetInstance(), boost::nowide::widen(nameReal).c_str(), iconIndex);
    }

    if ( !hicon )
    {
        wxLogSysError(wxT("Failed to load icon from the file '%s'"),
                      std::string(name));

        return false;
    }

    if ( !icon->CreateFromHICON(hicon) )
        return false;

    if ( (desiredWidth != -1 && desiredWidth != icon->GetWidth()) ||
         (desiredHeight != -1 && desiredHeight != icon->GetHeight()) )
    {
        wxLogTrace(wxT("iconload"),
                   wxT("Returning false from wxICOFileHandler::Load because of the size mismatch: actual (%d, %d), requested (%d, %d)"),
                   icon->GetWidth(), icon->GetHeight(),
                   desiredWidth, desiredHeight);

        icon->UnRef();

        return false;
    }

    return true;
}

bool wxICOResourceHandler::LoadIcon(wxIcon *icon,
                                    std::string_view name,
                                    wxBitmapType WXUNUSED(flags),
                                    int desiredWidth, int desiredHeight)
{
    // do we need the icon of the specific size or would any icon do?
    bool hasSize = desiredWidth != -1 || desiredHeight != -1;

    wxASSERT_MSG( !hasSize || (desiredWidth != -1 && desiredHeight != -1),
                  wxT("width and height should be either both -1 or not") );

    // try to load the icon from this program first to allow overriding the
    // standard icons (although why one would want to do it considering that
    // we already have wxApp::GetStdIcon() is unclear)

    // note that we can't just always call LoadImage() because it seems to do
    // some icon rescaling internally which results in very ugly 16x16 icons
    HICON hicon = [=]() {
        if ( hasSize )
        {
            return (HICON)::LoadImageW(wxGetInstance(), boost::nowide::widen(name).c_str(), IMAGE_ICON,
                                        desiredWidth, desiredHeight,
                                        LR_DEFAULTCOLOR);
        }
        else
        {
            return ::LoadIconW(wxGetInstance(), boost::nowide::widen(name).c_str());
        }
    }();

    // next check if it's not a standard icon
    if ( !hicon && !hasSize )
    {
        static const struct
        {
            std::string_view name;
            LPWSTR id;
        } stdIcons[] =
        {
            { "wxICON_QUESTION",     IDI_QUESTION    },
            { "wxICON_WARNING",      IDI_EXCLAMATION },
            { "wxICON_ERROR",        IDI_HAND        },
            { "wxICON_INFORMATION",  IDI_ASTERISK    },
        };

        for ( size_t nIcon = 0; !hicon && nIcon < WXSIZEOF(stdIcons); nIcon++ )
        {
            if ( name == stdIcons[nIcon].name )
            {
                hicon = ::LoadIconW((HINSTANCE)nullptr, stdIcons[nIcon].id);
                break;
            }
        }
    }

    return icon->CreateFromHICON((WXHICON)hicon);
}

#if wxUSE_PNG_RESOURCE_HANDLER

// ----------------------------------------------------------------------------
// PNG handler
// ----------------------------------------------------------------------------

bool wxPNGResourceHandler::LoadFile(wxBitmap *bitmap,
                                    std::string_view name,
                                    wxBitmapType WXUNUSED(flags),
                                    int WXUNUSED(desiredWidth),
                                    int WXUNUSED(desiredHeight))
{
    const void* pngData = nullptr;
    size_t pngSize = 0;

    // Currently we hardcode RCDATA resource type as this is what is usually
    // used for the embedded images. We could allow specifying the type as part
    // of the name in the future (e.g. "type:name" or something like this) if
    // really needed.
    // FIXME: Change function to widen at function.
    if ( !wxLoadUserResource(&pngData, &pngSize,
                             boost::nowide::widen(name),
                             RT_RCDATA,
                             wxGetInstance()) )
    {
        // Notice that this message is not translated because only the
        // programmer (and not the end user) can make any use of it.
        wxLogError(wxS("Bitmap in PNG format \"%s\" not found, check ")
                   wxS("that the resource file contains \"RCDATA\" ")
                   wxS("resource with this name."),
                   std::string(name));

        return false;
    }

    *bitmap = wxBitmap::NewFromPNGData(pngData, pngSize);
    if ( !bitmap->IsOk() )
    {
        wxLogError(wxS("Couldn't load resource bitmap \"%s\" as a PNG. ")
                   wxS("Have you registered PNG image handler?"),
                   std::string(name));

        return false;
    }

    return true;
}

#endif // wxUSE_PNG_RESOURCE_HANDLER

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

wxSize wxGetHiconSize(HICON hicon)
{
    wxSize size;

    if ( hicon )
    {
        AutoIconInfo info;
        if ( info.GetFrom(hicon) )
        {
            HBITMAP hbmp = info.hbmMask;
            if ( hbmp )
            {
                BITMAP bm;
                if ( ::GetObjectW(hbmp, sizeof(BITMAP), (LPWSTR) &bm) )
                {
                    size = wxSize(bm.bmWidth, bm.bmHeight);
                }
            }
            // For monochrome icon reported height is doubled
            // because it contains both AND and XOR masks.
            if ( info.hbmColor == nullptr )
            {
                size.y /= 2;
            }
        }
    }

    if ( !size.x )
    {
        // use default icon size on this hardware
        const wxWindow* win = wxApp::GetMainTopWindow();
        size.x = wxGetSystemMetrics(SM_CXICON, win);
        size.y = wxGetSystemMetrics(SM_CYICON, win);
    }

    return size;
}
