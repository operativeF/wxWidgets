///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dndcmn.cpp
// Author:      Robert Roebling
// Modified by:
// Created:     19.10.99
// Copyright:   (c) wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#include "wx/dnd.h"

#if wxUSE_DRAG_AND_DROP

bool wxIsDragResultOk(wxDragResult res)
{
    return res == wxDragResult::Copy || res == wxDragResult::Move || res == wxDragResult::Link;
}

#endif

