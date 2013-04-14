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



#include "pyemb.h"

//XXX: is this threadsafe?
int GameQueryThread::thread_count = 0;
wxMutex GameQueryThread::all_mutex;
wxCondition GameQueryThread::all_done(GameQueryThread::all_mutex);
ThreadIdList GameQueryThread::thread_list;

GameQueryThread::GameQueryThread(wxEvtHandler *target, int winid, int game_id, 
        const wxString &host, int port, int query_type) 
        :wxThread(wxTHREAD_DETACHED), m_target(target), 
        m_winid(winid),
        m_host(host), m_port(port), 
        m_query_type(query_type), m_error(true)
{
    m_query = PyEmbFindQuery(game_id);

    Create();
    Run();
}

GameQueryThread::GameQueryThread(wxEvtHandler *target, int winid, const wxString &abbrev,
        const wxString &host, int port, int query_type) 
        :wxThread(wxTHREAD_DETACHED), m_target(target), 
        m_winid(winid),
        m_host(host), m_port(port), 
        m_query_type(query_type), m_error(true)
{
    printf("abbr = %s\n", abbrev.c_str());
    m_query = PyEmbFindQuery(abbrev);

    Create();
    Run();
} 


//Query thread runs a python function to get stats from a game server
void *GameQueryThread::Entry()
{
    PyObject *func = NULL, *ret = NULL;

    AddThread();

    m_gilstate = PyGILState_Ensure();
    m_gdata = new GameData();

    if (!m_query)
    {
        PyErr_SetString(PyExc_RuntimeError, "Invalid game");
        goto error;
    }

    PyPrintDebug(false, "(%ld) Starting up: game %s\n", GetId(), m_query->abbrev.c_str());

    
#ifdef Py_DEBUG
    {
        int refcount = PyEmbGetTotalRefcount();
        PyPrintDebug(true, "Python Total RefCount (BEGIN) -> %d\n", refcount);
    }
#endif


    func = m_query_type==OVERVIEW? m_query->over_cb : m_query->all_cb;
    ret = PyObject_CallFunction(func, "(ssi)", m_query->abbrev.c_str(), m_host.c_str(), m_port);  
    if (!ret)
        goto error;
        
    if (m_query_type == OVERVIEW)
    {
        if (!m_gdata->MakeGdata(ret, m_host, m_port)) 
            goto error;
    }
    else //query_type == ALL
    {
        PyObject *overview = NULL, *players = NULL, *rules = NULL; 

        if (!PyArg_ParseTuple(ret, "OOO:all_parse", &overview, &players, &rules))
            goto error;

        if (!m_gdata->MakeGdata(overview, m_host, m_port, players, rules))
            goto error;
    }

    m_error = false;

error:
    if (PyErr_Occurred())
    {
        m_errmsg = PyEmbGetExcStr();
        PyErr_Print();
    }
    Py_XDECREF(ret);

    return NULL;
}

void GameQueryThread::OnExit()
{
    if (m_target && !PyEmbIsHeadless())
    {
        PyEvent evt(m_winid);
        if (m_error) //m_error not set to false above
        {
            delete m_gdata;
            evt.SetError(true);
            evt.SetErrorMsg("Error communicating with game server (" + m_errmsg + ")");
        }
        else
            evt.SetGdata(m_gdata);

        wxPostEvent(m_target, evt);
    }

    /*
    //TEST
    PyObject *gc = PyImport_ImportModule("gc");
    assert(gc);
    PyObject *collected = PyObject_CallMethod(gc, "collect", NULL);
    assert(collected);
    PyPrintDebug(true, "gc.collect() -> %d\n", PyInt_AsLong(collected));
    PyObject *garbage = PyObject_GetAttrString(gc, "garbage");
    assert(garbage);
    PyObject *repr = PyObject_Repr(garbage);
    assert(repr);
    PyPrintDebug(true, "gc.garbage -> %s\n", PyString_AsString(repr));

    Py_DECREF(gc);
    Py_DECREF(collected);
    Py_DECREF(garbage);
    Py_DECREF(repr);
    */

#ifdef Py_DEBUG
    {
        int refcount = PyEmbGetTotalRefcount();
        PyPrintDebug(true, "Python Total RefCount (END THREAD) -> %d\n", refcount);
    }
#endif

    PyPrintDebug(false, "(%ld) Shutting down\n", GetId());
    PyGILState_Release(m_gilstate);

    DelThread(); 
}

//XXX: could put wait methods in a mix-in or static class

void GameQueryThread::AddThread()
{
    all_mutex.Lock();
    thread_count++;
    thread_list.Add(GetCurrentId());
    all_mutex.Unlock();
}

void GameQueryThread::DelThread()
{
    all_mutex.Lock();
    thread_count--;
    thread_list.Remove(GetCurrentId());
    if (thread_count == 0)
    {
        all_done.Broadcast();
    }

    all_mutex.Unlock();
}

void GameQueryThread::WaitForThreads()
{
    all_mutex.Lock();
    //printf("thread_count %d\n", thread_count);
    for (int i = 0; i < thread_list.GetCount(); i++)
    {
        SetException(thread_list[i]);
    }

    if (thread_count)
    {
        all_done.Wait();
    }

    all_mutex.Unlock();
}

void GameQueryThread::SetException(long id)
{
    int count = 0;
    PyGILState_STATE gs = PyGILState_Ensure();

    if ((count = PyThreadState_SetAsyncExc(id, PyExc_KeyboardInterrupt)) > 1)
    {
        //PyPrintDebug(true, "Hmm PyThreadState_SetAsyncExc(%ld) == %d?\n", id, count);
        PyThreadState_SetAsyncExc(id, NULL);
    }
    printf("PyThreadState_SetAsyncExc(%ld) == %d\n", id, count);
    
    PyGILState_Release(gs);
}

void GameQueryThread::InitCount()
{
    thread_count = 0;
    wxASSERT(all_done.IsOk());
}

void GameQueryThread::FiniCount()
{
    //
}

