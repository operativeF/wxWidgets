/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dcgraph.cpp
// Purpose:     graphics context methods common to all platforms
// Author:      Stefan Csomor
// Modified by:
// Created:
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_GRAPHICS_CONTEXT

#include "wx/dcgraph.h"

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/math.h"
    #include "wx/geometry.h"
#endif

//-----------------------------------------------------------------------------
// Local functions
//-----------------------------------------------------------------------------

static wxCompositionMode TranslateRasterOp(wxRasterOperationMode function)
{
    switch ( function )
    {
        case wxRasterOperationMode::Copy: // src
            // since we are supporting alpha, _OVER is closer to the intention than _SOURCE
            // since the latter would overwrite even when alpha is not set to opaque
            return wxCOMPOSITION_OVER;

        case wxRasterOperationMode::Or:         // src OR dst
            return wxCOMPOSITION_ADD;

        case wxRasterOperationMode::NoOp:      // dst
            return wxCOMPOSITION_DEST; // ignore the source

        case wxRasterOperationMode::Clear:      // 0
            return wxCOMPOSITION_CLEAR;// clear dst

        case wxRasterOperationMode::Xor:        // src XOR dst
            return wxCOMPOSITION_XOR;

        case wxRasterOperationMode::And:        // src AND dst
        case wxRasterOperationMode::AndInvert: // (NOT src) AND dst
        case wxRasterOperationMode::AndReverse:// src AND (NOT dst)
        case wxRasterOperationMode::Equiv:      // (NOT src) XOR dst
        case wxRasterOperationMode::Invert:     // NOT dst
        case wxRasterOperationMode::Nand:       // (NOT src) OR (NOT dst)
        case wxRasterOperationMode::Nor:        // (NOT src) AND (NOT dst)
        case wxRasterOperationMode::OrInvert:  // (NOT src) OR dst
        case wxRasterOperationMode::OrReverse: // src OR (NOT dst)
        case wxRasterOperationMode::Set:        // 1
        case wxRasterOperationMode::SrcInvert: // NOT src
            break;
    }

    return wxCOMPOSITION_INVALID;
}

//-----------------------------------------------------------------------------
// wxDC bridge class
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxGCDC, wxDC);

wxGCDC::wxGCDC(const wxWindowDC& dc) :
  wxDC( new wxGCDCImpl( this, dc ) )
{
}

wxGCDC::wxGCDC( const wxMemoryDC& dc) :
  wxDC( new wxGCDCImpl( this, dc ) )
{
}

#if wxUSE_PRINTING_ARCHITECTURE
wxGCDC::wxGCDC( const wxPrinterDC& dc) :
  wxDC( new wxGCDCImpl( this, dc ) )
{
}
#endif

#if defined(__WXMSW__) && wxUSE_ENH_METAFILE
wxGCDC::wxGCDC(const wxEnhMetaFileDC& dc)
   : wxDC(new wxGCDCImpl(this, dc))
{
}
#endif

wxGCDC::wxGCDC(wxGraphicsContext* context) :
    wxDC(new wxGCDCImpl(this, context))
{
}

wxGCDC::wxGCDC() :
  wxDC( new wxGCDCImpl( this ) )
{
}

wxIMPLEMENT_ABSTRACT_CLASS(wxGCDCImpl, wxDCImpl);

wxGCDCImpl::wxGCDCImpl(wxDC *owner, wxGraphicsContext* context) :
    wxDCImpl(owner)
{
    CommonInit();

    DoInitContext(context);

    // We can't currently initialize m_font, m_pen and m_brush here as we don't
    // have any way of converting the corresponding wxGraphicsXXX objects to
    // plain wxXXX ones. This is obviously not ideal as it means that GetXXX()
    // won't return the actual object being used, but is better than the only
    // alternative which is overwriting the objects currently used in the
    // graphics context with the defaults.
}

wxGCDCImpl::wxGCDCImpl( wxDC *owner ) :
   wxDCImpl( owner )
{
    Init(wxGraphicsContext::Create());
}

void wxGCDCImpl::SetGraphicsContext( wxGraphicsContext* ctx )
{
    delete m_graphicContext;

    if ( DoInitContext(ctx) )
    {
        // Reapply our attributes to the context.
        m_graphicContext->SetFont( m_font , m_textForegroundColour );
        m_graphicContext->SetPen( m_pen );
        m_graphicContext->SetBrush( m_brush);
    }
}

wxGCDCImpl::wxGCDCImpl( wxDC *owner, const wxWindowDC& dc ) :
   wxDCImpl( owner )
{
    Init(wxGraphicsContext::Create(dc));
    m_window = dc.GetWindow();
}

wxGCDCImpl::wxGCDCImpl( wxDC *owner, const wxMemoryDC& dc ) :
   wxDCImpl( owner )
{
    Init(wxGraphicsContext::Create(dc));
}

#if wxUSE_PRINTING_ARCHITECTURE
wxGCDCImpl::wxGCDCImpl( wxDC *owner, const wxPrinterDC& dc ) :
   wxDCImpl( owner )
{
    Init(wxGraphicsContext::Create(dc));
}
#endif

#if defined(__WXMSW__) && wxUSE_ENH_METAFILE
wxGCDCImpl::wxGCDCImpl(wxDC *owner, const wxEnhMetaFileDC& dc)
   : wxDCImpl(owner)
{
    Init(wxGraphicsContext::Create(dc));
}
#endif

wxGCDCImpl::wxGCDCImpl(wxDC* owner, int)
   : wxDCImpl(owner)
{
    // derived class will set a context
    Init(nullptr);
}

void wxGCDCImpl::CommonInit()
{
    m_mm_to_pix_x = mm2pt;
    m_mm_to_pix_y = mm2pt;

    m_isClipBoxValid = false;

    m_logicalFunctionSupported = true;
}

