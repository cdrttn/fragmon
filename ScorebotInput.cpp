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
// Name:        ScorebotInput.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "ScorebotInput"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Fragmon.h"
#include "ScorebotInput.h"
#include "QueryInput.h"
#include "ChannelInput.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// ScorebotInput
//----------------------------------------------------------------------------

// WDR: event table for ScorebotInput

BEGIN_EVENT_TABLE(ScorebotInput,wxDialog)
    //EVT_BUTTON( ID_SBS_SERV, ScorebotInput::OnServ )
    EVT_BUTTON( ID_SBS_CHAN, ScorebotInput::OnChan )
    EVT_BUTTON(wxID_OK, ScorebotInput::OnOk)
    EVT_CHECKBOX( ID_SBS_CUST_CHK, ScorebotInput::OnCustCheck )
    EVT_CHOICE( ID_SBS_BNAME_IN, ScorebotInput::OnSelName )
    EVT_LISTBOX_DCLICK( ID_SBS_OPTS_LST, ScorebotInput::OnOptClick )
END_EVENT_TABLE()

ScorebotInput::ScorebotInput( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style ), 
    m_bot(NULL), m_pybot(NULL), m_port(-1)
{
    // WDR: dialog function ScorebotInputFunc for ScorebotInput
    ScorebotInputFunc( this, TRUE ); 
}

int ScorebotInput::Load()
{
    LoadChannels();
    m_bot = wxGetApp().GetMainFrame()->GetScorebot();
    
    if (m_bot)
    {
        m_host = m_bot->GetHost();
        m_port = m_bot->GetPort();
        m_abbrev = m_bot->GetGame();
        GetSbsTitleIn()->SetValue(m_bot->GetTitle());
        GetSbsInterIn()->SetValue(m_bot->GetInterval());
        GetSbsChanIn()->SetStringSelection(m_bot->GetChannel());

        LoadServer();
    }

    if (m_bot && m_bot->IsRunning())
        GetSbsStatusOut()->SetLabel("Scorebot Running");
    else
    {
        GetSbsStatusOut()->SetLabel("Scorebot Stopped");

        //If scorebot stopped, load in the current server from the main frame.
        wxString host, abbrev;
        int port;
        QueryInput *qi = wxGetApp().GetMainFrame()->GetQueryInput();
        if (qi->GetCurrentServer(host, port, abbrev) && abbrev != "Unk")
        {
            m_host = host;
            m_port = port;
            m_abbrev = abbrev;
            LoadServer();
        }
        else
        {
            wxMessageBox("No or unknown server selected");
            return wxID_CANCEL;
        }

        GetSbsCustChk()->Enable(true);
    }

    //load custom list
    PyGetBotList(m_botlist);
    for (int i = 0; i < m_botlist.GetCount(); i++)
        GetSbsBnameIn()->Append(m_botlist[i].name, (void *) &m_botlist[i]);

    m_pybot = dynamic_cast<PyScoreBot *>(m_bot);
    if (m_pybot)
    {
        BotType *bt = NULL;
        for (int i = 0; i < m_botlist.GetCount(); i++)
        {
            if (m_pybot->Check(&m_botlist[i]))
                bt = &m_botlist[i];
        }

        //bot should be in the list
        wxASSERT(bt);
        
        GetSbsCustChk()->SetValue(true);
        GetSbsBnameIn()->SetStringSelection(bt->name);
        if (GetSbsCustChk()->IsEnabled())
            GetSbsBnameIn()->Enable(true);

        LoadOptions(bt);
    }

    return ShowModal();
}

void ScorebotInput::LoadChannels()
{
    IRCBase *irc = wxGetApp().GetMainFrame()->GetIrc();
    wxASSERT(irc);

    wxArrayString chans = irc->GetChanList();
    wxChoice *clist = GetSbsChanIn();
    clist->Clear();
    for (int i = 0; i < chans.GetCount(); i++)
        clist->Append(chans[i]);
}

void ScorebotInput::LoadServer()
{
    GetSbsServIn()->SetValue(wxString::Format("%s / %s:%d", m_abbrev.c_str(), m_host.c_str(), m_port));
}

// WDR: handler implementations for ScorebotInput

