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



#ifndef FRAGMON_H
#define FRAGMON_H
#include "wx/wx.h"
#include "MainFrame.h"
#include "DebugFrame.h"
#include "wx/config.h"
#include <wx/snglinst.h>
#include "pyemb.h"

#define FM_APPNAME "Fragmon"
#define FM_VERSION "0.2"
#define FM_WEBSITE "http://fragmon.ath.cx"

class Fragmon: public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
    wxConfigBase *GetConf() { return m_config; }
    wxImageList *GetArrows() { return &m_arrows; }
    //wxImageList *GetTeams() { return &m_teams; }
    MainFrame *GetMainFrame() { return m_frame; }
    DebugFrame *GetDebugFrame() { return m_debugfrm; }
    wxString GetHelpFile() { return m_helpfile; }

private:
    wxSingleInstanceChecker *m_checker;
    wxConfigBase *m_config;
    wxImageList m_arrows;
    //wxImageList m_teams;
    MainFrame *m_frame;
    DebugFrame *m_debugfrm;
    wxString m_helpfile;
};

DECLARE_APP(Fragmon)

#endif
