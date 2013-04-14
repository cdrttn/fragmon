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
// Name:        BroadcastFmt.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __BroadcastFmt_H__
#define __BroadcastFmt_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "BroadcastFmt.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "fragmon_wdr.h"
#include "scorebot.h"

#define DEF_BOT_FMT "{title}: [{name_a} {score_a}] vs [{name_b} {score_b}]  time: {time} map: {map}"

// WDR: class declarations

//----------------------------------------------------------------------------
// BroadcastFmt
//----------------------------------------------------------------------------

class BroadcastFmt: public wxPanel
{
public:
    // constructors and destructors
    BroadcastFmt( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
    // WDR: method declarations for BroadcastFmt
    wxTextCtrl* GetBotExOut()  { return (wxTextCtrl*) FindWindow( ID_BOT_EX_OUT ); }
    wxTextCtrl* GetBotFormatIn()  { return (wxTextCtrl*) FindWindow( ID_BOT_FORMAT_IN ); }

    static wxString GetFormat();
    static void LoadFormat(const ScoreBot &bot, VarMap &vmap);
    
private:
    // WDR: member variable declarations for BroadcastFmt
    
private:
    // WDR: handler declarations for BroadcastFmt
    void OnText( wxCommandEvent &event );
    void OnApply( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