void ScorebotInput::OnOptClick( wxCommandEvent &event )
{
    GameRule *gr = (GameRule *) event.GetClientData();
    if (gr)
    {
        wxTextEntryDialog te(this, "Change value for " + gr->key);
        te.SetValue(gr->value);

        if (te.ShowModal() != wxID_OK)
            return;

        gr->value = te.GetValue();

        int sel = event.GetSelection();
        GetSbsOptsLst()->SetString(sel, gr->key + " = " + gr->value);
    }
}

void ScorebotInput::LoadOptions(BotType *bt)
{
    if (bt)
    {
        RuleList &ol = bt->options;

        wxListBox *ls = GetSbsOptsLst();
        ls->Clear();
        
        if (m_pybot && m_pybot->Check(bt))
        {
            puts("LOADING FROM Current INSTANCE");
            for (int i = 0; i < ol.GetCount(); i++)
                ol[i].value = m_pybot->GetOption(ol[i].key);
        }

        for (int i = 0; i < ol.GetCount(); i++)
            ls->Append(ol[i].key + " = " + ol[i].value, (void *) &ol[i]);
    }
}

void ScorebotInput::OnSelName( wxCommandEvent &event )
{
    BotType *bt = (BotType *) event.GetClientData();
    LoadOptions(bt);
}

void ScorebotInput::OnCustCheck( wxCommandEvent &event )
{
    if (event.IsChecked())
        GetSbsBnameIn()->Enable(true);
    else
    {
        GetSbsBnameIn()->Enable(false);
        GetSbsOptsLst()->Clear();
    }
}

void ScorebotInput::OnChan( wxCommandEvent &event )
{
    ChannelInput ci(this, -1, "Channels");
    ci.ShowModal();
    LoadChannels();
}

void ScorebotInput::OnOk(wxCommandEvent &evt)
{
    int inter = GetSbsInterIn()->GetValue();
    wxString chan = GetSbsChanIn()->GetStringSelection();
    wxString title = GetSbsTitleIn()->GetValue();
    title.Trim(true);
    title.Trim(false);

    if (m_host.empty() || m_port < 0 || m_abbrev.empty())
        wxMessageBox("No server entered.");
    else if (chan.empty())
        wxMessageBox("No channel selected.");
    else if (title.empty())
        wxMessageBox("No title entered.");
    else
    {
        if (GetSbsCustChk()->IsChecked()) 
        {
            //Attempt loading custom scorebot
            if (!m_pybot)
            {
                m_pybot = new PyScoreBot;
                SetBot(m_pybot);
            }
            
            BotType *bt = NULL;
            int sel = GetSbsBnameIn()->GetSelection();
            if (sel < 0 || (bt = (BotType*)GetSbsBnameIn()->GetClientData(sel)) == NULL)
            {
                wxMessageBox("No custom scorebot selected");
                return;
            }
       
            if (!m_pybot->Check(bt))
            {
                if (!m_pybot->Init(bt))
                {
                    wxMessageBox("Failed to initialize custom scorebot");
                    return;
                }
            }

            //Set options
            wxListBox *lst = GetSbsOptsLst();
            for (int i = 0; i < lst->GetCount(); i++)
            {
                GameRule *gr = (GameRule *)lst->GetClientData(i);
                if (gr)
                {
                    PyPrintDebug(false, "Setting PyScorebot Option, %s = %s\n", 
                            gr->key.c_str(), gr->value.c_str());
                    
                    wxString err = m_pybot->SetOption(gr->key, gr->value);
                    if (!err.empty())
                    {
                        wxString msg = "Invalid option, " + gr->key + " = " + gr->value + ":\n" + err;
                        wxMessageBox(msg);
                        return;
                    }
                }
            }
            
            wxASSERT(m_bot == m_pybot);
        }
        else if (!m_bot || m_pybot)
            SetBot(new ScoreBot());  //load basic scorebot
            
        m_bot->SetInterval(inter);
        m_bot->SetTitle(title);
        m_bot->SetChannel(chan);

        m_bot->SetHost(m_host);
        m_bot->SetPort(m_port);
        m_bot->SetGame(m_abbrev);
        
        EndModal(wxID_OK);
    }
}



