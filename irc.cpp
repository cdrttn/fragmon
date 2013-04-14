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
#include "wx/socket.h"
#include "wx/tokenzr.h"
#include "irc.h"
#include "Fragmon.h"

enum 
{
    IRC_SOCKET=1,
    IRC_FLUSH,
    IRC_RESET
};

BEGIN_EVENT_TABLE(IRC, IRCBase)
    EVT_SOCKET(IRC_SOCKET, IRC::OnSockEvent)
    EVT_TIMER(IRC_FLUSH, IRC::OnFlush)
    EVT_TIMER(IRC_RESET, IRC::OnReset)
END_EVENT_TABLE()


IRC::IRC(wxEvtHandler *tgt, int winid, bool autorecon, bool autojoin, 
        int buf_threshold, int cmd_threshold, int buf_threshold_time, int flush_interval)
        :IRCBase(tgt, winid), m_buf_reset_timer(this, IRC_RESET), m_buf_flush_timer(this, IRC_FLUSH), 
        m_buf_threshold(buf_threshold),m_buf_current(0), 
        m_cmd_threshold(cmd_threshold),m_cmd_current(0),
        m_connected(false), m_connecting(false),
        m_nickiter(0), m_autorecon(autorecon),
        m_quitsent(false), m_rejoin(autojoin)
{
    m_sock.SetFlags(wxSOCKET_NOWAIT);
    m_sock.SetEventHandler(*this, IRC_SOCKET);
    m_sock.SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    m_sock.Notify(true);

    m_buf_reset_timer.Start(buf_threshold_time);
    m_buf_flush_timer.Start(flush_interval);
}

void IRC::WaitForConnection()
{
    int waits = 10;

    while (!m_connected && waits)
    {
        wxString src, cmd;
        wxArrayString args;
        wxArrayString lines = ReadMsg(5);
        if (lines.IsEmpty())
            waits--;

        for (int i = 0; i < lines.GetCount(); i++)
        {
            SendMsg(lines[i]);
        }
    }
}

void IRC::Connect(const wxString &host, const wxString &service, const wxString &username, 
        const wxString &realname, const wxString &pass, const wxString &nick)
{
    wxString src, cmd;
    wxArrayString args;
    wxArrayString lines;

    //avoid reentering this function
    //XXX: this shit doesn't work WTFever
    if (m_connecting)
    {
        return;
    }

    m_sock.Notify(false);
    m_connecting = true;
    m_nickiter = 0;
    
    m_host = host;
    m_service = service;
    m_username = username;
    m_realname = realname;
    m_password = pass;
    
    wxIPV4address addr;
    addr.Hostname(host);
    addr.Service(service);

    m_sock.SetTimeout(10);
    m_sock.Connect(addr, false);
    m_sock.WaitOnConnect();

    if (!m_sock.IsConnected())
    {
        SendErr("Invalid host");
        return;
    }

    if (!pass.empty())
        Quote("PASS :" + pass);

    if (!nick.empty())
        Nick(nick);
    else
    {
        if (!SetNextNick())
        {
            SendErr("Out of nick choices");
            return;
        }
    }

    Quote("USER " + username + " somehost someserv :" + realname);
    WaitForConnection(); 
    
    if (!m_connected)
    {
        SendErr("Connection Failed");
        return;
    }
   
    Quote("WHOIS " + m_myhostmask.Nick);
   
    for (int i = 0; i < m_chanlist.GetCount(); i++)
        Join(m_chanlist[i]);
    
    m_sock.Notify(true);
    m_connecting = false;
}

void IRC::Disconnect()
{
    Quit(wxString::Format("%s scorebot %s %s", FM_APPNAME, FM_VERSION, FM_WEBSITE));
}

void IRC::Quote(const wxString &text, bool bypass)
{
    wxString str = text + "\r\n";

    m_buf_current += str.Len();
    m_cmd_current++;

    if (m_buf_current > m_buf_threshold || m_cmd_current > m_cmd_threshold || !m_output_buf.IsEmpty())
    {
        if (bypass)
            m_output_buf.Insert(str, 0);
        else
            m_output_buf.Add(str);
    }
    else
        m_sock.Write(str.c_str(), str.Len());
}


