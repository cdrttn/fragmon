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
// Name:        QueryInput.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "QueryInput.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "QueryInput.h"
#include "pyemb.h"
#include "wx/config.h"
#include "Fragmon.h"
#include "PassLock.h"
#include "ExecInput.h"
#include "fragmon_wdr.h"
#include "wx/clntdata.h"
#include "varstr.h"
#include "wx/clipbrd.h"
#include "CopyFmt.h"
#include "ipcserv.h"
#include "QueryOpts.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// QueryInput
//----------------------------------------------------------------------------

// WDR: event table for QueryInput

BEGIN_EVENT_TABLE(QueryInput,wxPanel)
    EVT_IPC_PING(QueryInput::OnPing)
    EVT_GAMEQUERY(-1, QueryInput::OnQueryOutput)
    EVT_GOTPASS(ID_PASSLOCK, QueryInput::OnGotPass)
    EVT_TEXT_ENTER( ID_SERV_IN, QueryInput::OnQuery )
    EVT_BUTTON( ID_SERV_QUERY, QueryInput::OnQuery )
    EVT_BUTTON( ID_SERV_SAVE, QueryInput::OnSave )
    EVT_BUTTON( ID_DEL, QueryInput::OnDel )
    //EVT_LISTBOX( ID_SERV_LIST, QueryInput::OnListSelect )
    EVT_LISTBOX( ID_SERV_LIST, QueryInput::OnListClick )
    EVT_LISTBOX_DCLICK( ID_SERV_LIST, QueryInput::OnListClick )
    EVT_BUTTON( ID_SERV_EDIT, QueryInput::OnEdit )
    EVT_BUTTON( ID_SERV_PLAY, QueryInput::OnPlay )
    EVT_BUTTON( ID_SERV_COPY, QueryInput::OnCopy )
END_EVENT_TABLE()

QueryInput::QueryInput( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style ),
    m_unknown_type(-1, "Unknown", "Unk", -1),
    m_cur_info(NULL),
    m_quiet(false)
{
    PassLock *pl = new PassLock(this, ID_PASSLOCK);
    // WDR: dialog function QueryInputFunc for QueryInput
    QueryInputFunc( this, TRUE ); 

    pl->Hide();
    
    LoadList();
    ReadConf();
}


QueryInput::~QueryInput()
{
    SaveList();
    if (m_cur_info) delete m_cur_info;
}

void QueryInput::SaveList()
{
    int i;
    wxConfigBase *conf = wxGetApp().GetConf();

    //Re create list to delete excess servers
    //conf->DeleteGroup("/GameServers");   
 
    conf->SetPath("/GameServers");
   
    conf->Write("UserName", m_name);
    
    wxListBox *sl = GetServList();
    
    for (i=0; i < sl->GetCount(); i++)
    {
        ServerListEntry *sent = dynamic_cast<ServerListEntry *>(sl->GetClientObject(i));

        conf->Write(wxString::Format("Server%d", i), sent->host);
        conf->Write(wxString::Format("Port%d", i), sent->port);
        conf->Write(wxString::Format("Type%d", i), sent->game_type->abbrev);
        conf->Write(wxString::Format("Password%d", i), sent->password);
    }

    while (1)
    {
        wxString s = wxString::Format("Server%d", i);
        if (!conf->HasEntry(s))
            break;

        conf->DeleteEntry(s);
        conf->DeleteEntry(wxString::Format("Port%d", i));
        conf->DeleteEntry(wxString::Format("Type%d", i));
        conf->DeleteEntry(wxString::Format("Password%d", i));

        i++;
    }
    
}

void QueryInput::Reload()
{
    wxString gname = GetServType()->GetStringSelection();

    GetServType()->Clear();
    GetServList()->Clear();
    m_gamelist.Clear();
    
    LoadList();
    ReadConf();

    GetServType()->SetStringSelection(gname);
}

void QueryInput::LoadList()
{
    PyGetGameList(m_gamelist);
    wxChoice *stype = GetServType();
    
    //Add default item
    stype->Append(m_unknown_type.name, (void *)&m_unknown_type);
    
    for (int i=0; i < m_gamelist.GetCount(); i++)
        stype->Append(m_gamelist[i].name, (void *)&m_gamelist[i]);
    
    stype->SetSelection(0);
}

