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
// Name:        CopyFmt.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "CopyFmt.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "CopyFmt.h"
#include "Fragmon.h"
#include "varstr.h"
#include "pyemb.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CopyFmt
//----------------------------------------------------------------------------

// WDR: event table for CopyFmt

BEGIN_EVENT_TABLE(CopyFmt,wxPanel)
    EVT_BUTTON( wxID_APPLY, CopyFmt::OnOk )
    EVT_TEXT( ID_CFMT_IN, CopyFmt::OnText )
END_EVENT_TABLE()

CopyFmt::CopyFmt( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    // WDR: dialog function CopyFmtFunc for CopyFmt
    CopyFmtFunc( this, TRUE ); 
    
    wxString fmt = GetFormat();
    GetCfmtIn()->SetValue(fmt);
}

// WDR: handler implementations for CopyFmt

void CopyFmt::OnText( wxCommandEvent &event )
{
    VarMap vmap;
    
    vmap["server"] = "11.12.13.14";
    vmap["port"] = "4421";
    vmap["ping"] = "50";
    vmap["hostname"] = "Booboo's baby server";
    vmap["curplayers"] = "12";
    vmap["maxplayers"] = "16";
    vmap["map"] = "pro-q3dm6";
    vmap["type"] = "Team DM";
    vmap["mod"] = "OSP";
    vmap["version"] = "q3 1.31";
    vmap["password"] = "No";
    
    GetCfmtEx()->SetValue(VarSubst(GetCfmtIn()->GetValue(), vmap, true));
}

void CopyFmt::OnOk(wxCommandEvent &event)
{
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/GameOpts");
    config->Write("format", GetCfmtIn()->GetValue());
    
    event.Skip();
}

wxString CopyFmt::GetFormat()
{
    wxString fmt = DEF_CPY_FMT;
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/GameOpts");
    config->Read("format", &fmt);

    return fmt;
}

wxString CopyFmt::FormatCopy(GameInfo *ginfo)
{
    wxString fmt = GetFormat();    
    VarMap vmap;
    LoadFormat(ginfo, vmap); 
    
    return VarSubst(fmt, vmap);
}

void CopyFmt::LoadFormat(GameInfo *ginfo, VarMap &vmap)
{
    vmap["server"] = ginfo->server;
    vmap["port"] = wxString::Format("%d", ginfo->port);
    vmap["ping"] = wxString::Format("%d", ginfo->ping);
    vmap["hostname"] = ginfo->hostname;
    vmap["curplayers"] = wxString::Format("%d", ginfo->curplayers);
    vmap["maxplayers"] = wxString::Format("%d", ginfo->maxplayers);
    vmap["map"] = ginfo->map;
    vmap["type"] = ginfo->type;
    vmap["mod"] = ginfo->mod;
    vmap["version"] = ginfo->version;
    vmap["password"] = ginfo->password? "Yes" : "No";
}




