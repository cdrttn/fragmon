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



#ifndef _IRC_H_
#define _IRC_H_ 

//irc stuff
#include "wx/wx.h"
#include "wx/socket.h"
#include "ircbase.h"
#include "ircresp.h"
#include "ircmask.h"
#include "ircevent.h"

#define IRC_BUFTHRESH 4096 //bytes queue in buf past this
#define IRC_CMDTHRESH 3
#define IRC_THRESHTIME 2000 //reset buf threshold every 2 sec
#define IRC_FLUSHTIME 1000 //flush buf every sec

class IRC : public IRCBase
{
public:
    IRC(wxEvtHandler *tgt, int winid=wxID_ANY, bool autorecon=true, bool autojoin=true, 
            int buf_threshold=IRC_BUFTHRESH, int cmd_threshold=IRC_CMDTHRESH, 
            int buf_threshold_time=IRC_THRESHTIME, int flush_interval=IRC_FLUSHTIME);
    void Connect(const wxString &host, const wxString &service, const wxString &username, 
                    const wxString &realname, const wxString &pass = "", const wxString &nick = "");  
    void Connect()
    {
        Connect(m_host, m_service, m_username, m_realname, m_password, m_myhostmask.Nick);
    }
   
    void Disconnect();
    
    void Close() 
    {
        m_connecting = false; 
        m_connected = false; 
        if (m_sock.IsConnected())
            m_sock.Close(); 
    }

    bool IsConnected() const { return m_connected; }
    
    void Quote(const wxString &text, bool bypass = false);
    void AddNick(const wxString &choice) { m_nicklist.Add(choice); }
    void ClearNicks() { m_nicklist.Empty(); }
    void Pong(const wxString &daemon)
    {
        Quote("PONG :" + daemon, true); //jump to first in line in Q if necessary
    }

    void Nick(const wxString &nick)
    {
        Quote("NICK :" + nick); 
    }

    void Join(const wxString &chan)
    {
        Quote("JOIN :" + chan);
    }

    void Part(const wxString &chan)
    {
        Quote("PART :" + chan);
    }

    //Force quit through no matter what. Bypass queue
    void Quit(const wxString &exit)
    {
        m_quitsent = true;
        Quote("QUIT :" + exit, true);
        if (!m_sock.WaitForLost(3))
            Close();
    }

    wxString GetNick() const { return m_myhostmask.Nick; }
    IRCMask GetHostmask() const { return m_myhostmask; }
    void Msg(const wxString &target, const wxString &text);
    static wxString GetSockErrText(int id);

    wxArrayString GetChanList() const { return m_chanlist; }
    void SetAutoRecon(bool autorecon) { m_autorecon = autorecon; }
    bool GetAutoRecon() { return m_autorecon; }
    void SetAutoRejoin(bool rj) { m_rejoin = rj; }
    bool GetAutoRejoin() { return m_rejoin; }
    void SetBufThreshold(int thresh) { m_buf_threshold = thresh; }
    int GetBufThreshold() { return m_buf_threshold; }
    void SetCmdThreshold(int thresh) { m_cmd_threshold = thresh; }
    int GetCmdThreshold() { return m_cmd_threshold; }
    void SetFlushInterval(int intr) { m_buf_flush_timer.Start(intr); } 
    int GetFlushInterval() { return m_buf_flush_timer.GetInterval(); }
    void SetThresholdTime(int intr) { m_buf_reset_timer.Start(intr); } 
    int GetThresholdTime() { return m_buf_reset_timer.GetInterval(); } 

    static bool ParseMsg(const wxString &msg, wxString &Source, wxString &Command, wxArrayString &Args);

protected:
    void OnSockEvent(wxSocketEvent &evt);
    void OnFlush(wxTimerEvent &evt);
    void OnReset(wxTimerEvent &evt);
    virtual void OnUserMsg(IRCEvent &evt);
    
protected:
    bool m_connected;
    bool m_connecting;
    bool m_autorecon;
    bool m_quitsent;
    bool m_rejoin;
    int m_nickiter;
    wxArrayString m_nicklist;
    wxArrayString m_chanlist;
    IRCMask m_myhostmask;

    wxSocketClient m_sock;
    wxString m_tmp_buf;
    wxString m_host;
    wxString m_service;
    wxString m_username;
    wxString m_realname;
    wxString m_password;

    wxArrayString m_output_buf;
    size_t m_buf_threshold;
    size_t m_cmd_threshold;
    size_t m_buf_current;
    size_t m_cmd_current;
    wxTimer m_buf_flush_timer;
    wxTimer m_buf_reset_timer;

    bool SetNextNick();
    wxString GetNextNick()
    {
        wxString nick;
        if (m_nickiter < m_nicklist.GetCount())
            nick = m_nicklist[m_nickiter++];
        return nick;
    }
    
    void WaitForConnection();
    void SendErr(const wxString &msg = "Connection Closed", bool dead_link = true);
    void SendWarn(const wxString &msg) { SendErr(msg, false); }
    void SendMsg(const wxString &msg);
    void SendMsg(const wxString &src, const wxString &command, const wxArrayString &args);
    wxArrayString ReadMsg(int wait = 0);
    
private:
    DECLARE_EVENT_TABLE()
};

#endif
