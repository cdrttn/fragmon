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
// Name:        CopyFmt.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __CopyFmt_H__
#define __CopyFmt_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "CopyFmt.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "fragmon_wdr.h"
#include "pyemb.h"
#include "varstr.h"

#define DEF_CPY_FMT "{server}:{port} {ping}ms {hostname} players: {curplayers}/{maxplayers} map: {map}"

// WDR: class declarations

//----------------------------------------------------------------------------
// CopyFmt
//----------------------------------------------------------------------------

class CopyFmt: public wxPanel
{
public:
    // constructors and destructors
    CopyFmt( wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL );
    
    // WDR: method declarations for CopyFmt
    wxTextCtrl* GetCfmtEx()  { return (wxTextCtrl*) FindWindow( ID_CFMT_EX ); }
    wxTextCtrl* GetCfmtIn()  { return (wxTextCtrl*) FindWindow( ID_CFMT_IN ); }
    
    static wxString GetFormat();
    static wxString FormatCopy(GameInfo *ginfo);
    static void LoadFormat(GameInfo *ginfo, VarMap &vmap);
    
private:
    // WDR: member variable declarations for CopyFmt

private:
    // WDR: handler declarations for CopyFmt
    void OnText( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
