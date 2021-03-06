/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/stringimpl.cpp
// Purpose:     wxString class
// Author:      Vadim Zeitlin, Ryan Norton
// Modified by:
// Created:     29/01/98
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
//              (c) 2004 Ryan Norton <wxprojects@comcast.net>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
 * About ref counting:
 *  1) all empty strings use g_strEmpty, nRefs = -1 (set in Init())
 *  2) AllocBuffer() sets nRefs to 1, Lock() increments it by one
 *  3) Unlock() decrements nRefs and frees memory if it goes to 0
 */

// ===========================================================================
// headers, declarations, constants
// ===========================================================================

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/stringimpl.h"
    #include "wx/wxcrt.h"
#endif

#include <cctype>

#include <cerrno>

#include <cstdlib>
#include <cstring>

// ---------------------------------------------------------------------------
// static class variables definition
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// static data
// ----------------------------------------------------------------------------

// FIXME-UTF8: get rid of this, have only one wxEmptyString
#if wxUSE_UNICODE_UTF8
const wxStringCharType WXDLLIMPEXP_BASE *wxEmptyStringImpl = "";
#endif
const wxChar WXDLLIMPEXP_BASE *wxEmptyString = wxT("");