void wxGCDCImpl::Init(wxGraphicsContext* ctx)
{
    CommonInit();

    m_ok = false;

    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;

    m_graphicContext = nullptr;
    if (ctx)
        SetGraphicsContext(ctx);
}

bool wxGCDCImpl::DoInitContext(wxGraphicsContext* ctx)
{
    m_graphicContext = ctx;
    m_ok = m_graphicContext != nullptr;

    if ( m_ok )
    {
        // apply the stored transformations to the passed in context
        m_matrixOriginal = m_graphicContext->GetTransform();
        ComputeScaleAndOrigin();
    }

    return m_ok;
}

wxGCDCImpl::~wxGCDCImpl()
{
    delete m_graphicContext;
}

void wxGCDCImpl::DoDrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y,
                               bool useMask )
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawBitmap - invalid DC") );
    wxCHECK_RET( bmp.IsOk(), wxT("wxGCDC(cg)::DoDrawBitmap - invalid bitmap") );

    int w = bmp.GetScaledWidth();
    int h = bmp.GetScaledHeight();
    if ( bmp.GetDepth() == 1 )
    {
        m_graphicContext->SetPen(*wxTRANSPARENT_PEN);
        m_graphicContext->SetBrush(m_textBackgroundColour);
        m_graphicContext->DrawRectangle( x, y, w, h );
        m_graphicContext->SetBrush(m_textForegroundColour);
        m_graphicContext->DrawBitmap( bmp, x, y, w, h );
        m_graphicContext->SetBrush( m_graphicContext->CreateBrush(m_brush));
        m_graphicContext->SetPen( m_graphicContext->CreatePen(m_pen));
    }
    else // not a monochrome bitmap, handle it normally
    {
        // make a copy in case we need to remove its mask, if we don't modify
        // it the copy is cheap as bitmaps are reference-counted
        wxBitmap bmpCopy(bmp);
        if ( !useMask && bmp.GetMask() )
            bmpCopy.SetMask(nullptr);

        m_graphicContext->DrawBitmap( bmpCopy, x, y, w, h );
    }

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + w, y + h);
}

void wxGCDCImpl::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y )
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawIcon - invalid DC") );
    wxCHECK_RET( icon.IsOk(), wxT("wxGCDC(cg)::DoDrawIcon - invalid icon") );

    wxCoord w = icon.GetWidth();
    wxCoord h = icon.GetHeight();

    m_graphicContext->DrawIcon( icon , x, y, w, h );

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + w, y + h);
}

bool wxGCDCImpl::StartDoc( const wxString& message )
{
    return m_graphicContext->StartDoc(message);
}

void wxGCDCImpl::EndDoc()
{
    m_graphicContext->EndDoc();
}

void wxGCDCImpl::StartPage()
{
    m_graphicContext->StartPage();
}

void wxGCDCImpl::EndPage()
{
    m_graphicContext->EndPage();
}

void wxGCDCImpl::Flush()
{
    m_graphicContext->Flush();
}

void wxGCDCImpl::UpdateClipBox()
{
    // TODO: Return rect
    double x, y, w, h;
    m_graphicContext->GetClipBox(&x, &y, &w, &h);

    // We shouldn't reset m_clipping if the clipping region that we set happens
    // to be empty (e.g. because its intersection with the previous clipping
    // region was empty), but we should set it to true if we do have a valid
    // clipping region and it was false which may happen if the clipping region
    // set from the outside of wxWidgets code.
    if ( !m_clipping )
    {
        if ( w != 0. && h != 0. )
            m_clipping = true;
    }

    m_clipX1 = wxRound(x);
    m_clipY1 = wxRound(y);
    m_clipX2 = wxRound(x+w);
    m_clipY2 = wxRound(y+h);
    m_isClipBoxValid = true;
}

bool wxGCDCImpl::DoGetClippingRect(wxRect& rect) const
{
    wxCHECK_MSG( IsOk(), false, wxS("wxGCDC::DoGetClippingRegion - invalid GC") );
    // Check if we should retrieve the clipping region possibly not set
    // by SetClippingRegion() but modified by application: this can
    // happen when we're associated with an existing graphics context using
    // SetGraphicsContext() or when wxGCDC logical coordinates are transformed
    // with SetDeviceOrigin(), SetLogicalOrigin(), SetUserScale(), SetLogicalScale().
    if ( !m_isClipBoxValid )
    {
        wxGCDCImpl *self = const_cast<wxGCDCImpl *>(this);
        self->UpdateClipBox();
    }

    return wxDCImpl::DoGetClippingRect(rect);
}

void wxGCDCImpl::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord w, wxCoord h )
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoSetClippingRegion - invalid DC") );

    // Generally, renderers accept negative values of width/height
    // but for internal calculations we need to have a box definition
    // in the standard form, with (x,y) pointing to the top-left
    // corner of the box and with non-negative width and height.
    if ( w < 0 )
    {
        w = -w;
        x -= (w - 1);
    }
    if ( h < 0 )
    {
        h = -h;
        y -= (h - 1);
    }
    m_graphicContext->Clip( x, y, w, h );

    m_clipping = true;
    UpdateClipBox();
}

void wxGCDCImpl::DoSetDeviceClippingRegion( const wxRegion &region )
{
    // region is in device coordinates
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoSetDeviceClippingRegion - invalid DC") );

    // Because graphics context works with logical coordinates
    // and clipping region is given in device coordinates
    // we need temporarily reset graphics context's coordinate system
    // to the initial state in which logical and device coordinate
    // systems are equivalent.
    // So, at first save current transformation parameters.
    wxGraphicsMatrix currTransform = m_graphicContext->GetTransform();
    // Reset coordinate system with identity transformation matrix
    // to make logical coordinates the same as device coordinates.
    wxGraphicsMatrix m = m_graphicContext->CreateMatrix();
    m_graphicContext->SetTransform(m);

    // Set clipping region
    m_graphicContext->Clip(region);

    // Restore original transformation settings.
    m_graphicContext->SetTransform(currTransform);

    m_clipping = true;
    UpdateClipBox();
}

