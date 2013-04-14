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
// Name:        ChannelInput.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "ChannelInput.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "ChannelInput.h"
#include "Fragmon.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// ChannelInput
//----------------------------------------------------------------------------

// WDR: event table for ChannelInput

BEGIN_EVENT_TABLE(ChannelInput,wxDialog)
    EVT_BUTTON( ID_CI_JOIN, ChannelInput::OnJoin )
    EVT_BUTTON( ID_CI_PART, ChannelInput::OnPart )
    EVT_IRC(-1, ChannelInput::OnIrcMsg)
END_EVENT_TABLE()

ChannelInput::ChannelInput( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style ), m_irc(NULL)
{
    // WDR: dialog function ChannelInputFunc for ChannelInput
    ChannelInputFunc( this, TRUE ); 

    wxArrayString chanex;
    chanex.Add(',');
    chanex.Add('\t');
    chanex.Add('\r');
    chanex.Add('\n');
    chanex.Add('\a');

    wxTextValidator chanval(wxFILTER_EXCLUDE_CHAR_LIST, &m_channel);
    chanval.SetExcludes(chanex);
    GetCiIn()->SetValidator(chanval);

    m_irc = wxGetApp().GetMainFrame()->GetIrc();
    wxASSERT(m_irc);
    
    m_irc->SetTempHandler(this, -1);

    //add current channels
    wxArrayString chans = m_irc->GetChanList();
    wxListBox *cl = GetCiList();
    for (int i = 0; i < chans.GetCount(); i++)
        cl->Append(chans[i]);
}

ChannelInput::~ChannelInput()
{
    m_irc->PopTempHandler();
}

// WDR: handler implementations for ChannelInput

void ChannelInput::OnPart( wxCommandEvent &event )
{
    wxASSERT(m_irc);

    int csel = GetCiList()->GetSelection();
    if (csel < 0)
    {
        wxMessageBox("No channel selected.");
        return;
    }
   
    wxString chan = GetCiList()->GetString(csel);
    GetCiList()->Delete(csel);
    
    IRCEvent ie;
    ie << "part" << chan;
    wxPostEvent(m_irc, ie);
}

void ChannelInput::OnJoin( wxCommandEvent &event )
{
    wxASSERT(m_irc);

    if (!Validate() || !TransferDataFromWindow() 
            || m_channel.empty() || m_channel.Freq(' ') > 1)
    {
        wxMessageBox("Invalid channel.");
        return;
    }
    m_channel.Trim(true);
    m_channel.Trim(false);

    wxString chan, pass;
    int pos = m_channel.find(' ');
    if (pos >= 0)
    {
        chan = m_channel.substr(0, pos);
        pass = m_channel.substr(pos+1);
    }
    else 
        chan = m_channel;

    m_channel.clear();

    
    IRCEvent ie;
    ie << "join" << chan;
    if (!pass.empty())
        ie << pass;

    wxPostEvent(m_irc, ie);
}

void ChannelInput::OnIrcMsg(IRCEvent &evt)
{
    if (evt.Command == RPL_JOINOK)
        GetCiList()->Append(evt.Args[0]);

    //make sure MainFrame's handler gets a copy
    wxGetApp().GetMainFrame()->ProcessEvent(evt);
}



