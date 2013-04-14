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



#ifndef _PYGAMEQUERY_H_
#define _PYGAMEQUERY_H_
#include "wx/wx.h"
#include "pyemb.h"

WX_DEFINE_ARRAY(long, ThreadIdList);

class GameQueryThread : public wxThread
{
public:
    GameQueryThread(wxEvtHandler *target, int winid, int game_id, 
            const wxString &host, int port, int query_type = GameQueryThread::OVERVIEW);
    GameQueryThread(wxEvtHandler *target, int winid, const wxString &abbrev, 
            const wxString &host, int port, int query_type = GameQueryThread::OVERVIEW);
    
    virtual void *Entry();
    enum
    {
        OVERVIEW,
        ALL
    };

protected:
    virtual void OnExit();
    
protected:
    PyGILState_STATE m_gilstate;
    GameType *m_query;
    GameData *m_gdata;
    bool m_error;
    wxEvtHandler *m_target;
    int m_winid;
    //int m_game_id;
    wxString m_host;
    int m_port;
    int m_query_type;
    wxString m_errmsg;

    //XXX: could put this in a mix-in class
protected:
    static ThreadIdList thread_list;
    static int thread_count;
    static wxMutex all_mutex;
    static wxCondition all_done;
    static void AddThread();
    static void DelThread();
    static void SetException(long id);

public:
    static void WaitForThreads();
    static void InitCount();
    static void FiniCount();
};

#endif
