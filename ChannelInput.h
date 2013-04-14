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
// Name:        ChannelInput.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __ChannelInput_H__
#define __ChannelInput_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ChannelInput.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "fragmon_wdr.h"
#include "ircbase.h"
#include "ircevent.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// ChannelInput
//----------------------------------------------------------------------------

class ChannelInput: public wxDialog
{
public:
    // constructors and destructors
    ChannelInput( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );
    ~ChannelInput();

    // WDR: method declarations for ChannelInput
    wxListBox* GetCiList()  { return (wxListBox*) FindWindow( ID_CI_LIST ); }
    wxTextCtrl* GetCiIn()  { return (wxTextCtrl*) FindWindow( ID_CI_IN ); }
    
private:
    // WDR: member variable declarations for ChannelInput
    wxString m_channel;
    IRCBase *m_irc;

private:
    // WDR: handler declarations for ChannelInput
    void OnPart( wxCommandEvent &event );
    void OnJoin( wxCommandEvent &event );
    void OnIrcMsg(IRCEvent &evt);

private:
    DECLARE_EVENT_TABLE()
};




#endif
