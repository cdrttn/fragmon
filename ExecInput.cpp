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
// Name:        ExecInput.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "ExecInput.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/filename.h"
#include "ExecInput.h"
#include "pyemb.h"
#include "varstr.h"
#include "Fragmon.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// ExecInput
//----------------------------------------------------------------------------

// WDR: event table for ExecInput

BEGIN_EVENT_TABLE(ExecInput,wxPanel)
    EVT_CHOICE( ID_EXEC_TYPE, ExecInput::OnSelect )
    EVT_BUTTON( ID_EXEC_GETPATH, ExecInput::OnGetPath )
    EVT_BUTTON( wxID_APPLY, ExecInput::OnApply )
    EVT_TEXT( -1, ExecInput::OnTextChange )
END_EVENT_TABLE()

ExecInput::ExecInput( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    // WDR: dialog function ExecInputFunc for ExecInput
    ExecInputFunc( this, TRUE ); 
    
    PyGetGameList(m_glist);
    wxChoice *type = GetExecType();
    for (int i = 0; i < m_glist.GetCount(); i++)
        type->Append(m_glist[i].name, (void *)&m_glist[i]);
}

// WDR: handler implementations for ExecInput

void ExecInput::OnTextChange( wxCommandEvent &event )
{
    //make sure to ignore text events from ex box
    if (event.GetId() == ID_EXEC_EX)
        return;

    VarMap vmap;
    vmap["host"] = "11.12.13.14";
    vmap["port"] = "5432";
    vmap["name"] = "Tester";
    vmap["password"] = "myscrt";
    
    VarMap vopt;
    wxString tmp = GetExecConnIn()->GetValue();
    if (!tmp.IsEmpty())
        vopt["connect_opt"] = VarSubst(tmp, vmap, true);
    
    tmp = GetExecPassIn()->GetValue();
    if (!tmp.IsEmpty())
        vopt["password_opt"] = VarSubst(tmp, vmap, true);
        
    tmp = GetExecNameIn()->GetValue();
    if (!tmp.IsEmpty())
        vopt["name_opt"] = VarSubst(tmp, vmap, true);
        
    tmp = GetExecPathIn()->GetValue();
    if (!tmp.IsEmpty())
        vopt["game_path"] = tmp;
        
    wxString fmt = _T("{game_path} ") + GetExecOrderIn()->GetValue();
    
    GetExecEx()->SetValue(VarSubst(fmt, vopt, true));
}

void ExecInput::OnApply( wxCommandEvent &event )
{
    wxChoice *type = GetExecType();
    int sel = type->GetSelection();
    
    if (sel == wxNOT_FOUND)
        return;

    
    GameType *gtype = (GameType *)type->GetClientData(sel);
    wxASSERT (gtype != NULL);
    
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/GameOpts");
    config->Write(wxString::Format("Path%s", gtype->abbrev.c_str()), GetExecPathIn()->GetValue());
    config->Write(wxString::Format("Order%s", gtype->abbrev.c_str()), GetExecOrderIn()->GetValue());
    config->Write(wxString::Format("Conn%s", gtype->abbrev.c_str()), GetExecConnIn()->GetValue());
    config->Write(wxString::Format("Pass%s", gtype->abbrev.c_str()), GetExecPassIn()->GetValue());
    config->Write(wxString::Format("Name%s", gtype->abbrev.c_str()), GetExecNameIn()->GetValue());
}

void ExecInput::OnGetPath( wxCommandEvent &event )
{
    //FIXME: would be good to have *.exe for windows
    wxFileDialog dlg(this, "Choose Game Executable", "", "", "*.*", wxOPEN | wxFILE_MUST_EXIST);
    
    if (dlg.ShowModal() != wxID_OK)
        return;
    
    wxString file = dlg.GetFilename();
    wxString dir = dlg.GetDirectory();
    wxFileName fn(dir,file);

    GetExecPathIn()->SetValue(fn.GetFullPath());
}

void ExecInput::OnSelect( wxCommandEvent &event )
{
    wxString path, order, conn, pass, name;
    GameType *gtype = (GameType *)event.GetClientData();
    
    ReadExecOpts(gtype, &path, &order, &conn, &pass, &name);
    GetExecPathIn()->SetValue(path);
    GetExecOrderIn()->SetValue(order);
    GetExecConnIn()->SetValue(conn);
    GetExecPassIn()->SetValue(pass);
    GetExecNameIn()->SetValue(name);
}

void ExecInput::ReadExecOpts(const GameType *gtype, wxString *path, wxString *order, wxString *conn,
            wxString *pass, wxString *name)
{
    wxConfigBase *config = wxGetApp().GetConf();
    
    config->SetPath("/GameOpts");    
    config->Read(wxString::Format("Path%s", gtype->abbrev.c_str()), path);  
    config->Read(wxString::Format("Order%s", gtype->abbrev.c_str()), order, gtype->exec_order.c_str());
    config->Read(wxString::Format("Conn%s", gtype->abbrev.c_str()), conn, gtype->exec_connect.c_str());
    config->Read(wxString::Format("Pass%s", gtype->abbrev.c_str()), pass, gtype->exec_password.c_str());
    config->Read(wxString::Format("Name%s", gtype->abbrev.c_str()), name, gtype->exec_name.c_str());
}

//Format and (optionally) execute command
bool ExecInput::FormatCommand(const GameType *gtype, const wxString &host, int port,
                              const wxString &password, const wxString &name, 
                              wxString *out, bool execute)
{
    wxString path, order, conn, pass, nm;
    wxString command;
 
    ReadExecOpts(gtype, &path, &order, &conn, &pass, &nm);
    
    if (path.IsEmpty() || order.IsEmpty() || conn.IsEmpty())
        return false;
        
    VarMap vmap, optmap;

    vmap["host"] = host;
    vmap["port"] = wxString::Format("%d", port);

    optmap["connect_opt"] = VarSubst(conn, vmap);
    

    if (!password.IsEmpty() && !pass.IsEmpty())
    {
        vmap["password"] = password;
        optmap["password_opt"] = VarSubst(pass, vmap);
    }
    
    if (!name.IsEmpty() && !nm.IsEmpty())
    {
        vmap["name"] = name;
        optmap["name_opt"] = VarSubst(nm, vmap);
    }

    wxFileName exec(path);
    exec.Normalize();
    
    wxSetWorkingDirectory(exec.GetPath());  
 
    command = exec.GetFullPath() + _T(" ") + VarSubst(order, optmap);
    if (out) 
        *out = command;
    
    if (execute)
        wxExecute(command);   
 
    return true;
}




