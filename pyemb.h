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



#include "pygamequery.h"
#ifndef _PYEMB_H_
#define _PYEMB_H_
#include "wx/wx.h"
#include "irc.h"
#include "pydataconv.h"

DECLARE_EVENT_TYPE(wxEVT_PYDEBUG, -1)
DECLARE_EVENT_TYPE(wxEVT_GAMEQUERY, -1)
#define EVT_PYDEBUG(id, fn) EVT_COMMAND(id, wxEVT_PYDEBUG, fn)
#define EVT_GAMEQUERY(id, fn) \
        DECLARE_EVENT_TABLE_ENTRY(wxEVT_GAMEQUERY, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(PyEventFunction) &fn, NULL),

class GameType;
class BotType;
class PyEvent;

WX_DECLARE_OBJARRAY(GameType, GameList);
WX_DECLARE_OBJARRAY(BotType, BotList);

typedef void (wxEvtHandler::*PyEventFunction)(PyEvent&);

//funcs
void PyGetBotList(BotList &bl);
void PyGetGameList(GameList &gl);
void PyPrintDebug(bool err, const char *format, ...);
void PyPrintDebugNoFmt(bool err, const char *buf);
void PySetSocketTimeout(float timeout);
float PyGetSocketTimeout();
wxColour PyMakeColor(unsigned long value);
unsigned long PyGetColor(const wxColour &color);

//
bool PyEmbIsHeadless();
GameType *PyEmbFindQuery(int id);
GameType *PyEmbFindQuery(const wxString &abbrev);
BotType *PyEmbFindBot(const wxString &name);
void PyEmbSetDebugTarget(wxEvtHandler *eh);
wxEvtHandler *PyEmbGetDebugTarget();
void PyEmbSetIrcTarget(IRCBase *irc);
IRCBase *PyEmbGetIrcTarget();
void PyEmbCallIrcHooks(IRCEvent &evt);
void PyEmbSetModulePath(const wxString &path);
wxString PyEmbGetModulePath();
wxString PyEmbGetExcStr();
int PyEmbInit();
void PyEmbFinalize();

#ifdef Py_DEBUG
int PyEmbGetTotalRefcount();
#endif

class GameType
{
public:
    GameType(int i, const wxString &n, const wxString &a, int p, 
            PyObject *ocb = NULL, PyObject *acb = NULL,
            const wxString &xo = "", const wxString &xc = "", 
            const wxString &xp = "", const wxString &xn = "")
        :id(i), name(n), abbrev(a), defport(p), 
        over_cb(ocb), all_cb(acb),
        exec_order(xo), exec_connect(xc), 
        exec_password(xp), exec_name(xn) {}
    
    wxString name;
    wxString abbrev;
    wxString exec_order;
    wxString exec_connect;
    wxString exec_password;
    wxString exec_name;
    int defport;
    int id;

    PyObject *all_cb;
    PyObject *over_cb;
};

class BotType
{
public:
    BotType(const wxString &n = "", PyObject *k = NULL):
        name(n), klass(k) {}

    wxString name;
    RuleList options;
    PyObject *klass;
};

//Only delete when cloaned (from posting event)
class PyEvent : public wxEvent
{
public:
    PyEvent(int winid = -1):wxEvent(winid, wxEVT_GAMEQUERY),m_gdata(NULL),m_cloaned(false),
                            m_error(false)  { ResumePropagation(wxEVENT_PROPAGATE_MAX); }

    PyEvent(const PyEvent &evt): wxEvent(evt), m_gdata(evt.m_gdata), m_cloaned(true), 
                                 m_error(evt.m_error), m_errormsg(evt.m_errormsg) {} 
    ~PyEvent() 
    { 
        if (m_cloaned && m_gdata)
        {
            delete m_gdata;
            PyPrintDebug(false, "Deleting g_data\n");
        }
        else
            PyPrintDebug(false, "NOT Deleting g_data\n");
            
    }

    wxEvent *Clone() const { return new PyEvent(*this); }
    bool IsCloaned() const { return m_cloaned; }
    void SetGdata(GameData *gdata) { m_gdata = gdata; }
    GameData *GetGdata() { return m_gdata; }
    GameData *RetreiveGdata() 
    { 
        assert(m_cloaned == true); 
        GameData *tmp = m_gdata;
        m_gdata = NULL;
        return tmp;
    }
    void SetError(bool err) { m_error = err; }
    bool GotError() const { return m_error; }
    void SetErrorMsg(const wxString &msg) { m_errormsg = msg; }
    wxString GetErrorMsg() const { return m_errormsg; }

    //void SetThread(GameQueryThread *thread) { m_thread = thread; }
    //GameQueryThread *GetThread() { return m_thread; } // the thread that posted this msg

private:
    //GameQueryThread *m_thread;
    bool m_cloaned;
    bool m_error;
    wxString m_errormsg;
    GameData *m_gdata;
};

#endif