bool IRC::SetNextNick()
{
    wxString nick = GetNextNick();

    if (nick.empty())
    {
        SendErr("No more nick choices");
        return false;
    }

    Nick(nick);
    return true;
}

void IRC::Msg(const wxString &target, const wxString &text)
{
    wxStringTokenizer tok(text, _T("\r\n"));
    
    while (tok.HasMoreTokens())
    {
        wxString s = tok.GetNextToken();
        s.Trim(); s.Trim(false);
        if (!s.empty())
        {
            wxString msg;
            msg.Printf("PRIVMSG %s :%s", target.c_str(), s.c_str());
            
            Quote(msg);

            //Echo this to user
            SendMsg(_T(":") + m_myhostmask.MakeString() + _T(" ") + msg);
        }
    }
}

//IRC Message reception

/* IRC Message :
 * [:SrcPrefix] Command [arg1 arg2 ...] [:suff ix str ing] \r\n
 */
/* Parse a single IRC msg line and fire an IRC MSG event 
 * Automatically respond to PINGS with PONG
 */

bool IRC::ParseMsg(const wxString &msg, wxString &Source, wxString &Command, wxArrayString &Args)
{
    wxStringTokenizer tok(msg, _T(" \t"));

    if (msg[0] == ':')
    {
        Source = tok.GetNextToken(); 
        Source.Remove(0, 1);
    }

    Command = tok.GetNextToken();

    if (Command.IsEmpty())
        return false;
   
    while (tok.HasMoreTokens())
    {
        wxString s = tok.GetString();

        if (s[0] == ':')
        {
            Args.Add(s.Mid(1));
            break;
        }
        else
            Args.Add(tok.GetNextToken());
    }

    return true;
}

void IRC::SendMsg(const wxString &msg)
{
    wxString src, cmd; 
    wxArrayString args;

    if (!ParseMsg(msg, src, cmd, args))
        return;

    SendMsg(src, cmd, args);
}

void IRC::SendMsg(const wxString &src, const wxString &command, const wxArrayString &args)
{
    IRCEvent evt(m_target_id);
    evt.Source = src;
    evt.Command = command;
    evt.Args = args;
    IRCMask mask(evt.Source);
    int acount = evt.Args.GetCount();
    wxString cmd = evt.Command.Lower();
    bool mymask = (mask.Nick == m_myhostmask.Nick);

    if (cmd == "ping")
    {
        Pong(evt.Args[0]);
    }
    else if (cmd == RPL_WELCOME)
    {
        if (!evt.Args.GetCount() >= 1)
        {
            SendErr("Malformed welcome message");
            return;
        }
        m_myhostmask.Nick = evt.Args[0];
        m_connected = true; //set as response to sucesful login
    }
    else if (cmd == "nick" && mymask && acount >= 1)
    {
        m_myhostmask.Nick = evt.Args[0];
    }
    else if (cmd == "join" && mymask && acount >= 1)
    {
        wxString chan = evt.Args[0].Lower();
        JoinOk(chan);
        if (m_chanlist.Index(chan) < 0)
            m_chanlist.Add(chan);
    }
    else if (cmd == "part" && mymask && acount >= 1)
    {
        wxString chan = evt.Args[0].Lower();
        if (m_chanlist.Index(chan) >= 0)
            m_chanlist.Remove(chan);
    }
    else if (cmd[0] == '4' && cmd[1] == '3' && !m_connected) //nick error, 43x
    {
        SetNextNick();
    }
    else if (cmd == RPL_WHOISUSER && acount >= 4)
    {
        if (evt.Args[1] == m_myhostmask.Nick)
        {
            m_myhostmask.Parse(evt.Args[1], evt.Args[2], evt.Args[3]);
            
            IRCMask host2 = m_myhostmask.MakeString();
        }
    }

    if (cmd == "error" || cmd[0] == '4' || cmd[0] == '5')
    {
        evt.IsError = true;
    }

    wxPostEvent(m_target, evt);
}

void IRC::OnFlush(wxTimerEvent &evt)
{
    if (m_output_buf.IsEmpty())
    {
        //printf("output_buf is empty not flushing\n");
        return;
    }

    wxString first = m_output_buf[0];
    m_sock.Write(first, first.Len());
    m_output_buf.RemoveAt(0);
}

