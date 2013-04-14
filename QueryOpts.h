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
// Name:        QueryOpts.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __QueryOpts_H__
#define __QueryOpts_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "QueryOpts.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "fragmon_wdr.h"
#include "pyemb.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// QueryOpts
//----------------------------------------------------------------------------

class QueryOpts: public wxPanel
{
public:
    // constructors and destructors
    QueryOpts( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
        
    static wxString GetDefaultGame();
    static void LoadSocketTimeout();
    static void SaveSocketTimeout();
    
    // WDR: method declarations for QueryOpts
    wxTextCtrl* GetQueryTo()  { return (wxTextCtrl*) FindWindow( ID_QUERY_TO ); }
    wxChoice* GetDefGm()  { return (wxChoice*) FindWindow( ID_DEF_GM ); }
    
private:
    // WDR: member variable declarations for QueryOpts
    GameList m_glist;
    
private:
    // WDR: handler declarations for QueryOpts
    void OnApply( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