void QueryInput::ReadConf()
{
    //Load servers
    wxConfigBase *conf = wxGetApp().GetConf();
    wxString entry;
    long yup;
    
    conf->SetPath("/GameServers");
    conf->Read("UserName", &m_name);

    wxListBox *sl = GetServList();
    int i = 0;
    while (1)
    {
        wxString server, password, type;
        int port = -1;
        GameType *gtype = NULL;
        ServerListEntry *sent;

        conf->Read(wxString::Format("Server%d", i), &server);
        conf->Read(wxString::Format("Port%d", i), &port);
        conf->Read(wxString::Format("Type%d", i), &type);
        conf->Read(wxString::Format("Password%d", i), &password);
  
        if (server.IsEmpty() || port == -1 || type.IsEmpty())
            break;

        sent = new ServerListEntry(&m_unknown_type, server, port, password);
        
        for (int j=0; j < m_gamelist.GetCount(); j++)
            if (type == m_gamelist[j].abbrev)
                sent->game_type = &m_gamelist[j];

        sl->Append(sent->MakeFullServer(), sent);
        
        i++;
    }
}

// WDR: handler implementations for QueryInput

void QueryInput::OnCopy( wxCommandEvent &event )
{
    if (!m_cur_info)
        return;

    wxString copy = CopyFmt::FormatCopy(m_cur_info);
    
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(copy));
        wxTheClipboard->Close();
    }

}

void QueryInput::OnPlay( wxCommandEvent &event )
{
    GameType *gtype = (GameType *) GetServType()->GetClientData(GetServType()->GetSelection());
    //wxString command;

    wxString host;
    int port;
    if (!GetServIn()->ParseInput(host,port))
    {
        wxMessageBox("Invalid server input", "Error");
        return;
    }    

    wxString passwd = GetPasslock()->GetPassword();
    
    if (!ExecInput::FormatCommand(gtype, host, port, passwd, m_name))
    {
        wxMessageBox("Game not set up", "Error");
        return;
    }

    //Update: FormatCommand also executes command
    //wxExecute(command);
}

void QueryInput::OnEdit( wxCommandEvent &event )
{
    int sel = GetServList()->GetSelection();
    GameType *gtype = (GameType *) GetServType()->GetClientData(GetServType()->GetSelection());

    if (sel == wxNOT_FOUND)
    {
        wxMessageBox("No server selected to edit", "Error");
        return;
    }
    
    //make a copy. Delete method will delete the old one
    ServerListEntry *old = dynamic_cast<ServerListEntry *>(GetServList()->GetClientObject(sel));
    ServerListEntry *sent = new ServerListEntry(*old);
  
    wxString host;
    int port;
    if (!GetServIn()->ParseInput(host,port))
    {
        wxMessageBox("Invalid server input", "Error");
        return;
    }
    
    sent->game_type = gtype;
    sent->host = host;
    sent->port = port;
    sent->password = GetPasslock()->GetPassword();
    
    GetServList()->Delete(sel);
    GetServList()->Append(sent->MakeFullServer(), sent);
}

//FIXME: think more about these event handlers
void QueryInput::OnListClick( wxCommandEvent &event )
{
    //On dbl click do everything for a selection, but also query
    OnListSelect(event);
    OnQuery(event);
}

void QueryInput::OnListSelect( wxCommandEvent &event )
{
    ServerListEntry *sent = dynamic_cast<ServerListEntry *>(event.GetClientObject());
    //GetServType()->SetSelection(sent->game_type->id);
    SetSelServType(sent->game_type->id);
    
    if (sent->password.IsEmpty())
        GetPasslock()->Hide();
    else
        GetPasslock()->Show();

    GetPasslock()->SetPassword(sent->password);
    GetServIn()->SetValue(sent->MakeServer());
}

void QueryInput::OnDel( wxCommandEvent &event )
{
    int sel = GetServList()->GetSelection();
    
    if (sel != wxNOT_FOUND)
        GetServList()->Delete(sel);

}

//see if Server list contains sent
ServerListEntry *QueryInput::HasServer(const ServerListEntry *sent) 
{
    wxListBox *sl = GetServList();
    
    for (int i = 0; i < sl->GetCount(); i++)
    {
        ServerListEntry *test = dynamic_cast<ServerListEntry *>(sl->GetClientObject(i));
        if (test->host == sent->host && test->port == sent->port)
            return test;
    }
    
    return NULL;
}

//check to see if current server is saved
ServerListEntry *QueryInput::HasServer() 
{
    /*
    wxChoice *stype = GetServType();
    int sel = stype->GetSelection();
    GameType *info = (GameType *)stype->GetClientData(sel);
    wxString host;
    int port;
    port = info->defport;
    ServerListEntry *find = NULL;

    if (GetServIn()->ParseInput(host, port))
    {
        ServerListEntry test;
        test.host = host;
        test.port = port;
        find = HasServer(&test);
    }
    */

    ServerListEntry *find = NULL;
    wxString host, abbrev;
    int port;
   
    if (GetCurrentServer(host, port, abbrev))
    {
        ServerListEntry test;
        test.host = host;
        test.port = port;
        find = HasServer(&test);
    }
    else
        wxMessageBox("Invalid Server.");

    return find;
}

