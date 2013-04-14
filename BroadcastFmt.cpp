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
// Name:        BroadcastFmt.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "BroadcastFmt.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "BroadcastFmt.h"
#include "varstr.h"
#include "Fragmon.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// BroadcastFmt
//----------------------------------------------------------------------------

// WDR: event table for BroadcastFmt

BEGIN_EVENT_TABLE(BroadcastFmt,wxPanel)
    EVT_BUTTON( wxID_APPLY, BroadcastFmt::OnApply )
    EVT_TEXT( ID_BOT_FORMAT_IN, BroadcastFmt::OnText )
END_EVENT_TABLE()

BroadcastFmt::BroadcastFmt( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    // WDR: dialog function BroadcastFmtFunc for BroadcastFmt
    BroadcastFmtFunc( this, TRUE ); 

    wxString fmt = GetFormat();
    GetBotFormatIn()->SetValue(fmt);
}

// WDR: handler implementations for BroadcastFmt

void BroadcastFmt::OnText( wxCommandEvent &event )
{
    VarMap vmap;

    vmap["server"] = "11.12.13.14";
    vmap["port"] = "4421";
    vmap["ping"] = "50";
    vmap["hostname"] = "We want your pie";
    vmap["curplayers"] = "12";
    vmap["maxplayers"] = "16";
    vmap["map"] = "pro-q3dm6";
    vmap["type"] = "1v1";
    vmap["mod"] = "OSP";
    vmap["version"] = "q3 1.31";
    vmap["password"] = "No";

    vmap["title"] = "Backdoor Mastas Round xyz";
    vmap["time"] = "9:00";
    vmap["score_a"] = "54";
    vmap["name_a"] = "Team Rubbenlove";
    vmap["score_b"] = "40";
    vmap["name_b"] = "Ride The Bare";

    GetBotExOut()->SetValue(VarSubst(GetBotFormatIn()->GetValue(), vmap, true));
}

void BroadcastFmt::OnApply( wxCommandEvent &event )
{    
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/GameOpts");
    config->Write("BotFormat", GetBotFormatIn()->GetValue());

    event.Skip();
}

wxString BroadcastFmt::GetFormat()
{
    wxString fmt = DEF_BOT_FMT;
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/GameOpts");
    config->Read("BotFormat", &fmt);

    return fmt;
}

void BroadcastFmt::LoadFormat(const ScoreBot &bot, VarMap &vmap)
{
    wxString fmt = GetFormat();

    vmap["title"] = bot.GetTitle();
    vmap["time"] = bot.GetTime();
    vmap["score_a"] = bot.GetScoreA();
    vmap["name_a"] = bot.GetNameA();
    vmap["score_b"] = bot.GetScoreB();  
    vmap["name_b"] = bot.GetNameB();
}

