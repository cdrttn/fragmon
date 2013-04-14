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
// Name:        IRCInput.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __IRCInput_H__
#define __IRCInput_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "IRCInput.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "fragmon_wdr.h"     
#include "ServerEntry.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// IRCInput
//----------------------------------------------------------------------------

class IRCInput: public wxDialog
{
public:
    // constructors and destructors
    IRCInput( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );
    
    // WDR: method declarations for IRCInput
    virtual bool Validate();

    ServerEntry* GetIrcServIn()  { return (ServerEntry*) FindWindow( ID_IRC_SERV_IN ); }
    wxTextCtrl* GetIrcNick3In()  { return (wxTextCtrl*) FindWindow( ID_IRC_NICK3_IN ); }
    wxTextCtrl* GetIrcNick2In()  { return (wxTextCtrl*) FindWindow( ID_IRC_NICK2_IN ); }
    wxTextCtrl* GetIrcNick1In()  { return (wxTextCtrl*) FindWindow( ID_IRC_NICK1_IN ); }
    wxTextCtrl* GetIrcRealIn()  { return (wxTextCtrl*) FindWindow( ID_IRC_REAL_IN ); }
    wxTextCtrl* GetIrcUserIn()  { return (wxTextCtrl*) FindWindow( ID_IRC_USER_IN ); }
    wxTextCtrl* GetIrcPassIn()  { return (wxTextCtrl*) FindWindow( ID_IRC_PASS_IN ); }
    wxCheckBox* GetIrcPassChk()  { return (wxCheckBox*) FindWindow( ID_IRC_PASS_CHK ); }
    
    wxString Nick1;
    wxString Nick2;
    wxString Nick3;
    wxString Realname;
    wxString Username;
    bool HasPassword;
    wxString Password;
    wxString Host;
    int Port;

    
private:
    // WDR: member variable declarations for IRCInput
    
private:
    // WDR: handler declarations for IRCInput
    void OnPassChecked( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
