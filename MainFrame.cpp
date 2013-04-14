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



#include "wx/wx.h"
#include "wx/config.h"
#include "wx/filesys.h"
#include "wx/fs_zip.h"
#include "MainFrame.h"
#include "RuleListCtrl.h"
#include "PlayerListCtrl.h"
#include "QueryInput.h"
#include "pyemb.h"
#include "Fragmon.h"
#include "fragmon_wdr.h"
#include "ExecInput.h"
//#include "CopyFmt.h"
#include "OptionsDlg.h"
#include "ipcserv.h"
#include "QueryOpts.h"
#include "icon.xpm"
#include "CompOutput.h"
#include "IRCInput.h"
#include "ScorebotInput.h"
#include "BroadcastFmt.h"
#include "ChannelInput.h"

#ifdef __WXDEBUG__
//XXX arbitrary numbers
enum
{
    ID_DBG_RANDOM = 55000,
    ID_DBG_REFRESH
};
#endif

enum
{
    ID_SCOREBOT = 19999,
    ID_GQUERY,
    ID_IRC_CONN,
    ID_IRC_DISCONN
};

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_IPC_PING(MainFrame::OnPing)
    EVT_GAMEQUERY(ID_GQUERY, MainFrame::OnGameQuery)
    EVT_GAMEQUERY(ID_SCOREBOT, MainFrame::OnScorebotQuery)
    EVT_TIMER(ID_SCOREBOT, MainFrame::OnScorebotTimer)
    EVT_MENU(ID_QUERY_DBG, MainFrame::OnViewDbg)
    EVT_MENU(ID_QUERY_RLD, MainFrame::OnQueryRld)
    EVT_MENU(ID_OPTIONS, MainFrame::OnOptions)
    EVT_MENU(ID_SET_NAME, MainFrame::OnSetName)
    EVT_MENU(ID_IRC_CONN, MainFrame::OnIrcConn)
    EVT_MENU(ID_IRC_DISCONN, MainFrame::OnIrcDisconn)
    //EVT_MENU(ID_IRC_RECONN, MainFrame::OnIrcReconn)
    EVT_MENU(ID_IRC_NICK, MainFrame::OnIrcNick)
    EVT_MENU(ID_IRC_CHAN, MainFrame::OnIrcChan)
    EVT_MENU(ID_IRC_BC_CONF, MainFrame::OnIrcBcConf)
    EVT_MENU(ID_IRC_BC_START, MainFrame::OnIrcBcStart)
    EVT_MENU(ID_IRC_BC_STOP, MainFrame::OnIrcBcStop)
    EVT_MENU(ID_IRC_BC_PULSE, MainFrame::OnIrcBcPulse)
    EVT_MENU(ID_IRC_BC_PP, MainFrame::OnIrcBcPP)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(ID_HELP, MainFrame::OnHelp)
    EVT_IRC(-1, MainFrame::OnIrcEvent)
#ifdef __WXDEBUG__
    EVT_MENU(ID_DBG_RANDOM, MainFrame::OnRandPing)
    EVT_TIMER(ID_DBG_REFRESH, MainFrame::OnRandPingRefresh)
