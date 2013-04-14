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
// Name:        ExecInput.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __ExecInput_H__
#define __ExecInput_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ExecInput.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "fragmon_wdr.h"
#include "pyemb.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// ExecInput
//----------------------------------------------------------------------------

class ExecInput: public wxPanel
{
public:
    // constructors and destructors
    ExecInput( wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL );
    
    // WDR: method declarations for ExecInput
    wxTextCtrl* GetExecEx()  { return (wxTextCtrl*) FindWindow( ID_EXEC_EX ); }
    wxTextCtrl* GetExecOrderIn()  { return (wxTextCtrl*) FindWindow( ID_EXEC_ORDER_IN ); }
    wxTextCtrl* GetExecNameIn()  { return (wxTextCtrl*) FindWindow( ID_EXEC_NAME_IN ); }
    wxTextCtrl* GetExecPassIn()  { return (wxTextCtrl*) FindWindow( ID_EXEC_PASS_IN ); }
    wxTextCtrl* GetExecConnIn()  { return (wxTextCtrl*) FindWindow( ID_EXEC_CONN_IN ); }
    wxTextCtrl* GetExecPathIn()  { return (wxTextCtrl*) FindWindow( ID_EXEC_PATH_IN ); }
    wxChoice* GetExecType()  { return (wxChoice*) FindWindow( ID_EXEC_TYPE ); }
    static void ReadExecOpts(const GameType *gtype, wxString *path, wxString *order, 
                             wxString *conn, wxString *pass, wxString *name);
    static bool FormatCommand(const GameType *gtype, const wxString &host, int port,
                              const wxString &password = "", const wxString &name = "", 
                              wxString *out = NULL, bool execute = true);
                               

private:
    // WDR: member variable declarations for ExecInput
    GameList m_glist;

private:
    // WDR: handler declarations for ExecInput
    void OnTextChange( wxCommandEvent &event );
    void OnApply( wxCommandEvent &event );
    void OnGetPath( wxCommandEvent &event );
    void OnSelect( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