void wxGCDCImpl::DestroyClippingRegion()
{
    m_graphicContext->ResetClip();
    // currently the clip eg of a window extends to the area between the scrollbars
    // so we must explicitly make sure it only covers the area we want it to draw
    const wxSize sz = GetOwner()->GetSize() ;
    wxPoint origin;
#ifdef __WXOSX__
    origin = OSXGetOrigin();
#endif
    m_graphicContext->Clip( DeviceToLogicalX(origin.x) , DeviceToLogicalY(origin.y) , DeviceToLogicalXRel(sz.x), DeviceToLogicalYRel(sz.y) );

    m_graphicContext->SetPen( m_pen );
    m_graphicContext->SetBrush( m_brush );

    wxDCImpl::DestroyClippingRegion();
    m_isClipBoxValid = false;
}

void wxGCDCImpl::DoGetSizeMM( int* width, int* height ) const
{
    wxSize sz = GetOwner()->GetSize();

    if (width)
        *width = long( double(sz.x) / (m_scale.x * GetMMToPXx()) );
    if (height)
        *height = long( double(sz.y) / (m_scale.y * GetMMToPXy()) );
}

void wxGCDCImpl::SetTextForeground( const wxColour &col )
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::SetTextForeground - invalid DC") );

    // don't set m_textForegroundColour to an invalid colour as we'd crash
    // later then (we use m_textForegroundColour.GetColor() without checking
    // in a few places)
    if ( col.IsOk() )
    {
        m_textForegroundColour = col;
        m_graphicContext->SetFont( m_font, m_textForegroundColour );
    }
}

void wxGCDCImpl::SetTextBackground( const wxColour &col )
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::SetTextBackground - invalid DC") );

    m_textBackgroundColour = col;
}

wxSize wxGCDCImpl::GetPPI() const
{
    if ( m_graphicContext )
    {
        double x, y;
        m_graphicContext->GetDPI(&x, &y);
        return {wxRound(x), wxRound(y)};
    }

    // This is the same value that wxGraphicsContext::GetDPI() returns by
    // default.
    return {72, 72};
}

int wxGCDCImpl::GetDepth() const
{
    return 32;
}

void wxGCDCImpl::ComputeScaleAndOrigin()
{
    wxDCImpl::ComputeScaleAndOrigin();

    if ( m_graphicContext )
    {
        m_matrixCurrent = m_graphicContext->CreateMatrix();

        // the logical origin sets the origin to have new coordinates
        m_matrixCurrent.Translate( m_deviceOrigin.x - m_logicalOrigin.x * m_signX * m_scale.x,
                                   m_deviceOrigin.y - m_logicalOrigin.y * m_signY * m_scale.y);

        m_matrixCurrent.Scale( m_scale.x * m_signX, m_scale.y * m_signY );

        m_graphicContext->SetTransform( m_matrixOriginal );
#if wxUSE_DC_TRANSFORM_MATRIX
        // Concatenate extended transform (affine) with basic transform of coordinate system.
        wxGraphicsMatrix mtxExt = m_graphicContext->CreateMatrix(m_matrixExtTransform);
        m_matrixCurrent.Concat(mtxExt);
#endif // wxUSE_DC_TRANSFORM_MATRIX
        m_graphicContext->ConcatTransform( m_matrixCurrent );
        m_matrixCurrentInv = m_matrixCurrent;
        m_matrixCurrentInv.Invert();
        m_isClipBoxValid = false;
    }
}

void* wxGCDCImpl::GetHandle() const
{
    void* cgctx = nullptr;
    wxGraphicsContext* gc = GetGraphicsContext();
    if (gc) {
        cgctx = gc->GetNativeContext();
    }
    return cgctx;
}

#if wxUSE_PALETTE
void wxGCDCImpl::SetPalette( const wxPalette& WXUNUSED(palette) )
{

}
#endif

void wxGCDCImpl::SetBackgroundMode( wxBrushStyle mode )
{
    m_backgroundMode = mode;
}

void wxGCDCImpl::SetFont( const wxFont &font )
{
    m_font = font;
    if ( m_graphicContext )
    {
        m_graphicContext->SetFont(font, m_textForegroundColour);
    }
}

void wxGCDCImpl::SetPen( const wxPen &pen )
{
    m_pen = pen;
    if ( m_graphicContext )
    {
        m_graphicContext->SetPen( m_pen );
    }
}

void wxGCDCImpl::SetBrush( const wxBrush &brush )
{
    m_brush = brush;
    if ( m_graphicContext )
    {
        m_graphicContext->SetBrush( m_brush );
    }
}

void wxGCDCImpl::SetBackground( const wxBrush &brush )
{
    m_backgroundBrush = brush;
}

void wxGCDCImpl::SetLogicalFunction( wxRasterOperationMode function )
{
    m_logicalFunction = function;

    wxCompositionMode mode = TranslateRasterOp( function );
    m_logicalFunctionSupported = mode != wxCOMPOSITION_INVALID;
    if (m_logicalFunctionSupported)
        m_logicalFunctionSupported = m_graphicContext->SetCompositionMode(mode);

    if ( function == wxRasterOperationMode::Xor )
        m_graphicContext->SetAntialiasMode(wxANTIALIAS_NONE);
    else
        m_graphicContext->SetAntialiasMode(wxANTIALIAS_DEFAULT);
}

// ----------------------------------------------------------------------------
// Transform matrix
// ----------------------------------------------------------------------------

#if wxUSE_DC_TRANSFORM_MATRIX

bool wxGCDCImpl::CanUseTransformMatrix() const
{
    return true;
}

bool wxGCDCImpl::SetTransformMatrix(const wxAffineMatrix2D &matrix)
{
    // Passed affine transform will be concatenated
    // with current basic transform of the coordinate system.
    m_matrixExtTransform = matrix;
    ComputeScaleAndOrigin();
    return true;
}

