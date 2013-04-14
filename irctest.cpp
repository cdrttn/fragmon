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
#include "irc.h"
#include "wx/tokenzr.h"

class OutputFrame : public wxFrame
{
public:
    OutputFrame(const wxString &title);
    void OnIRCMsgEvent(IRCEvent &evt); 
    void OutputFrame::OnYa(wxCommandEvent &evt);
    void OnEnter(wxCommandEvent &evt);
    void OutputFrame::OnReConnect(wxCommandEvent &evt);
    IRC *GetIRC() const { return m_irc; }

protected:
    void PostMsg(const wxString &msg, bool err = true);

protected:
    IRC *m_irc;
    wxTextCtrl *m_output;
    wxTextCtrl *m_prompt;
    wxTextCtrl *m_bthresh;
    wxTextCtrl *m_cthresh;
    wxTextCtrl *m_fint;
    wxTextCtrl *m_btime;

    wxString m_bthresh_s;
    wxString m_cthresh_s;
    wxString m_fint_s;
    wxString m_btime_s;
    wxString m_chan;

private:
    DECLARE_EVENT_TABLE()
};

class TestIrc : public wxApp
{
public:
    virtual bool OnInit();
};

BEGIN_EVENT_TABLE(OutputFrame, wxFrame)
    EVT_IRC(-1, OutputFrame::OnIRCMsgEvent)
    EVT_TEXT_ENTER(44, OutputFrame::OnEnter)
    EVT_TEXT_ENTER(77, OutputFrame::OnYa)
    //EVT_CUSTOM(wxEVT_IRC_MSG_ACTION, -1, OutputFrame::OnIRCMsgEvent)
    //EVT_BUTTON(-1, OutputFrame::OnIRCMsgEvent)
    EVT_BUTTON(99, OutputFrame::OnYa)
    EVT_BUTTON(100, OutputFrame::OnReConnect)
END_EVENT_TABLE()

IMPLEMENT_APP(TestIrc)

OutputFrame::OutputFrame(const wxString &title)
    :wxFrame(NULL, -1, title)
{
    m_irc = new IRC(this, -1);
    m_bthresh_s.Printf("%d", m_irc->GetBufThreshold()); 
    m_cthresh_s.Printf("%d", m_irc->GetCmdThreshold());
    m_fint_s.Printf("%d", m_irc->GetFlushInterval());
    m_btime_s.Printf("%d", m_irc->GetThresholdTime());

    m_output = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);  
    m_prompt = new wxTextCtrl(this, 44, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    m_bthresh = new wxTextCtrl(this, 77, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC, &m_bthresh_s));
    m_cthresh = new wxTextCtrl(this, 77, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC, &m_cthresh_s));
    m_fint = new wxTextCtrl(this, 77, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC, &m_fint_s));
    m_btime = new wxTextCtrl(this, 77, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC, &m_btime_s));
    
    wxBoxSizer *box = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);
    wxGridSizer *opts = new wxGridSizer(4, 2, 9);
    
    box->Add(m_output, 1, wxEXPAND);
    hbox->Add(m_prompt, 1, wxEXPAND);
    box->Add(hbox, 0, wxEXPAND);

    opts->Add(new wxStaticText(this, -1, "BufThreshold"));
    opts->Add(new wxStaticText(this, -1, "CmdThreshold"));
    opts->Add(new wxStaticText(this, -1, "FlushInterval"));
    opts->Add(new wxStaticText(this, -1, "ThresholdTime"));
    opts->Add(m_bthresh);
    opts->Add(m_cthresh);
    opts->Add(m_fint);
    opts->Add(m_btime);

    hbox2->Add(opts, 1, wxEXPAND);
    hbox2->Add(new wxButton(this, 99, "&Change"), 0, wxALIGN_CENTER|wxALL, 5);
    hbox2->Add(new wxButton(this, 100, "&ReConnect"), 0, wxALIGN_CENTER|wxALL, 5);
    box->Add(hbox2, 0, wxALIGN_CENTER|wxALL, 7);


    SetSizerAndFit(box);
    SetSize(800,600);
}

