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



#ifndef _SCOREBOT_H_
#define _SCOREBOT_H_
#include "pyemb.h"
#include "varstr.h"
#include "PlayerListCtrl.h"
#include "RuleListCtrl.h"

// !forwards a game query event and a timer event to alert, if set
// both will have id, aid
// alert's timer event handler can make changes before the game query is made
// the game query event handler can update the gui with the gdata. it is alright
// for it to take it

class ScoreBot: public wxEvtHandler
{
public:
    ScoreBot(wxEvtHandler *alert = NULL, int aid = -1, wxEvtHandler *broad = NULL, 
            int bid = -1, const wxString &server = "", int port = 0, const wxString &game = "")
        :m_alert(alert), m_alert_id(aid), m_broadcast(broad), 
        m_broadcast_id(bid), m_host(server), m_port(port), 
        m_game(game), m_timer(this, -1), m_interval(5),
        m_players(NULL), m_rules(NULL), m_last_info(NULL) 
    {
    }

    virtual ~ScoreBot() 
    {
        if (m_last_info) 
            delete m_last_info;
    }

    //perform operations needed to save data before restarting python, and reload afterwards
    virtual void Save() {}
    virtual bool Reload() { return true; }
    
    //interval is always in seconds. needs to be converted to ms
    void Start() { wxASSERT(m_interval >= 1); m_timer.Start(m_interval * 1000); }
    void Stop() { m_timer.Stop(); }
    bool IsRunning() const { return m_timer.IsRunning(); }
    void Pulse() { if (!IsRunning()) m_timer.Start(1, true); }
    wxString SubFormat(const wxString &format, VarMap &vmap); 
    wxString SubFormat(VarMap &vmap); 
    wxString SubFormat();
    virtual void Broadcast();
    virtual void BroadcastPlayers();

    void SetBroadcast(wxEvtHandler *broad, int bid = -1) { m_broadcast = broad; m_broadcast_id = bid; }
    void SetAlert(wxEvtHandler *alert, int aid = -1) { m_alert = alert; m_alert_id = aid; }
    void SetHost(const wxString &s) { m_host = s; }
    void SetPort(const int i) { m_port = i; }
    void SetFormat(const wxString &s) { m_oformat = s; }
    void SetChannel(const wxString &s) { m_irc_chan = s; }
    void SetInterval(const int i) 
    { 
        wxASSERT(i >= 1); 
        m_interval = i; 
        if (IsRunning())
            Start();
    }
    void SetTitle(const wxString &s) { m_title = s; }
    void SetNameA(const wxString &s) { m_name_a = s; }
    void SetNameB(const wxString &s) { m_name_b = s; }
    void SetScoreA(const wxString &s) { m_score_a = s; }
    void SetScoreB(const wxString &s) { m_score_b = s; }
    void SetTime(const wxString &s) { m_time = s; }
    void SetGame(const wxString &s) { m_game = s; }

    wxString GetHost() const { return m_host; }
    int GetPort() const { return m_port; }
    wxString GetFormat() const { return m_oformat; }
    wxString GetChannel() const { return m_irc_chan; }
    int GetInterval() const { return m_interval; }
    wxString GetTitle() const { return m_title; }
    wxString GetNameA() const { return m_name_a; }
    wxString GetNameB() const { return m_name_b; }
    wxString GetScoreA() const { return m_score_a; }
    wxString GetScoreB() const { return m_score_b; }
    wxString GetTime() const { return m_time; }
    wxString GetGame() const { return m_game; }

    void SetPlayers(PlayerListCtrl *p) { m_players = p; }
    void SetRules(RuleListCtrl *r) { m_rules = r; }
    
protected:
    void OnGameQueryM(PyEvent &evt);
    void OnTimerPulseM(wxTimerEvent &evt);
    virtual void OnTimerPulse(wxTimerEvent &evt);
    virtual void OnGameQuery(PyEvent &evt);

protected:
    //user requested info
    wxString m_host;
    int m_port;
    wxString m_oformat;
    wxString m_irc_chan;
    int m_interval; //in seconds
    wxString m_title;
    wxString m_name_a;
    wxString m_name_b;
    wxString m_score_a;
    wxString m_score_b;
    wxString m_time;

    wxString m_game; //game abbrev
    wxTimer m_timer;
    int m_alert_id;
    wxEvtHandler *m_alert; //to post query events to to alert user
    int m_broadcast_id;
    wxEvtHandler *m_broadcast; //to send IRC messages to

    RuleListCtrl *m_rules;
    PlayerListCtrl *m_players;
    
    RuleList m_last_rules;
    PlayerList m_last_players;
    GameInfo *m_last_info;

private:
    DECLARE_EVENT_TABLE()
};

#endif