wxAffineMatrix2D wxGCDCImpl::GetTransformMatrix() const
{
    return m_matrixExtTransform;
}

void wxGCDCImpl::ResetTransformMatrix()
{
    // Reset affine transfrom matrix (extended) to identity matrix.
    m_matrixExtTransform.Set(wxMatrix2D(), wxPoint2DDouble());
    ComputeScaleAndOrigin();
}

#endif // wxUSE_DC_TRANSFORM_MATRIX

// coordinates conversions and transforms
wxPoint wxGCDCImpl::DeviceToLogical(wxCoord x, wxCoord y) const
{
    double px = x;
    double py = y;
    m_matrixCurrentInv.TransformPoint(&px, &py);
    return {wxRound(px), wxRound(py)};
}

wxPoint wxGCDCImpl::LogicalToDevice(wxCoord x, wxCoord y) const
{
    double px = x;
    double py = y;
    m_matrixCurrent.TransformPoint(&px, &py);
    return {wxRound(px), wxRound(py)};
}

wxSize wxGCDCImpl::DeviceToLogicalRel(int x, int y) const
{
    double dx = x;
    double dy = y;
    m_matrixCurrentInv.TransformDistance(&dx, &dy);
    return {wxRound(dx), wxRound(dy)};
}

wxSize wxGCDCImpl::LogicalToDeviceRel(int x, int y) const
{
    double dx = x;
    double dy = y;
    m_matrixCurrent.TransformDistance(&dx, &dy);
    return {wxRound(dx), wxRound(dy)};
}

bool wxGCDCImpl::DoFloodFill(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                             const wxColour& WXUNUSED(col),
                             wxFloodFillStyle WXUNUSED(style))
{
    return false;
}

bool wxGCDCImpl::DoGetPixel( wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxColour *WXUNUSED(col) ) const
{
    //  wxCHECK_MSG( 0 , false, wxT("wxGCDC(cg)::DoGetPixel - not implemented") );
    return false;
}

void wxGCDCImpl::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawLine - invalid DC") );

    if ( !m_logicalFunctionSupported )
        return;

    m_graphicContext->StrokeLine(x1,y1,x2,y2);

    CalcBoundingBox(x1, y1);
    CalcBoundingBox(x2, y2);
}

void wxGCDCImpl::DoCrossHair( wxCoord x, wxCoord y )
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoCrossHair - invalid DC") );

    if ( !m_logicalFunctionSupported )
        return;

    wxSize sz = GetOwner()->GetSize();

    m_graphicContext->StrokeLine(0,y,sz.x,y);
    m_graphicContext->StrokeLine(x,0,x,sz.y);

    CalcBoundingBox(0, 0);
    CalcBoundingBox(sz.x, sz.y);
}

void wxGCDCImpl::DoDrawArc( wxCoord x1, wxCoord y1,
                        wxCoord x2, wxCoord y2,
                        wxCoord xc, wxCoord yc )
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawArc - invalid DC") );

    if ( !m_logicalFunctionSupported )
        return;

    const double dx = x1 - xc;
    const double dy = y1 - yc;
    const double radius = sqrt((double)(dx * dx + dy * dy));
    const wxCoord rad = (wxCoord)radius;

    double sa, ea; // In radians

    // TODO: Return a pair
    if (x1 == x2 && y1 == y2)
    {
        sa = 0.0;
        ea = 2.0 * M_PI;
    }
    else if (radius == 0.0)
    {
        sa = ea = 0.0;
    }
    else
    {
        sa = (x1 - xc == 0) ?
     (y1 - yc < 0) ? M_PI / 2.0 : -M_PI / 2.0 :
             -atan2(double(y1 - yc), double(x1 - xc));
        ea = (x2 - xc == 0) ?
     (y2 - yc < 0) ? M_PI / 2.0 : -M_PI / 2.0 :
             -atan2(double(y2 - yc), double(x2 - xc));
    }

    const bool fill = m_brush.GetStyle() != wxBrushStyle::Transparent;

    wxGraphicsPath path = m_graphicContext->CreatePath();
    if ( fill && ((x1!=x2)||(y1!=y2)) )
        path.MoveToPoint( xc, yc );
    // since these angles (ea,sa) are measured counter-clockwise, we invert them to
    // get clockwise angles
    path.AddArc( xc, yc , rad, -sa, -ea, false );
    if ( fill && ((x1!=x2)||(y1!=y2)) )
        path.AddLineToPoint( xc, yc );
    m_graphicContext->DrawPath(path);

    const wxRect2DDouble box = path.GetBox();
    CalcBoundingBox(wxRound(box.m_x), wxRound(box.m_y));
    CalcBoundingBox(wxRound(box.m_x + box.m_width),
                    wxRound(box.m_y + box.m_height));
}

void wxGCDCImpl::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                double sa, double ea )
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawEllipticArc - invalid DC") );

    if ( !m_logicalFunctionSupported )
        return;

    const wxCoord dx = x + w / 2.0;
    const wxCoord dy = y + h / 2.0;
    const double factor = ((double) w) / h;
    m_graphicContext->PushState();
    m_graphicContext->Translate(dx, dy);
    m_graphicContext->Scale(factor, 1.0);
    wxGraphicsPath path = m_graphicContext->CreatePath();

    // If end angle equals start angle we want draw a full ellipse.
    if (ea == sa)
    {
        ea += 360.0;
    }
    // since these angles (ea,sa) are measured counter-clockwise, we invert them to
    // get clockwise angles
    if ( m_brush.GetStyle() != wxBrushStyle::Transparent )
    {
        path.MoveToPoint( 0, 0 );
        path.AddArc( 0, 0, h/2.0, wxDegToRad(-sa), wxDegToRad(-ea), false );
        path.AddLineToPoint( 0, 0 );
        m_graphicContext->FillPath( path );

        path = m_graphicContext->CreatePath();
        path.AddArc( 0, 0, h/2.0, wxDegToRad(-sa), wxDegToRad(-ea), false );
        m_graphicContext->StrokePath( path );
    }
    else
    {
        path.AddArc( 0, 0, h/2.0, wxDegToRad(-sa), wxDegToRad(-ea), false );
        m_graphicContext->DrawPath( path );
    }

    wxRect2DDouble box = path.GetBox();
    // apply the transformation to the box
    box.m_x *= factor;
    box.m_width *= factor;
    box.m_x += dx;
    box.m_y += dy;

    CalcBoundingBox(wxRound(box.m_x), wxRound(box.m_y));
    CalcBoundingBox(wxRound(box.m_x + box.m_width),
                    wxRound(box.m_y + box.m_height));

    m_graphicContext->PopState();
}

