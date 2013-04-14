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
// Name:        QueryOpts.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "QueryOpts.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "QueryOpts.h"
#include "pyemb.h"
#include "Fragmon.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// QueryOpts
//----------------------------------------------------------------------------

// WDR: event table for QueryOpts

BEGIN_EVENT_TABLE(QueryOpts,wxPanel)
    EVT_BUTTON( wxID_APPLY, QueryOpts::OnApply )
END_EVENT_TABLE()

QueryOpts::QueryOpts( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    // WDR: dialog function QueryOptsFunc for QueryOpts
    QueryOptsFunc( this, TRUE ); 
    
    PyGetGameList(m_glist);
    wxChoice *type = GetDefGm();
    for (int i = 0; i < m_glist.GetCount(); i++)
        type->Append(m_glist[i].name, (void *)&m_glist[i]); 
        
    type->SetSelection(0);

    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/GameOpts");
    
    wxString defgame;
    int autofind = 0;
    
    config->Read("DefaultGame", &defgame);
    
    for (int i = 0; i < m_glist.GetCount(); i++)
        if (m_glist[i].abbrev == defgame)
        {
            type->SetSelection(i);
            break;
        }
        
    GetQueryTo()->SetValue(wxString::Format("%.1f", PyGetSocketTimeout()));
}

// WDR: handler implementations for QueryOpts

void QueryOpts::OnApply( wxCommandEvent &event )
{
    int sel = GetDefGm()->GetSelection();
    wxASSERT(sel >= 0);
    
    GameType *gtype = (GameType *)GetDefGm()->GetClientData(sel);
    wxASSERT(gtype != NULL);
    
    wxString defgame = gtype->abbrev;
    
    double timeout;
    wxString ts = GetQueryTo()->GetValue();
    if (!ts.ToDouble(&timeout))
    {
        wxMessageBox("Invalid timeout", "Error");
        return;
    }
    PySetSocketTimeout(timeout);
    
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/GameOpts");    
    config->Write("DefaultGame", defgame);
}

wxString QueryOpts::GetDefaultGame()
{
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/GameOpts");    
    
    wxString defgame;
    
    config->Read("DefaultGame", &defgame);
    
    return defgame;
}

void QueryOpts::LoadSocketTimeout()
{
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/GameOpts");   
    
    double sockto = 1.0;
    config->Read("SockTimeout", &sockto);
    PySetSocketTimeout(sockto);
}

void QueryOpts::SaveSocketTimeout()
{
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/GameOpts");   
    
    double sockto = PyGetSocketTimeout();
    config->Write("SockTimeout", sockto);
}




