/*
** Copyright (c) 2006 Christopher Davis
** 
** Permission is hereby granted, free of charge, to any person obtaining a copy of
** this software and associated documentation files (the "Software"), to deal in 
** the Software without restriction, including without limitation the rights to 
** use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
** the Software, and to permit persons to whom the Software is furnished to do so,
** subject to the following conditions:
** 
** The above copyright notice and this permission notice shall be included in all 
** copies or substantial portions of the Software.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
** FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
** COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
** IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
** 
** $Id$
*/



/////////////////////////////////////////////////////////////////////////////
// Name:        CompOutput.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "CompOutput.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "CompOutput.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CompOutput
//----------------------------------------------------------------------------

// WDR: event table for CompOutput
           
BEGIN_EVENT_TABLE(CompOutput,wxPanel)
    EVT_GAMEQUERY(-1, CompOutput::OnGameQuery)
    EVT_TEXT(ID_COMP_TIME, CompOutput::OnText)
    EVT_TEXT(ID_COMP_SCORE_A, CompOutput::OnText)
    EVT_TEXT(ID_COMP_SCORE_B, CompOutput::OnText)
    EVT_TEXT(ID_COMP_NAME_A, CompOutput::OnText)
    EVT_TEXT(ID_COMP_NAME_B, CompOutput::OnText)
END_EVENT_TABLE()

CompOutput::CompOutput( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style ),
    m_dirty(0), m_setval(false)
{
    // WDR: dialog function CompOutputFunc for CompOutput
    CompOutputFunc( this, TRUE ); 
}

// WDR: handler implementations for CompOutput
void CompOutput::OnGameQuery(PyEvent &evt)
{
    SetNameA(""); 
    SetNameB(""); 
    SetScoreA("");
    SetScoreB("");
    SetTime("");
    
    GameData *gdata = evt.GetGdata();
    if (!gdata)
        return;

    if (gdata->info)
    {
        SetTime(gdata->info->time);
        SetNameA(gdata->info->teamAname);
        SetNameB(gdata->info->teamBname);
        SetScoreA(wxString::Format("%d", gdata->info->teamAscore));
        SetScoreB(wxString::Format("%d", gdata->info->teamBscore));
    }
}

void CompOutput::OnText(wxCommandEvent &evt)
{
    bool clear = false;
    //SetValue shouldn't make dirty; only user's keypresses should
    if (m_setval)
        return;
    //PyPrintDebug(false, "value -> %s\n", evt.GetString().c_str());
    if (evt.GetString().empty())
        clear = true;

    switch (evt.GetId())
    {
    case ID_COMP_TIME:
        if (clear) ClearDirty(TIME_DIRTY);
        else AddDirty(TIME_DIRTY);
        break;
    case ID_COMP_SCORE_B:
        if (clear) ClearDirty(SCOREB_DIRTY);
        else AddDirty(SCOREB_DIRTY);
        break;
    case ID_COMP_SCORE_A: 
        if (clear) ClearDirty(SCOREA_DIRTY);
        else AddDirty(SCOREA_DIRTY);
        break;
    case ID_COMP_NAME_B:
        if (clear) ClearDirty(NAMEB_DIRTY);
        else AddDirty(NAMEB_DIRTY); 
        break;
    case ID_COMP_NAME_A:
        if (clear) ClearDirty(NAMEA_DIRTY);
        else AddDirty(NAMEA_DIRTY);
    }

}