bool QueryInput::GetCurrentServer(wxString &host, int &port, wxString &abbrev)
{
    wxChoice *stype = GetServType();
    int sel = stype->GetSelection();
    GameType *info = (GameType *)stype->GetClientData(sel);
  
    wxString h;
    int p;
    
    p = info->defport;
    if (GetServIn()->ParseInput(h, p))
    {
        host = h;
        port = p;
        abbrev = info->abbrev;
        return true;
    }

    return false;
}

bool QueryInput::SetCurrentServer(const wxString &host, int port, const wxString &abbrev)
{
    bool typeset = false;

    wxChoice *stype = GetServType();
    for (int i = 0; i < stype->GetCount(); i++)
    {
        GameType *info = (GameType *)stype->GetClientData(i);
        if (info->abbrev == abbrev)
        {
            typeset = true;
            stype->SetSelection(i);
            break;
        }
    }

    if (!typeset)
        return false;

    GetServIn()->SetValue(wxString::Format("%s:%d", host.c_str(), port));
    return true;
}

void QueryInput::OnSave( wxCommandEvent &event )
{
    wxChoice *stype = GetServType();
    int sel = stype->GetSelection();
    GameType *gtype = (GameType *)stype->GetClientData(sel);
    wxString host;
    int port = gtype->defport; //set default port
    
    //Only add to box if parseable
    if (GetServIn()->ParseInput(host, port))
    {
        ServerListEntry *sent = new ServerListEntry(gtype, host, port,
                                                GetPasslock()->GetPassword());
       
        if (HasServer(sent))
        {
            wxMessageBox("Server already saved");
            delete sent;
        }
        else
            GetServList()->Append(sent->MakeFullServer(), sent);
    }
    else
        wxMessageBox("Invalid Server.");
}

void QueryInput::OnQuery( wxCommandEvent &event )
{
    wxString host, abbrev;
    int port;

    if (!GetCurrentServer(host, port, abbrev))
    {
        wxMessageBox("Invalid Server.");
        return;
    }

    if (abbrev == "Unk")
        wxMessageBox("Unknown server");
    else 
        new GameQueryThread(this, -1, abbrev, host, port, GameQueryThread::ALL);
}

void QueryInput::OnQueryOutput(PyEvent &evt)
{
    if (evt.GotError())
    {
        if (!m_quiet)
            wxMessageBox(evt.GetErrorMsg(), "Query Error");
        return;
    }
    
    if (m_cur_info)
        delete m_cur_info;
 
    //GameData *gdata = 
    GameInfo *ginfo = evt.GetGdata()->info;
    m_cur_info = new GameInfo(*ginfo); 

    GetServPing()->SetValue(wxString::Format("%d", m_cur_info->ping));
    GetServHost()->SetValue(m_cur_info->hostname);
    GetServMap()->SetValue(m_cur_info->map);
    GetServPlayers()->SetValue(wxString::Format("%d/%d", m_cur_info->curplayers, m_cur_info->maxplayers));
    GetServGtype()->SetValue(m_cur_info->type);
    GetServVers()->SetValue(m_cur_info->version);
    GetServMod()->SetValue(m_cur_info->mod);
    
    if (m_cur_info->password)
        GetPasslock()->Show();
    else
        GetPasslock()->Hide();

    /*
    puts("QueryInput -> Taking Data");
    printf("%25s: %d\n", "Ping", m_cur_info->ping);
    printf("%25s: %s\n", "Hostname", m_cur_info->hostname.c_str());
    printf("%25s: %s\n", "Map", m_cur_info->map.c_str());
    printf("%25s: %d\n", "Curplayers", m_cur_info->curplayers);
    printf("%25s: %d\n", "Maxplayers", m_cur_info->maxplayers);
    printf("%25s: %s\n", "Type", m_cur_info->type.c_str());

    printf("%25s: %s\n", "Passwd?", (m_cur_info->password)? "Yes" : "No"); 
    */ 

    evt.SetId(GetId());
    evt.Skip();
}

void QueryInput::OnGotPass(wxCommandEvent &evt)
{
    ServerListEntry *find;
    find = HasServer();
    if (find)
        find->password = evt.GetString();
}

void QueryInput::OnPing(wxCommandEvent &evt)
{
    //use default game for external ping request
    wxString defgame = QueryOpts::GetDefaultGame();
    SetSelServType(0);

    for (int i = 0; i < m_gamelist.GetCount(); i++)
        if (m_gamelist[i].abbrev == defgame)
            SetSelServType(i);
    

    GetServIn()->SetValue(evt.GetString());
    OnQuery(evt);
}

void QueryInput::Ping(const wxString &addy)
{
    wxCommandEvent evt;
    evt.SetString(addy);
    OnPing(evt);
}

void QueryInput::Ping(ServerListEntry *server)
{
    //Direct ping
    new GameQueryThread(this, -1, server->game_type->id, server->host, server->port, GameQueryThread::ALL);
}