void IRC::OnReset(wxTimerEvent &evt)
{
    //printf("Reseting buf. buf now -> %d\n", m_buf_current);
    //printf("Reseting cmd. cmd now -> %d\n", m_cmd_current);
    m_buf_current = 0;
    m_cmd_current = 0;
}

wxString IRC::GetSockErrText(int id) 
{
    wxString err;

    switch (id)
    {
    case wxSOCKET_NOERROR: 	
        err = "No error happened.";
        break;
    case wxSOCKET_INVOP: 	
        err= "Invalid operation.";
        break;
    case wxSOCKET_IOERR: 	
        err = "Input/Output error.";
        break;
    case wxSOCKET_INVADDR: 	
        err = "Invalid address passed to wxSocket.";
        break;
    case wxSOCKET_INVSOCK: 	
        err = "Invalid socket (uninitialized).";
        break;
    case wxSOCKET_NOHOST: 	
        err = "No corresponding host.";
        break;
    case wxSOCKET_INVPORT: 	
        err = "Invalid port.";
        break;
    case wxSOCKET_WOULDBLOCK: 	
        err = "The socket is non-blocking and the operation would block.";
        break;
    case wxSOCKET_TIMEDOUT: 	
        err = "The timeout for this operation expired.";
        break;
    case wxSOCKET_MEMERR: 	
        err = "Memory exhausted.";
        break;
    default:
        err = "Unknown Error";
    }

    return err;
}

//send an error to the user
void IRC::SendErr(const wxString &msg, bool dead_link)
{
    IRCEvent evt(m_target_id);
    wxString errtxt = msg; 

    if (dead_link)
    {
        if (m_sock.Error())
            errtxt.Printf("%s (%s)", msg.c_str(), GetSockErrText(m_sock.LastError()).c_str());
        Close();
    }

    evt.IsError = true;
    evt.Command = "ERROR";

    evt.Args.Add(errtxt);
    wxPostEvent(m_target, evt);
}

wxArrayString IRC::ReadMsg(int wait)
{
    char buf[512];
    wxString line;
    wxArrayString lines;
    int i, count;

    if (wait)
    {
        if (!m_sock.WaitForRead(wait))
            return lines;
    }
    
    m_sock.Read(buf, sizeof(buf));  
    if (m_sock.LastCount() == 0 || m_sock.Error())
        return lines;

    wxString bufstr(buf, m_sock.LastCount());
    wxStringTokenizer tok(bufstr, "\r\n");


    if (!m_tmp_buf.IsEmpty())
    {
        line += m_tmp_buf;
        m_tmp_buf.Empty();
    }

    while (tok.HasMoreTokens()) 
    {
        line += tok.GetNextToken();
        lines.Add(line);
        line.Empty();
    }

    count = lines.GetCount();
    //incomplete message
    if (bufstr.Last() != '\n' && bufstr.Last() != '\r')
    {
        m_tmp_buf = lines.Last();
        lines.RemoveAt(count-1);
    }

    return lines;
}

/*Handle output from the server, considering multiple messages*/
void IRC::OnSockEvent(wxSocketEvent &evt)
{
    if (evt.GetSocketEvent() == wxSOCKET_LOST)
    {
        SendErr();
        if (!m_rejoin)
            m_chanlist.Empty();
        if (m_autorecon && !m_quitsent)
            Connect();
        m_quitsent = false;

        return;
    }

    wxArrayString lines = ReadMsg();
    int i;
    int count = lines.GetCount();
    for (int i=0; i<count; i++)
        SendMsg(lines[i]);
}

//handle an EVT_IRC command from user and send to IRC server
//Only handles one command per message event
void IRC::OnUserMsg(IRCEvent &evt)
{
    wxString cmd = evt.Command.Lower();
    int acount = evt.Args.GetCount();

    //pass through command, putting the ':' marker where appropriate
    wxString send = evt.Command.Upper();
    
    for (int i = 0; i < acount; i++)
    {
        wxString arg = evt.Args[i];

        if (arg.find_first_of(" \t") != wxNOT_FOUND)
        {
            arg = evt.GetString(i);

            send << " " << ":" << arg;
            break;
        }
        else
            send << " " << arg;
    }

    Quote(send);
}
