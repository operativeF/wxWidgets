///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/gdiimage.h
// Purpose:     wxGDIImage class: base class for wxBitmap, wxIcon, wxCursor
//              under MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.11.99
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// NB: this is a private header, it is not intended to be directly included by
//     user code (but may be included from other, public, wxWin headers

#ifndef _WX_MSW_GDIIMAGE_H_
#define _WX_MSW_GDIIMAGE_H_

#include "wx/gdiobj.h"          // base class
#include "wx/gdicmn.h"          // wxBITMAP_TYPE_INVALID
#include "wx/list.h"

class WXDLLIMPEXP_FWD_CORE wxGDIImageRefData;
class WXDLLIMPEXP_FWD_CORE wxGDIImageHandler;
class WXDLLIMPEXP_FWD_CORE wxGDIImage;

WX_DECLARE_EXPORTED_LIST(wxGDIImageHandler, wxGDIImageHandlerList);

// ----------------------------------------------------------------------------
// wxGDIImageRefData: common data fields for all derived classes
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGDIImageRefData : public wxGDIRefData
{
public:
    wxGDIImageRefData()
    {
        m_handle = nullptr;
    }

    wxGDIImageRefData(const wxGDIImageRefData& data)  
    {
        m_width = data.m_width;
        m_height = data.m_height;
        m_depth = data.m_depth;

        // can't copy handles like this, derived class copy ctor must do it!
        m_handle = nullptr;
    }

    bool IsOk() const override { return m_handle != nullptr; }

    void SetSize(int w, int h) { m_width = w; m_height = h; }

    virtual void Free() = 0;

    // for compatibility, the member fields are public

    // the size of the image
    int m_width{0};
    int m_height{0};

    // the depth of the image
    int m_depth{0};

    // the handle to it
    union
    {
        WXHANDLE  m_handle;     // for untyped access
        WXHBITMAP m_hBitmap;
        WXHICON   m_hIcon;
        WXHCURSOR m_hCursor;
    };
};

// ----------------------------------------------------------------------------
// wxGDIImage: this class supports GDI image handlers which may be registered
// dynamically and will be used for loading/saving the images in the specified
// format. It also falls back to wxImage if no appropriate image is found.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGDIImage : public wxGDIObject
{
public:
    // handlers list interface
    static wxGDIImageHandlerList& GetHandlers() { return ms_handlers; }

    static void AddHandler(wxGDIImageHandler *handler);
    static void InsertHandler(wxGDIImageHandler *handler);
    static bool RemoveHandler(const wxString& name);

    static wxGDIImageHandler *FindHandler(const wxString& name);
    static wxGDIImageHandler *FindHandler(const wxString& extension, long type);
    static wxGDIImageHandler *FindHandler(long type);

    static void InitStandardHandlers();
    static void CleanUpHandlers();

    // access to the ref data casted to the right type
    wxGDIImageRefData *GetGDIImageData() const
        { return (wxGDIImageRefData *)m_refData; }

    WXHANDLE GetHandle() const
        { return IsNull() ? nullptr : GetGDIImageData()->m_handle; }
    void SetHandle(WXHANDLE handle)
        { AllocExclusive(); GetGDIImageData()->m_handle = handle; }

    int GetWidth() const { return IsNull() ? 0 : GetGDIImageData()->m_width; }
    int GetHeight() const { return IsNull() ? 0 : GetGDIImageData()->m_height; }
    int GetDepth() const { return IsNull() ? 0 : GetGDIImageData()->m_depth; }

    wxSize GetSize() const
    {
        return IsNull() ? wxSize(0,0) :
               wxSize(GetGDIImageData()->m_width, GetGDIImageData()->m_height);
    }

    // forward some of base class virtuals to wxGDIImageRefData
    bool FreeResource(bool force = false) override;
    WXHANDLE GetResourceHandle() const override;

protected:
    // create the data for the derived class here
    virtual wxGDIImageRefData *CreateData() const = 0;

    // implement the wxGDIObject method in terms of our, more specific, one
    wxGDIRefData *CreateGDIRefData() const override { return CreateData(); }

    // we can't [efficiently] clone objects of this class
    wxGDIRefData *
    CloneGDIRefData(const wxGDIRefData *WXUNUSED(data)) const override
    {
        wxFAIL_MSG( wxT("must be implemented if used") );

        return nullptr;
    }

    static wxGDIImageHandlerList ms_handlers;
};

// ----------------------------------------------------------------------------
// wxGDIImageHandler: a class which knows how to load/save wxGDIImages.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGDIImageHandler
{
public:
    wxGDIImageHandler() { m_type = wxBITMAP_TYPE_INVALID; }
    wxGDIImageHandler(const std::string& name,
                      const std::string& ext,
                      wxBitmapType type)
        : m_name(name), m_extension(ext), m_type(type) { }

    virtual ~wxGDIImageHandler() = default;
    
    void SetName(const std::string& name) { m_name = name; }
    void SetExtension(const std::string& ext) { m_extension = ext; }
    void SetType(wxBitmapType type) { m_type = type; }

    const std::string& GetName() const { return m_name; }
    const std::string& GetExtension() const { return m_extension; }
    wxBitmapType GetType() const { return m_type; }

    // real handler operations: to implement in derived classes
    [[maybe_unused]] virtual bool Create(wxGDIImage *image,
                        const void* data,
                        wxBitmapType flags,
                        int width, int height, int depth = 1) = 0;
    virtual bool Load(wxGDIImage *image,
                      std::string_view name,
                      wxBitmapType flags,
                      int desiredWidth, int desiredHeight) = 0;
    virtual bool Save(const wxGDIImage *image,
                      std::string_view name,
                      wxBitmapType type) const = 0;

protected:
    std::string  m_name;
    std::string  m_extension;

    wxBitmapType m_type;
};

#endif // _WX_MSW_GDIIMAGE_H_
