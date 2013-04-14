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
// Name:        ScorebotInput.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __ScorebotInput_H__
#define __ScorebotInput_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ScorebotInput"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "fragmon_wdr.h"
#include "scorebot.h"
#include "pyscorebot.h"
#include "Fragmon.h"
#include "ircbase.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// ScorebotInput
//----------------------------------------------------------------------------

class ScorebotInput: public wxDialog
{
public:
    // constructors and destructors
    ScorebotInput( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );

    int Load();
    
    // WDR: method declarations for ScorebotInput
    wxListBox* GetSbsOptsLst()  { return (wxListBox*) FindWindow( ID_SBS_OPTS_LST ); }
    wxChoice* GetSbsBnameIn()  { return (wxChoice*) FindWindow( ID_SBS_BNAME_IN ); }
    wxCheckBox* GetSbsCustChk()  { return (wxCheckBox*) FindWindow( ID_SBS_CUST_CHK ); }
    wxTextCtrl* GetSbsServIn()  { return (wxTextCtrl*) FindWindow( ID_SBS_SERV_IN ); }
    wxChoice* GetSbsChanIn()  { return (wxChoice*) FindWindow( ID_SBS_CHAN_IN ); }
    wxStaticText* GetSbsStatusOut()  { return (wxStaticText*) FindWindow( ID_SBS_STATUS_OUT ); }
    wxSpinCtrl* GetSbsInterIn()  { return (wxSpinCtrl*) FindWindow( ID_SBS_INTER_IN ); }
    wxTextCtrl* GetSbsTitleIn()  { return (wxTextCtrl*) FindWindow( ID_SBS_TITLE_IN ); }
    ScoreBot *GetScorebot() { return m_bot; }

    //virtual bool TransferDataToWindow();
    
private:
    // WDR: member variable declarations for ScorebotInput
    void LoadOptions(BotType *bt);
    void SetBot(ScoreBot *bot)
    {
        if (m_bot && m_bot != bot)
            delete m_bot;
        m_bot = bot;
    }

    ScoreBot *m_bot;
    PyScoreBot *m_pybot;
    BotList m_botlist;

    wxString m_host;
    wxString m_abbrev;
    int m_port;

private:
    void LoadChannels();
    void LoadServer();

private:
    // WDR: handler declarations for ScorebotInput
    void OnOptClick( wxCommandEvent &event );
    void OnSelName( wxCommandEvent &event );
    void OnCustCheck( wxCommandEvent &event );
    void OnChan( wxCommandEvent &event );
    void OnOk(wxCommandEvent &evt);

private:
    DECLARE_EVENT_TABLE()
};




#endif