void wxGCDCImpl::DoDrawPoint( wxCoord x, wxCoord y )
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawPoint - invalid DC") );

    if (!m_logicalFunctionSupported)
        return;

    wxDCBrushChanger brushChanger(*GetOwner(), wxBrush(m_pen.GetColour()));
    wxDCPenChanger penChanger(*GetOwner(), *wxTRANSPARENT_PEN);

    // Raster-based DCs draw a single pixel regardless of scale
    m_graphicContext->DrawRectangle(x, y, 1 / m_scale.x, 1 / m_scale.y);

    CalcBoundingBox(x, y);
}

void wxGCDCImpl::DoDrawLines(int n, const wxPoint points[],
                         wxCoord xoffset, wxCoord yoffset)
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawLines - invalid DC") );
    wxASSERT_MSG( n > 0, wxT("wxGCDC(cg)::DoDrawLines - number of points too small") );

    if ( !m_logicalFunctionSupported )
        return;

    int minX = points[0].x;
    int minY = points[0].y;
    int maxX = minX;
    int maxY = minY;

    std::vector<wxPoint2DDouble> pointsD(n);

    for( int i = 0; i < n; ++i)
    {
        const wxPoint p = points[i];
        pointsD[i].m_x = p.x + xoffset;
        pointsD[i].m_y = p.y + yoffset;

        if (p.x < minX)      minX = p.x;
        else if (p.x > maxX) maxX = p.x;
        if (p.y < minY)      minY = p.y;
        else if (p.y > maxY) maxY = p.y;
    }

    m_graphicContext->StrokeLines( n , pointsD.data());

    CalcBoundingBox(minX + xoffset, minY + yoffset);
    CalcBoundingBox(maxX + xoffset, maxY + yoffset);
}

#if wxUSE_SPLINES
void wxGCDCImpl::DoDrawSpline(const wxPointList *points)
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawSpline - invalid DC") );

    if ( !m_logicalFunctionSupported )
        return;

    wxGraphicsPath path = m_graphicContext->CreatePath();

    wxPointList::compatibility_iterator node = points->GetFirst();
    if ( !node )
        // empty list
        return;

    const wxPoint *p = node->GetData();

    wxCoord x1 = p->x;
    wxCoord y1 = p->y;

    node = node->GetNext();
    p = node->GetData();

    wxCoord x2 = p->x;
    wxCoord y2 = p->y;
    const wxCoord cx1 = ( x1 + x2 ) / 2;
    const wxCoord cy1 = ( y1 + y2 ) / 2;

    path.MoveToPoint( x1 , y1 );
    path.AddLineToPoint( cx1 , cy1 );

    while ((node = node->GetNext()))
    {
        p = node->GetData();
        x1 = x2;
        y1 = y2;
        x2 = p->x;
        y2 = p->y;
        const wxCoord cx4 = (x1 + x2) / 2;
        const wxCoord cy4 = (y1 + y2) / 2;

        path.AddQuadCurveToPoint(x1 , y1 ,cx4 , cy4 );
    }

    path.AddLineToPoint( x2 , y2 );

    m_graphicContext->StrokePath( path );

    const wxRect2DDouble box = path.GetBox();
    CalcBoundingBox(wxRound(box.m_x), wxRound(box.m_y));
    CalcBoundingBox(wxRound(box.m_x + box.m_width),
                    wxRound(box.m_y + box.m_height));
}
#endif // wxUSE_SPLINES

void wxGCDCImpl::DoDrawPolygon( int n, const wxPoint points[],
                                wxCoord xoffset, wxCoord yoffset,
                                wxPolygonFillMode fillStyle )
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawPolygon - invalid DC") );

    if ( n <= 0 ||
            (m_brush.GetStyle() == wxBrushStyle::Transparent &&
                m_pen.GetStyle() == wxPenStyle::Transparent) )
        return;
    if ( !m_logicalFunctionSupported )
        return;

    bool closeIt = false;
    if (points[n-1] != points[0])
        closeIt = true;

    int minX = points[0].x;
    int minY = points[0].y;
    int maxX = minX;
    int maxY = minY;

    std::vector<wxPoint2DDouble> pointsD(n + (closeIt ? 1 : 0));

    for( int i = 0; i < n; ++i)
    {
        wxPoint p = points[i];
        pointsD[i].m_x = p.x + xoffset;
        pointsD[i].m_y = p.y + yoffset;

        if (p.x < minX)      minX = p.x;
        else if (p.x > maxX) maxX = p.x;
        if (p.y < minY)      minY = p.y;
        else if (p.y > maxY) maxY = p.y;
    }

    if ( closeIt )
        pointsD[n] = pointsD[0];

    m_graphicContext->DrawLines( n + (closeIt ? 1 : 0) , pointsD.data(), fillStyle);

    CalcBoundingBox(minX + xoffset, minY + yoffset);
    CalcBoundingBox(maxX + xoffset, maxY + yoffset);
}

