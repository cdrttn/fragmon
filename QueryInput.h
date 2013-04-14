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
// Name:        QueryInput.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __QueryInput_H__
#define __QueryInput_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "QueryInput.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "fragmon_wdr.h"
#include "pyemb.h"
#include "ServerEntry.h"
#include "PassLock.h"
#include "wx/clntdata.h"

// WDR: class declarations
class QueryInputDialog;
class ServerListEntry;
class QueryInput;


class ServerListEntry : public wxClientData
{
public:
    ServerListEntry(GameType *gt = NULL, const wxString &srv = "", 
                    int port = 0, const wxString &pw = "")
                   : game_type(gt), host(srv), port(port), password(pw){}
    ServerListEntry(const ServerListEntry &sent): 
        game_type(sent.game_type), host(sent.host), port(sent.port), password(sent.password){}
    //virtual ~ServerListEntry() {}
    
    wxString MakeServer() const { return wxString::Format("%s:%d", host.c_str(), port); }
    wxString MakeFullServer() const
    {
        wxASSERT(game_type != NULL);
        return wxString::Format("%s / %s:%d", game_type->abbrev.c_str(), host.c_str(), port); 
    }

    GameType *game_type;
    wxString host;
    int port;
    wxString password;
    //wxString name; 
};


//----------------------------------------------------------------------------
// QueryInput
//----------------------------------------------------------------------------

class QueryInput: public wxPanel
{
public:
    // constructors and destructors
    QueryInput( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    ~QueryInput();
    
    // WDR: method declarations for QueryInput
    wxButton* GetServQuery()  { return (wxButton*) FindWindow( ID_SERV_QUERY ); }
    wxTextCtrl* GetServVers()  { return (wxTextCtrl*) FindWindow( ID_SERV_VERS ); }
    wxTextCtrl* GetServMod()  { return (wxTextCtrl*) FindWindow( ID_SERV_MOD ); }
    PassLock* GetPasslock()  { return (PassLock*) FindWindow( ID_PASSLOCK ); }
    ServerEntry* GetServIn()  { return (ServerEntry*) FindWindow( ID_SERV_IN ); }
    wxTextCtrl* GetServMap()  { return (wxTextCtrl*) FindWindow( ID_SERV_MAP ); }
    wxTextCtrl* GetServGtype()  { return (wxTextCtrl*) FindWindow( ID_SERV_GTYPE ); }
    wxTextCtrl* GetServPlayers()  { return (wxTextCtrl*) FindWindow( ID_SERV_PLAYERS ); }
    wxTextCtrl* GetServHost()  { return (wxTextCtrl*) FindWindow( ID_SERV_HOST ); }
    wxTextCtrl* GetServPing()  { return (wxTextCtrl*) FindWindow( ID_SERV_PING ); }
    wxChoice* GetServType()  { return (wxChoice*) FindWindow( ID_SERV_TYPE ); }
    wxListBox* GetServList()  { return (wxListBox*) FindWindow( ID_SERV_LIST ); }
    void Reload();
    void SaveList();
    void Ping(const wxString &addy);
    void Ping(ServerListEntry *server);
    void SetName(const wxString &name) { m_name = name; }
    wxString GetName() { return m_name; }
    void SetQuiet(const bool q) { m_quiet = q; } 
    bool GetQuiet() const { return m_quiet; }
    bool GetCurrentServer(wxString &host, int &port, wxString &abbrev);
    bool SetCurrentServer(const wxString &host, int port, const wxString &abbrev);

private:
    // WDR: member variable declarations for QueryInput
    bool m_quiet;
    wxString m_name;
    //info from last successful query
    GameInfo *m_cur_info;

    GameList m_gamelist;
    GameType m_unknown_type;

private:
    // WDR: handler declarations for QueryInput
    void OnCopy( wxCommandEvent &event );
    void OnPlay( wxCommandEvent &event );
    void OnEdit( wxCommandEvent &event );
    void OnListClick( wxCommandEvent &event );
    void OnListSelect( wxCommandEvent &event );
    void OnDel( wxCommandEvent &event );
    void OnSave( wxCommandEvent &event );
    void OnQuery( wxCommandEvent &event );
    void OnQueryOutput(PyEvent &evt);
    void OnGotPass(wxCommandEvent &evt);
    void OnPing(wxCommandEvent &evt);
    
private:
    //my stuff
    //bool ParseInput(wxString &host, int &port);
    void ReadConf();
    void LoadList();
    ServerListEntry *HasServer(const ServerListEntry *sent);
    ServerListEntry *HasServer();
    void SetSelServType(int id) { GetServType()->SetSelection( (id < 0)? 0 : id+1 ); } //to account for unknown sel

private:
    DECLARE_EVENT_TABLE()
};

class QueryInputDialog : public wxDialog
{
public:
    QueryInputDialog( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE )
        :wxDialog(parent, id, title, pos, size, style), m_port(-1)
    {
        wxBoxSizer *box = new wxBoxSizer(wxVERTICAL);
        m_qinput = new QueryInput(this, -1);
        box->Add(m_qinput, 1, wxEXPAND | wxBOTTOM, 15);
        box->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxALIGN_CENTER);
        SetSizerAndFit(box);
    }

    virtual bool Validate()
    {
        if (!m_qinput->GetCurrentServer(m_host, m_port, m_abbrev) || m_abbrev == "Unk")
        {
            wxMessageBox("Invalid or unknown server entered.");
            return false;
        }

        return true;
    }

    void GetServer(wxString &host, int &port, wxString &abbrev)
    {
        host = m_host;
        port = m_port;
        abbrev = m_abbrev;
    }
  
    bool SetServer(const wxString &host, int port, const wxString &abbrev)
    {
        return m_qinput->SetCurrentServer(host, port, abbrev);
    }
    
private:
    QueryInput *m_qinput;
    wxString m_host;
    wxString m_abbrev;
    int m_port;
};


#endif
