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
// Name:        IRCInput.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "IRCInput.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "IRCInput.h"
#include "ctype.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// IRCInput
//----------------------------------------------------------------------------

// WDR: event table for IRCInput

BEGIN_EVENT_TABLE(IRCInput,wxDialog)
    EVT_CHECKBOX( ID_IRC_PASS_CHK, IRCInput::OnPassChecked )
END_EVENT_TABLE()

IRCInput::IRCInput( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    // WDR: dialog function IRCInputFunc for IRCInput
    IRCInputFunc( this, TRUE ); 

    Username = "Fragmon";
    Realname = "Fragmon Scorebot";
    Nick1 = "Fragmon";
    HasPassword = false;
    Port = 6667;
    
    //Nick input 
    wxArrayString nickchars;
    nickchars.Add('-');
    nickchars.Add('[');
    nickchars.Add(']');
    nickchars.Add('\\');
    nickchars.Add('`');
    nickchars.Add('^');
    nickchars.Add('{');
    nickchars.Add('}');
    for (char c = '0'; c <= '9'; c++)
        nickchars.Add(c);
    for (char c = 'A'; c <= 'Z'; c++)
        nickchars.Add(c);
    for (char c = 'a'; c <= 'z'; c++)
        nickchars.Add(c);
    
    wxTextValidator n1val(wxFILTER_INCLUDE_CHAR_LIST, &Nick1);
    wxTextValidator n2val(wxFILTER_INCLUDE_CHAR_LIST, &Nick2);
    wxTextValidator n3val(wxFILTER_INCLUDE_CHAR_LIST, &Nick3);
    n1val.SetIncludes(nickchars);
    n2val.SetIncludes(nickchars);
    n3val.SetIncludes(nickchars);
    GetIrcNick1In()->SetValidator(n1val);
    GetIrcNick2In()->SetValidator(n2val);
    GetIrcNick3In()->SetValidator(n3val);

    //Username (parameter)
    wxArrayString parmexchars;
    parmexchars.Add('\0');
    parmexchars.Add(' ');
    parmexchars.Add('\t');
    parmexchars.Add('\n');
    parmexchars.Add('\r');

    wxTextValidator userval(wxFILTER_EXCLUDE_CHAR_LIST, &Username);
    userval.SetExcludes(parmexchars);
    GetIrcUserIn()->SetValidator(userval);
    
    //Password,Realname (trailing)
    wxArrayString trailexchars;
    trailexchars.Add('\0');
    trailexchars.Add('\n');
    trailexchars.Add('\r');

    wxTextValidator passval(wxFILTER_EXCLUDE_CHAR_LIST, &Password);
    wxTextValidator realval(wxFILTER_EXCLUDE_CHAR_LIST, &Realname);
    passval.SetExcludes(trailexchars);
    realval.SetExcludes(trailexchars);
    GetIrcPassIn()->SetValidator(passval);
    GetIrcRealIn()->SetValidator(realval);

    //Hostname? use parmexchars
    //wxTextValidator hostval(wxFILTER_EXCLUDE_CHAR_LIST, &Realname);
}

bool IRCInput::Validate()
{
    if (!GetIrcNick1In()->Validate() || 
            GetIrcNick1In()->GetValue().empty() || 
            isdigit(GetIrcNick1In()->GetValue()[0]))
    {
        GetIrcNick1In()->SetFocus();
        wxMessageBox("(Required) Nick 1 is invalid");
        return false;
    }

    if (!GetIrcNick2In()->Validate() || isdigit(GetIrcNick2In()->GetValue()[0]))
    {
        GetIrcNick2In()->SetFocus();
        wxMessageBox("Nick 2 is invalid");
        return false;
    }

    if (!GetIrcNick3In()->Validate() || isdigit(GetIrcNick3In()->GetValue()[0]))
    {
        GetIrcNick3In()->SetFocus();
        wxMessageBox("Nick 3 is invalid");
        return false;
    }

    if (!GetIrcUserIn()->Validate() || GetIrcUserIn()->GetValue().empty())
    {
        GetIrcUserIn()->SetFocus();
        wxMessageBox("(Required) Username is invalid");
        return false;
    }

    if (!GetIrcRealIn()->Validate() || GetIrcRealIn()->GetValue().empty())
    {
        GetIrcRealIn()->SetFocus();
        wxMessageBox("(Required) Realname is invalid");
        return false;
    }

    if (!GetIrcServIn()->ParseInput(Host, Port))
    {
        GetIrcServIn()->SetFocus();
        return false;
    }
    
    return true;
}

// WDR: handler implementations for IRCInput

void IRCInput::OnPassChecked( wxCommandEvent &event )
{
    GetIrcPassIn()->Enable(event.IsChecked());
    HasPassword = event.IsChecked();
}