#endif
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MainStatBar, wxStatusBar)
END_EVENT_TABLE()
    

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString &title, const wxString &pinger,
            const wxPoint& pos, const wxSize& size, long style, const wxString& name)
            :wxFrame(parent, id, title, pos, size, style, name),
            m_irc(NULL), m_scorebot(NULL)
{
    SetIcon(wxICON(xicon));
    //SetIcon(wxIcon(xicon_xpm));

    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/Dimmensions");

    m_panel = new wxPanel(this, -1);
    wxBoxSizer *box = new wxBoxSizer(wxVERTICAL);
    m_qinput = new QueryInput(m_panel, ID_GQUERY);
    m_coutput = new CompOutput(m_panel, -1);
    m_splitter = new wxSplitterWindow(m_panel, -1, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE | wxSP_BORDER);
    m_players = new PlayerListCtrl(m_splitter, -1, m_coutput, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
    m_rules = new RuleListCtrl(m_splitter, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);

    int sashpos;
    config->Read("MainSashPos", &sashpos, 0);
    m_splitter->SplitVertically(m_players, m_rules, sashpos);
    m_splitter->SetMinimumPaneSize(25); //prevent unsplit
    
    box->Add(m_qinput, 0, wxALIGN_CENTER | wxBOTTOM, 5);
    box->Add(m_coutput, 0, wxALIGN_CENTER | wxBOTTOM, 5);
    box->Add(m_splitter, 1, wxEXPAND | wxALL, 5);
    
    m_panel->SetSizerAndFit(box);

    //Set statusbar
    m_statbar = new MainStatBar(this);
    SetStatusBar(m_statbar);
    m_statbar->SetConnected(false);

    //Set menu
    wxMenuBar *mb = MenuFunc();
   
#ifdef __WXDEBUG__
    wxMenu *debug = new wxMenu();
    debug->AppendCheckItem(ID_DBG_RANDOM, "Random Ping", "");
    mb->Append(debug, "&Debug");
    m_refresh.SetOwner(this, ID_DBG_REFRESH);
#endif
   
    m_irc_menu = mb->GetMenu(mb->FindMenu("IRC"));
    wxASSERT(m_irc_menu);
    SetMenuBar(mb);

    SetIrcMenuConnected(false);
    
    int width, height;
    config->Read("MainFrameW", &width, 800);
    config->Read("MainFrameH", &height, 600);
    SetSize(width,height);

    m_server.SetTarget(this);
    m_server.Create(ServServer::GetService());

    if (!pinger.IsEmpty())
        m_qinput->Ping(pinger);

    //Set up help
    wxFileSystem::AddHandler(new wxZipFSHandler);
    m_help.AddBook(wxGetApp().GetHelpFile(), true);
}

MainFrame::~MainFrame()
{
    wxConfigBase *config = wxGetApp().GetConf();

    wxGetApp().GetDebugFrame()->Destroy();

    config->SetPath("/Dimmensions");
    config->Write("MainSashPos", m_splitter->GetSashPosition());

    int width, height;
    GetSize(&width, &height);
    config->Write("MainFrameW", width);
    config->Write("MainFrameH", height);

    if (m_scorebot)
        delete m_scorebot;
    if (m_irc)
        delete m_irc;
}

void MainFrame::SetIrcMenuConnected(bool conn)
{
    wxMenuItem *mi = m_irc_menu->FindItemByPosition(0);
    m_irc_menu->Delete(mi);

    if (!conn)
    {
        m_statbar->SetConnected(false);
        m_irc_menu->Insert(0, ID_IRC_CONN, "&Connect...", "Connect to a new IRC server");
    }
    else
    {
        m_statbar->SetConnected(true);
        m_irc_menu->Insert(0, ID_IRC_DISCONN, "&Disconnect", "Quit from IRC server");
    }

    wxMenuItemList mlist = m_irc_menu->GetMenuItems();
    wxMenuItemList::Node *n = mlist.GetFirst();
    wxASSERT(n);

    n = n->GetNext();
    while (n)
    {
        n->GetData()->Enable(conn);
        n = n->GetNext();
    }
}

void MainFrame::OnGameQuery(PyEvent &evt)
{
    PyPrintDebug(false, "main win got query event\n");
    m_rules->ProcessEvent(evt);
    m_coutput->ProcessEvent(evt);
    //Players list needs to come after comp output so that it can override the score info if need be
    m_players->ProcessEvent(evt);
}

void MainFrame::OnViewDbg(wxCommandEvent &evt)
{
    wxGetApp().GetDebugFrame()->Show();
    wxGetApp().GetDebugFrame()->Raise();
}

void MainFrame::OnQueryRld(wxCommandEvent &evt)
{
    m_qinput->SaveList();
    wxString msg = "Reloading Python ";

    //Save options
    if (m_scorebot)
    {
        if (m_scorebot->IsRunning()) 
        {
            m_scorebot->Stop();
            msg << "(switching off scorebot)";
        }

        m_scorebot->Save();
    }
    msg << "...";

    m_statbar->SetIrcStatus(msg);
   
    //Reload python
    //XXX: no error check of PyEmb*
    PyPrintDebug(false, msg + "\n");
    PyEmbFinalize();
    PyEmbInit();

    //Instruct queryinput and scorebot to update themselves
    m_qinput->Reload();
    if (m_scorebot)
    {
        //m_scorebot->Reload();
        if (!m_scorebot->Reload())
        {
            PyPrintDebug(false, "Failed to reload scorebot. Maybe its class was removed?\n");
            delete m_scorebot;
            m_scorebot = NULL;
        }
    }
}

void MainFrame::OnOptions(wxCommandEvent &evt)
{
    OptionsDlg dlg(this, -1, "Options");
    dlg.ShowModal();
}

void MainFrame::OnPing(wxCommandEvent &evt)
{
    //bring window forward
    Raise();
    m_qinput->ProcessEvent(evt);
}

//XXX: it doesn't work well to set one name for multiple games 
void MainFrame::OnSetName(wxCommandEvent &evt)
{
    wxTextEntryDialog enter(this, "Enter game name", "Enter name", m_qinput->GetName());
    if (enter.ShowModal() == wxID_OK)
        m_qinput->SetName(enter.GetValue());
}

void MainFrame::OnExit(wxCommandEvent &evt)
{
    if (m_irc)
        m_irc->Disconnect();

    Close();
}

void MainFrame::OnAbout(wxCommandEvent &evt)
{
    wxMessageBox("Fragmon game query tool\n© 2006 Christopher Davis", "About");
}

void MainFrame::OnHelp(wxCommandEvent &evt)
{
    puts("SHOWING HELP");
    m_help.Display("index.html");
}

void MainFrame::OnIrcConn(wxCommandEvent &evt)
{
    IRCInput is(this, -1, "IRC Server");
    if (is.ShowModal() != wxID_OK)
        return;

    m_statbar->SetConnected(false);
    m_statbar->SetIrcStatus("Connecting...");

    IRC *irc = dynamic_cast<IRC *>(m_irc);
    if (!irc)
    {
        irc = new IRC(this, -1);
        PyEmbSetIrcTarget(irc);
        SetIrc(irc);
    }

    irc->ClearNicks();
    irc->AddNick(is.Nick1);
    irc->AddNick(is.Nick2);
    irc->AddNick(is.Nick3);

    wxString port = wxString::Format("%d", is.Port);
    
    if (is.HasPassword)
        irc->Connect(is.Host, port, is.Username, is.Realname, is.Password); 
    else
        irc->Connect(is.Host, port, is.Username, is.Realname);

    //XXX: no disconnect while connecting
    wxMenuItem *mi = m_irc_menu->FindItemByPosition(0);
    mi->Enable(false); 
}

void MainFrame::OnIrcDisconn(wxCommandEvent &evt)
{
    if (!m_irc)
        return;

    m_statbar->SetConnected(false);
    SetIrcMenuConnected(false);

    if (m_scorebot)
    {
        delete m_scorebot;
        m_scorebot = NULL;
    }
    
    m_statbar->SetIrcStatus("Quiting...");
    m_irc->Disconnect();
    m_statbar->SetIrcStatus("Disconnected.");
}

void MainFrame::OnIrcChan(wxCommandEvent &evt) 
{
    if (!m_irc)
        return;

    ChannelInput ci(this, -1, "Channels");
    ci.ShowModal();
}

void MainFrame::OnIrcNick(wxCommandEvent &evt) 
{
    if (!m_irc)
        return;

    wxTextEntryDialog enter(this, "Enter nick", "Enter nick", m_irc->GetNick());
    if (enter.ShowModal() == wxID_OK)
    {
        IRCEvent ie;
        ie << "nick" << enter.GetValue();
        wxPostEvent(m_irc, ie);
    }
}

void MainFrame::OnIrcBcConf(wxCommandEvent &evt)
{
    if (!m_irc)
        return;

    ScorebotInput si(this, -1, "Scorebot Configuration");
    if (si.Load() == wxID_OK)
    {
        m_scorebot = si.GetScorebot();
        if (m_scorebot)
        {
            m_scorebot->SetAlert(this, ID_SCOREBOT);
            m_scorebot->SetBroadcast(m_irc, -1);
            m_scorebot->SetPlayers(m_players);
            m_scorebot->SetRules(m_rules);
        }
    }
}

void MainFrame::OnIrcBcStart(wxCommandEvent &evt)
{
    if (!m_scorebot)
        return;

    m_statbar->SetIrcStatus("Starting Broadcast");
    m_scorebot->Start();
}

void MainFrame::OnIrcBcStop(wxCommandEvent &evt)
{
    if (!m_scorebot)
        return;

    m_statbar->SetIrcStatus("Stopped Broadcast");
    m_scorebot->Stop();
}

void MainFrame::OnIrcBcPulse(wxCommandEvent &evt)
{
    if (!m_scorebot)
        return;
    
    m_scorebot->Pulse();
}

void MainFrame::OnIrcBcPP(wxCommandEvent &evt)
{
    if (!m_scorebot)
        return;
    
    m_scorebot->BroadcastPlayers();
}

void MainFrame::OnIrcEvent(IRCEvent &evt)
{
    PyEmbCallIrcHooks(evt);
    
    if (evt.IsError)
    {
        if (!m_irc->IsConnected())
            SetIrcMenuConnected(false);

        m_statbar->SetIrcStatus(wxString::Format("Error: %s", evt.GetErrText().c_str()));
    }
    else if (evt.Command == RPL_WELCOME)
    {
        SetIrcMenuConnected(true);
        if (!evt.Args.IsEmpty())
            m_statbar->SetIrcStatus(evt.GetErrText());
        else
            m_statbar->SetIrcStatus("Logged In");
    }
    else if (evt.Command == RPL_JOINOK)
    {
        m_statbar->SetIrcStatus("Joined " + evt.Args[0]);
    }
}

void MainFrame::OnScorebotTimer(wxTimerEvent &evt)
{
    wxASSERT(m_scorebot);
    m_statbar->SetIrcStatus("Querying server...");
}

void MainFrame::OnScorebotQuery(PyEvent &evt)
{
    wxASSERT(m_scorebot);
    wxASSERT(m_irc);

    if (evt.GotError())
    {
        m_statbar->SetIrcStatus("Broadcast Error: " + evt.GetErrorMsg());
        return;
    }
    
    //Feed this into query input, and let it filter back down again to the list controls
    evt.SetId(-1);
    m_qinput->ProcessEvent(evt);
    m_statbar->SetIrcStatus("Broadcasting scores");

    //Make the scorebot data current
    m_scorebot->SetNameA(m_coutput->GetNameA());
    m_scorebot->SetNameB(m_coutput->GetNameB());
    m_scorebot->SetScoreA(m_coutput->GetScoreA());
    m_scorebot->SetScoreB(m_coutput->GetScoreB());
    m_scorebot->SetTime(m_coutput->GetTime());
    m_scorebot->SetFormat(BroadcastFmt::GetFormat());
}


#ifdef __WXDEBUG__
void MainFrame::OnRandPing(wxCommandEvent &evt)
{
    if (m_refresh.IsRunning())
    {
        m_refresh.Stop();
        m_qinput->SetQuiet(false);
    }
    else
    {
        wxTextEntryDialog txt(this, "Enter interval (ms)");
        if (txt.ShowModal() == wxID_OK)
        {
            wxString sival = txt.GetValue();
            long inter;
            if (sival.ToLong(&inter))
            {
                PyPrintDebug(false, "ping interval -> %d\n", inter);
                if (inter == 0)
                {
                    wxListBox *slist = m_qinput->GetServList();
                    //ping all :o
                    for (int i = 0; i < slist->GetCount(); i++)
                    {
                        ServerListEntry *sent = (ServerListEntry *) slist->GetClientObject(i);
                        wxASSERT(sent);
                        if (sent->game_type->abbrev == "Unk")
                            PyPrintDebug(false, "Skipping unk %s:%d\n", sent->host.c_str(), sent->port);
                        else
                        {
                            PyPrintDebug(false, "mega ping %s:%d\n", sent->host.c_str(), sent->port);
                            m_qinput->Ping(sent);
                        }
                    }
                }
                else
                {
                    m_refresh.Start(inter);
                    m_qinput->SetQuiet(true);
                }
            }
            else
                wxMessageBox("Bad interval");
        }
    }
}

void MainFrame::OnRandPingRefresh(wxTimerEvent &evt)
{
    wxListBox *slist = m_qinput->GetServList();
    int pick = rand() % slist->GetCount();
    ServerListEntry *sent = (ServerListEntry *) slist->GetClientObject(pick);
    wxASSERT(sent);

    PyPrintDebug(false, "random ping %s:%d\n", sent->host.c_str(), sent->port);
    m_qinput->Ping(sent);
}
#endif

MainStatBar::MainStatBar(wxWindow *parent)
    :wxStatusBar(parent, -1)
{
    static int widths[] = {-1, 90};
    static int fcount = sizeof(widths)/sizeof(int);

    SetFieldsCount(fcount);
    SetStatusWidths(fcount, widths);
}

void MainStatBar::SetConnected(bool con)
{
    m_connected = con;

    if (con)
        SetStatusText("IRC ON", CONNECTED);
    else
        SetStatusText("IRC OFF", CONNECTED);
}