void wxGCDCImpl::DoDrawPolyPolygon(int n,
                               const int count[],
                               const wxPoint points[],
                               wxCoord xoffset,
                               wxCoord yoffset,
                               wxPolygonFillMode fillStyle)
{
    wxASSERT(n > 1);
    wxGraphicsPath path = m_graphicContext->CreatePath();

    int i = 0;
    for ( int j = 0; j < n; ++j)
    {
        wxPoint start = points[i];
        path.MoveToPoint( start.x+ xoffset, start.y+ yoffset);
        ++i;
        int l = count[j];
        for ( int k = 1; k < l; ++k)
        {
            path.AddLineToPoint( points[i].x+ xoffset, points[i].y+ yoffset);
            ++i;
        }
        // close the polygon
        if ( start != points[i-1])
            path.AddLineToPoint( start.x+ xoffset, start.y+ yoffset);
    }

    m_graphicContext->DrawPath( path , fillStyle);

    wxRect2DDouble box = path.GetBox();
    CalcBoundingBox(wxRound(box.m_x), wxRound(box.m_y));
    CalcBoundingBox(wxRound(box.m_x + box.m_width),
                    wxRound(box.m_y + box.m_height));
}

void wxGCDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawRectangle - invalid DC") );

    if ( !m_logicalFunctionSupported )
        return;

    // CMB: draw nothing if transformed w or h is 0
    if (w == 0 || h == 0)
        return;

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + w, y + h);

    if (m_pen.IsOk() && m_pen.GetStyle() != wxPenStyle::Transparent && m_pen.GetWidth() > 0)
    {
        // outline is one pixel larger than what raster-based wxDC implementations draw
        w -= 1;
        h -= 1;
    }
    m_graphicContext->DrawRectangle(x,y,w,h);
}

void wxGCDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                    wxCoord w, wxCoord h,
                                    double radius)
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawRoundedRectangle - invalid DC") );

    if ( !m_logicalFunctionSupported )
        return;

    if (radius < 0.0)
        radius = - radius * ((w < h) ? w : h);

    // CMB: draw nothing if transformed w or h is 0
    if (w == 0 || h == 0)
        return;

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + w, y + h);

    if (m_pen.IsOk() && m_pen.GetStyle() != wxPenStyle::Transparent && m_pen.GetWidth() > 0)
    {
        // outline is one pixel larger than what raster-based wxDC implementations draw
        w -= 1;
        h -= 1;
    }
    m_graphicContext->DrawRoundedRectangle( x,y,w,h,radius);
}

void wxGCDCImpl::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawEllipse - invalid DC") );

    if ( !m_logicalFunctionSupported )
        return;

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + w, y + h);

    m_graphicContext->DrawEllipse(x,y,w,h);
}

bool wxGCDCImpl::CanDrawBitmap() const
{
    return true;
}

bool wxGCDCImpl::DoBlit(
    wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
    wxDC *source, wxCoord xsrc, wxCoord ysrc,
    wxRasterOperationMode logical_func , bool useMask,
    wxCoord xsrcMask, wxCoord ysrcMask )
{
    return DoStretchBlit( xdest, ydest, width, height,
        source, xsrc, ysrc, width, height, logical_func, useMask,
        xsrcMask,ysrcMask );
}

bool wxGCDCImpl::DoStretchBlit(
    wxCoord xdest, wxCoord ydest, wxCoord dstWidth, wxCoord dstHeight,
    wxDC *source, wxCoord xsrc, wxCoord ysrc, wxCoord srcWidth, wxCoord srcHeight,
    wxRasterOperationMode logical_func , bool useMask,
    wxCoord xsrcMask, wxCoord ysrcMask )
{
    wxCHECK_MSG( IsOk(), false, wxT("wxGCDC(cg)::DoStretchBlit - invalid DC") );
    wxCHECK_MSG( source->IsOk(), false, wxT("wxGCDC(cg)::DoStretchBlit - invalid source DC") );

    if ( logical_func == wxRasterOperationMode::NoOp )
        return true;

    const wxCompositionMode mode = TranslateRasterOp(logical_func);
    if ( mode == wxCOMPOSITION_INVALID )
    {
        // Do *not* assert here, this function is often call from wxEVT_PAINT
        // handler and asserting will just result in a reentrant call to the
        // same handler and a crash.
        return false;
    }

    wxRect subrect(source->LogicalToDeviceX(xsrc),
                   source->LogicalToDeviceY(ysrc),
                   source->LogicalToDeviceXRel(srcWidth),
                   source->LogicalToDeviceYRel(srcHeight));
    const wxRect subrectOrig = subrect;
    // clip the subrect down to the size of the source DC
    // FIXME: Directly initialize wxRect clip with a size instead.
    wxRect clip;
    clip.width = source->GetSize().x;
    clip.height = source->GetSize().y;

    subrect.Intersect(clip);
    if (subrect.width == 0)
        return true;

    bool retval = true;

    const wxCompositionMode formerMode = m_graphicContext->GetCompositionMode();
    if (m_graphicContext->SetCompositionMode(mode))
    {
        const wxAntialiasMode formerAa = m_graphicContext->GetAntialiasMode();
        if (mode == wxCOMPOSITION_XOR)
        {
            m_graphicContext->SetAntialiasMode(wxANTIALIAS_NONE);
        }

        if (xsrcMask == -1 && ysrcMask == -1)
        {
            xsrcMask = xsrc;
            ysrcMask = ysrc;
        }

        wxBitmap blit = source->GetAsBitmap( &subrect );

        if ( blit.IsOk() )
        {
            if ( !useMask && blit.GetMask() )
                blit.SetMask(nullptr);

            double x = xdest;
            double y = ydest;
            double w = dstWidth;
            double h = dstHeight;

            // adjust dest rect if source rect is clipped
            if (subrect.width != subrectOrig.width || subrect.height != subrectOrig.height)
            {
                x += (subrect.x - subrectOrig.x) / double(subrectOrig.width) * dstWidth;
                y += (subrect.y - subrectOrig.y) / double(subrectOrig.height) * dstHeight;
                w *= double(subrect.width) / subrectOrig.width;
                h *= double(subrect.height) / subrectOrig.height;
            }
            m_graphicContext->DrawBitmap(blit, x, y, w, h);
        }
        else
        {
            wxFAIL_MSG( wxT("Cannot Blit. Unable to get contents of DC as bitmap.") );
            retval = false;
        }

        if (mode == wxCOMPOSITION_XOR)
        {
            m_graphicContext->SetAntialiasMode(formerAa);
        }
    }
    // reset composition
    m_graphicContext->SetCompositionMode(formerMode);

    CalcBoundingBox(xdest, ydest);
    CalcBoundingBox(xdest + dstWidth, ydest + dstHeight);

    return retval;
}