void OutputFrame::OnEnter(wxCommandEvent &evt)
{
    wxString buf = m_prompt->GetValue();
    printf("Entered: %s\n", buf.c_str());
    m_prompt->Clear();

    buf.Trim();
    buf.Trim(false);
 
    IRCEvent e;
    
    if (buf.IsEmpty())
        return;
    if (buf[0] == '/')
    {
        buf = buf.substr(1); 
        wxStringTokenizer tok(buf, _T(" \t"));

        int tcount = tok.CountTokens();

        e << tok.GetNextToken();
        printf("msg -> %s(%s), tcount -> %d\n", e.Command.c_str(), tok.GetString().c_str(), tcount);
        
        if (e.Command.Lower() == "msg" && tcount >= 2)
        {
            wxString target = tok.GetNextToken();
            wxString msg = tok.GetString();
            printf("%s -> %s\n", target.c_str(), msg.c_str());
            e << target << msg;
        }
        else if (e.Command.Lower() == "chan")
        {
            if (tcount >= 2)
                m_chan = tok.GetNextToken();
            else
            {
                puts ("HEREREERE!");
                PostMsg(wxString::Format("Current Channel -> \"%s\"", m_chan.c_str()));
            }

            //dont post this
            return;
        }
        else
        {
            while (tok.HasMoreTokens())
                e << tok.GetNextToken();
        }

    } 
    else
    {
        if (!m_chan.empty())
            e << "privmsg" << m_chan << buf;
        else
        {
            PostMsg("No channel set");
            return;
        }
    }

    wxPostEvent(m_irc, e);
}

void OutputFrame::OnReConnect(wxCommandEvent &evt)
{
    m_irc->Connect();
}

void OutputFrame::PostMsg(const wxString &msg, bool err)
{
    IRCEvent e;
    e.Source = "Fragmon";
    e.IsError = err;

    if (err)
        e.Command = "ERROR";
    else
        e.Command = "INFO";

    e << msg;

    wxPostEvent(this, e);
}

void OutputFrame::OnIRCMsgEvent(IRCEvent &evt)
{
    if (evt.IsError)
    {
        m_output->SetDefaultStyle(wxTextAttr(*wxRED, *wxWHITE));
        //m_output->AppendText(wxString::Format(" ERROR: %s\n", evt.GetErrText().c_str()));
    }
    m_output->AppendText("SrcPrefix(" + evt.Source + ") Cmd(" + evt.Command + ")");

    m_output->AppendText(wxString::Format(" Args=%d(", evt.Args.GetCount()));
    for (int i=0; i < evt.Args.GetCount(); i++)
        m_output->AppendText(" ." + evt.Args[i]);
   
    m_output->AppendText(" )\n");

    if (evt.IsError)
        m_output->SetDefaultStyle(wxTextAttr());

}

void OutputFrame::OnYa(wxCommandEvent &evt)
{

    /*wxString m_bthresh_s;
    wxString m_cthresh_s;
    wxString m_fint_s;
    wxString m_btime_s;*/
    if (Validate() && TransferDataFromWindow() && 
            !m_bthresh_s.IsEmpty() && !m_cthresh_s.IsEmpty() && 
            !m_fint_s.IsEmpty() && !m_btime_s.IsEmpty())
    {
        long set;
        printf("Validated... btresh %s, cthresh %s, fint %s, btime %s\n", 
                m_bthresh_s.c_str(), m_cthresh_s.c_str(), m_fint_s.c_str(), m_btime_s.c_str());
        m_bthresh_s.ToLong(&set);
        m_irc->SetBufThreshold(set);

        m_cthresh_s.ToLong(&set);
        m_irc->SetCmdThreshold(set); 

        m_fint_s.ToLong(&set);
        m_irc->SetFlushInterval(set);

        m_btime_s.ToLong(&set);
        m_irc->SetThresholdTime(set);
    }
    else
        printf("NOOO\n");
}

bool TestIrc::OnInit()
{
    char *hostname = "", *port = "6667", *password = "", *nick = "loafnut";
    if (argc < 2)
    {
        puts("irctest hostname [port] [password] [nick1 nick2 ...]");
        return false;
    }

    if (argc >= 2)
        hostname = argv[1];
    if (argc >= 3)
        port = argv[2];
    if (argc >= 4)
        password = argv[3];
    if (argc >= 5)
        nick = argv[4];

    printf("Login: %s:%s, Nick: %s, Pass: %s\n", hostname, port, nick, password);
    
    OutputFrame *of = new OutputFrame(_T("Sup fool"));
    of->InitDialog();
    of->Show();
    IRCEvent e;
    e << "add_nicks" << "test1" << "test2" << "test3";
    wxPostEvent(of->GetIRC(), e);

    //e.Clear();
    //e << "clear_nicks";
    //wxPostEvent(of->GetIRC(), e);
    
    /*
    of->GetIRC()->AddNick(nick);
    if (argc >= 6)
    {
        for (int i = 5; i < argc; i++)
        {
            of->GetIRC()->AddNick(argv[i]);
            puts(argv[i]);
        }
    }*/

    //of->GetIRC()->Connect(hostname, port, "Juxtapose", "Lingo", password);
    e.Clear();
    e << "connect" << hostname << port << "Juxtapose" << "Lingo" << password;
    wxPostEvent(of->GetIRC(), e);

    return true;
}

