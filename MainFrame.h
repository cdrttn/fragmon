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



#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wx/wx.h"
#include "wx/html/helpctrl.h"
#include "RuleListCtrl.h"
#include "PlayerListCtrl.h"
#include "QueryInput.h"
#include "pyemb.h"
#include "ipcserv.h"
#include "CompOutput.h"
#include "irc.h"
#include "scorebot.h"
#include "wx/html/helpctrl.h"

class MainStatBar;
class MainFrame;

class MainFrame : public wxFrame
{
public:
    MainFrame(wxWindow* parent, wxWindowID id, const wxString &title, const wxString &pinger, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "panel");
    ~MainFrame();

    IRCBase *GetIrc() { return m_irc; }
    void SetIrc(IRCBase *irc) 
    {
        if (m_irc && m_irc != irc)
            delete m_irc;
        m_irc = irc; 
    }

    ScoreBot *GetScorebot() { return m_scorebot; }
    void SetScorebot(ScoreBot *bot)
    {
        if (m_scorebot && m_scorebot != bot)
            delete m_scorebot;
        m_scorebot = bot;
    }
    
    QueryInput *GetQueryInput() { return m_qinput; }

private:
    void SetIrcMenuConnected(bool conn);
    
private:
    void OnGameQuery(PyEvent &evt);
    void OnScorebotQuery(PyEvent &evt);
    void OnScorebotTimer(wxTimerEvent &evt);

    //menus
    void OnViewDbg(wxCommandEvent &evt);
    void OnQueryRld(wxCommandEvent &evt);
    void OnPing(wxCommandEvent &evt);
    void OnOptions(wxCommandEvent &evt);
    void OnSetName(wxCommandEvent &evt);
    void OnExit(wxCommandEvent &evt);
    void OnAbout(wxCommandEvent &evt);
    void OnHelp(wxCommandEvent &evt);
    void OnIrcConn(wxCommandEvent &evt);
    void OnIrcDisconn(wxCommandEvent &evt);
    //void OnIrcReconn(wxCommandEvent &evt);
    void OnIrcChan(wxCommandEvent &evt); 
    void OnIrcNick(wxCommandEvent &evt); 
    void OnIrcBcConf(wxCommandEvent &evt);
    void OnIrcBcStart(wxCommandEvent &evt);
    void OnIrcBcStop(wxCommandEvent &evt);
    void OnIrcBcPulse(wxCommandEvent &evt);
    void OnIrcBcPP(wxCommandEvent &evt);

    void OnIrcEvent(IRCEvent &evt);

private:
    wxPanel *m_panel;
    QueryInput *m_qinput;
    CompOutput *m_coutput;
    wxSplitterWindow *m_splitter;
    RuleListCtrl *m_rules;
    PlayerListCtrl *m_players;
    ServServer m_server;
    MainStatBar *m_statbar;
    IRCBase *m_irc;
    wxMenu *m_irc_menu;
    ScoreBot *m_scorebot;
    wxHtmlHelpController m_help;

private:
    DECLARE_EVENT_TABLE()

#ifdef __WXDEBUG__
private:
    void OnRandPing(wxCommandEvent &evt);
    void OnRandPingRefresh(wxTimerEvent &evt);
private:
    wxTimer m_refresh;
#endif
};

class MainStatBar: public wxStatusBar
{
public:
    MainStatBar(wxWindow *parent);
    void SetConnected(bool con);
    bool GetConnected() { return m_connected; }
    void SetIrcStatus(const wxString &stat) { SetStatusText(stat, STATUS); }

private:
    bool m_connected;

private:
    DECLARE_EVENT_TABLE()

private:
    enum
    {
        STATUS = 0,
        CONNECTED
    };
};

#endif