void wxGCDCImpl::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                               double angle)
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoDrawRotatedText - invalid DC") );

    if ( text.empty() )
        return;
    if ( !m_logicalFunctionSupported )
        return;

    // we test that we have some font because otherwise we should still use the
    // "else" part below to avoid that DrawRotatedText(angle = 180) and
    // DrawRotatedText(angle = 0) use different fonts (we can't use the default
    // font for drawing rotated fonts unfortunately)
    if ( (angle == 0.0) && m_font.IsOk() )
    {
        DoDrawText(text, x, y);

        // Bounding box already updated by DoDrawText(), no need to do it again.
        return;
    }

    // Get extent of whole text.
    wxCoord w, h, heightLine;
    GetOwner()->GetMultiLineTextExtent(text, &w, &h, &heightLine);

    // Compute the shift for the origin of the next line.
    const double rad = wxDegToRad(angle);
    const double dx = heightLine * sin(rad);
    const double dy = heightLine * cos(rad);

    // Draw all text line by line
    const std::vector<wxString> lines = wxSplit(text, '\n', '\0');
    for ( size_t lineNum = 0; lineNum < lines.size(); lineNum++ )
    {
        // Calculate origin for each line to avoid accumulation of
        // rounding errors.
        if ( m_backgroundMode == wxBrushStyle::Transparent )
            m_graphicContext->DrawText( lines[lineNum], x + wxRound(lineNum*dx), y + wxRound(lineNum*dy), wxDegToRad(angle ));
        else
            m_graphicContext->DrawText( lines[lineNum], x + wxRound(lineNum*dx), y + wxRound(lineNum*dy), wxDegToRad(angle ), m_graphicContext->CreateBrush(m_textBackgroundColour) );
   }

    // call the bounding box by adding all four vertices of the rectangle
    // containing the text to it (simpler and probably not slower than
    // determining which of them is really topmost/leftmost/...)

    // "upper left" and "upper right"
    CalcBoundingBox(x, y);
    CalcBoundingBox(x + wxCoord(w*cos(rad)), y - wxCoord(w*sin(rad)));

    // "bottom left" and "bottom right"
    x += (wxCoord)(h*sin(rad));
    y += (wxCoord)(h*cos(rad));
    CalcBoundingBox(x, y);
    CalcBoundingBox(x + wxCoord(w*cos(rad)), y - wxCoord(w*sin(rad)));
}

void wxGCDCImpl::DoDrawText(const wxString& str, wxCoord x, wxCoord y)
{
    wxCHECK_RET( IsOk(), "wxGCDC::DoDrawText - invalid DC" );

    if ( str.empty() )
        return;

    // For compatibility with other ports (notably wxGTK) and because it's
    // genuinely useful, we allow passing multiline strings to DrawText().
    // However there is no native OSX function to draw them directly so we
    // instead reuse the generic DrawLabel() method to render them. Of course,
    // DrawLabel() itself will call back to us but with single line strings
    // only so there won't be any infinite recursion here.
    if ( str.find('\n') != wxString::npos )
    {
        GetOwner()->DrawLabel(str, wxRect(x, y, 0, 0));
        return;
    }

    // Text drawing shouldn't be affected by the raster operation
    // mode set by SetLogicalFunction() and should be always done
    // in the default wxRasterOperationMode::Copy mode (which is wxCOMPOSITION_OVER
    // composition mode).
    const wxCompositionMode curMode = m_graphicContext->GetCompositionMode();
    m_graphicContext->SetCompositionMode(wxCOMPOSITION_OVER);

    if ( m_backgroundMode == wxBrushStyle::Transparent )
        m_graphicContext->DrawText( str, x ,y);
    else
        m_graphicContext->DrawText( str, x ,y , m_graphicContext->CreateBrush(m_textBackgroundColour) );

    m_graphicContext->SetCompositionMode(curMode);

    auto textExtents = GetOwner()->GetTextExtent(str);
    CalcBoundingBox(x, y);
    CalcBoundingBox(x + textExtents.x, y + textExtents.y);
}

bool wxGCDCImpl::CanGetTextExtent() const
{
    wxCHECK_MSG( IsOk(), false, wxT("wxGCDC(cg)::CanGetTextExtent - invalid DC") );

    return true;
}

wxSize wxGCDCImpl::DoGetTextExtent( const wxString &str,
                              wxCoord *descent, wxCoord *externalLeading ,
                              const wxFont *theFont ) const
{
    //wxCHECK_RET( m_graphicContext, wxT("wxGCDC(cg)::DoGetTextExtent - invalid DC") );

    if ( theFont )
    {
        m_graphicContext->SetFont( *theFont, m_textForegroundColour );
    }

    double   d wxDUMMY_INITIALIZE(0),
             e wxDUMMY_INITIALIZE(0);

    // Don't pass non-NULL pointers for the parts we don't need, this could
    // result in doing extra unnecessary work inside GetTextExtent().
    auto [width, height] = m_graphicContext->GetTextExtent
                      (
                        str,
                        descent ? &d : nullptr,
                        externalLeading ? &e : nullptr
                      );

    if ( descent )
        *descent = (wxCoord)wxRound(d);
    if ( externalLeading )
        *externalLeading = (wxCoord)wxRound(e);

    if ( theFont )
    {
        m_graphicContext->SetFont( m_font, m_textForegroundColour );
    }

    return { wxRound(width), wxRound(height) };
}

