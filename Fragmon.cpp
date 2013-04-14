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
#include "pyemb.h"
#include "DebugFrame.h"
#include "MainFrame.h"
#include "wx/config.h"
#include "fragmon_wdr.h"
#include "Fragmon.h"
#include "ipcserv.h"
#include <wx/snglinst.h>
#include <string.h>
#include "findapp.h"
#include "QueryOpts.h"

IMPLEMENT_APP(Fragmon)  
#define MODULE_DIR "gamequery"
#define HELP_FILE "fragmon.htb"

bool Fragmon::OnInit()
{
    char *pinger = "";

    if (argc > 1)
        pinger = argv[1];
    
    m_checker = new wxSingleInstanceChecker(_T("fragmon-") + wxGetUserId());
    if (m_checker->IsAnotherRunning() && argc > 1)
    {
        wxClient cl;
        wxConnectionBase *conn = cl.MakeConnection("", ServServer::GetService(), "PING");
        if (!conn)
            wxLogError("Can't send ping to fragmon");

        conn->Execute(pinger, strlen(pinger));

        return false;
    }
    
    //Get the config object  
    m_config = wxConfig::Get(); 

    //make arrows list
    wxInitAllImageHandlers();

    wxBitmap teama = TeamsFunc(ID_TEAM_A);
    wxBitmap teamb = TeamsFunc(ID_TEAM_B);
    wxBitmap teamf = TeamsFunc(ID_TEAM_F);
    wxBitmap up = SortArrowsFunc(ID_MINIDOWN); 
    wxBitmap down = SortArrowsFunc(ID_MINIUP); 

    m_arrows.Create(teama.GetWidth(), teamb.GetHeight());
    m_arrows.Add(up);
    m_arrows.Add(down);
    m_arrows.Add(teama);
    m_arrows.Add(teamb);
    m_arrows.Add(teamf);

    m_debugfrm = new DebugFrame(NULL, -1, "Debug Output");
    SetTopWindow(m_debugfrm);

    //Set up python stuff after debug frame so any errors are registered there
    wxString moddir = FindAppData(argv[0], MODULE_DIR);
    if (moddir.IsEmpty())
    {
        PyPrintDebug(true, "Can't find query modules!\n");
        wxMessageBox("Can't find query modules!", "Error");
        m_debugfrm->SetCritical(true);
        m_debugfrm->Show();
        return true; 
    }
    //Init python
    PyPrintDebug(false, "found module path -> %s\n", moddir.c_str());
    PyEmbSetModulePath(moddir);
    if (PyEmbInit() < 0)
    {
        m_debugfrm->SetCritical(true);
        m_debugfrm->Show();
        return true;
    }

    QueryOpts::LoadSocketTimeout();

    //Find help file
    m_helpfile = FindAppData(argv[0], HELP_FILE);
    if (m_helpfile.empty())
        m_helpfile = HELP_FILE;

    PyPrintDebug(false, "\nUsing helpfile %s\n", m_helpfile.c_str());

    m_frame = new MainFrame(NULL, -1, "Fragmon", pinger);
    m_frame->Show();

    
    return true;
}

int Fragmon::OnExit()
{
    QueryOpts::SaveSocketTimeout();
    PyEmbFinalize();

    delete m_config; //flush the config
    delete m_checker;
    return 0;
}