std::vector<int> wxGCDCImpl::DoGetPartialTextExtents(const wxString& text) const
{
    if ( text.IsEmpty() )
        return {};

    std::vector<double> widthsD = m_graphicContext->GetPartialTextExtents(text);

    std::vector<int> widths(widthsD.size());
    std::transform(widthsD.begin(), widthsD.end(), widths.begin(), [](auto width){ return wxRound(width); });


    return widths;
}

wxCoord wxGCDCImpl::GetCharWidth() const
{
    return DoGetTextExtent( wxT("g"), nullptr, nullptr, nullptr ).x;
}

wxCoord wxGCDCImpl::GetCharHeight() const
{
    return DoGetTextExtent( wxT("g"), nullptr, nullptr, nullptr ).y;
}

void wxGCDCImpl::Clear()
{
    wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::Clear - invalid DC") );

    if ( m_backgroundBrush.IsTransparent() )
        return;

    m_graphicContext->SetBrush( m_backgroundBrush.IsOk() ? m_backgroundBrush
                                                         : *wxWHITE_BRUSH );
    wxPen p = *wxTRANSPARENT_PEN;
    m_graphicContext->SetPen( p );
    const wxCompositionMode formerMode = m_graphicContext->GetCompositionMode();
    m_graphicContext->SetCompositionMode(wxCOMPOSITION_SOURCE);

    double x, y, w, h;
    m_graphicContext->GetClipBox(&x, &y, &w, &h);
    m_graphicContext->DrawRectangle(x, y, w, h);

    m_graphicContext->SetCompositionMode(formerMode);
    m_graphicContext->SetPen( m_pen );
    m_graphicContext->SetBrush( m_brush );
}

wxSize wxGCDCImpl::DoGetSize() const
{
    // FIXME: Return value?
    //wxCHECK_RET( IsOk(), wxT("wxGCDC(cg)::DoGetSize - invalid DC") );
    const wxRealPoint sz = m_graphicContext->GetSize();

    return {wxRound(sz.x), wxRound(sz.y)};
}

void wxGCDCImpl::DoGradientFillLinear(const wxRect& rect,
                                  const wxColour& initialColour,
                                  const wxColour& destColour,
                                  wxDirection nDirection )
{
    if (rect.width == 0 || rect.height == 0)
        return;

    wxPoint start;
    wxPoint end;
    switch( nDirection)
    {
    case wxWEST :
        start = rect.GetRightBottom();
        start.x++;
        end = rect.GetLeftBottom();
        break;
    case wxEAST :
        start = rect.GetLeftBottom();
        end = rect.GetRightBottom();
        end.x++;
        break;
    case wxNORTH :
        start = rect.GetLeftBottom();
        start.y++;
        end = rect.GetLeftTop();
        break;
    case wxSOUTH :
        start = rect.GetLeftTop();
        end = rect.GetLeftBottom();
        end.y++;
        break;
    default :
        break;
    }

    m_graphicContext->SetBrush( m_graphicContext->CreateLinearGradientBrush(
        start.x,start.y,end.x,end.y, initialColour, destColour));
    m_graphicContext->SetPen(*wxTRANSPARENT_PEN);
    m_graphicContext->DrawRectangle(rect.x,rect.y,rect.width,rect.height);
    m_graphicContext->SetPen(m_pen);
    m_graphicContext->SetBrush(m_brush);

    CalcBoundingBox(rect.x, rect.y);
    CalcBoundingBox(rect.x + rect.width, rect.y + rect.height);
}

void wxGCDCImpl::DoGradientFillConcentric(const wxRect& rect,
                                      const wxColour& initialColour,
                                      const wxColour& destColour,
                                      const wxPoint& circleCenter)
{
    //Radius
    const wxInt32 nRadius = [rect]()
    {
        if ((rect.GetWidth() / 2) < (rect.GetHeight() / 2))
        {
            return (rect.GetWidth() / 2);
        }
        else
        {
            return (rect.GetHeight() / 2);
        }
    }();

    // make sure the background is filled (todo move into specific platform implementation ?)
    m_graphicContext->SetPen(*wxTRANSPARENT_PEN);
    m_graphicContext->SetBrush( wxBrush( destColour) );
    m_graphicContext->DrawRectangle(rect.x,rect.y,rect.width,rect.height);

    m_graphicContext->SetBrush( m_graphicContext->CreateRadialGradientBrush(
        rect.x+circleCenter.x,rect.y+circleCenter.y,
        rect.x+circleCenter.x,rect.y+circleCenter.y,
        nRadius,initialColour,destColour));

    m_graphicContext->DrawRectangle(rect.x,rect.y,rect.width,rect.height);
    m_graphicContext->SetPen(m_pen);
    m_graphicContext->SetBrush(m_brush);

    CalcBoundingBox(rect.x, rect.y);
    CalcBoundingBox(rect.x + rect.width, rect.y + rect.height);
}

void wxGCDCImpl::DoDrawCheckMark(wxCoord x, wxCoord y,
                             wxCoord width, wxCoord height)
{
    wxDCImpl::DoDrawCheckMark(x,y,width,height);
}

#ifdef __WXMSW__
wxRect wxGCDCImpl::MSWApplyGDIPlusTransform(const wxRect& r) const
{
    wxCHECK_MSG( IsOk(), r, wxS("Invalid wxGCDC") );

    double x = 0,
           y = 0;
    m_graphicContext->GetTransform().TransformPoint(&x, &y);

    wxRect rect(r);
    rect.Offset(x, y);

    return rect;
}
#endif // __WXMSW__

#endif // wxUSE_GRAPHICS_CONTEXT
