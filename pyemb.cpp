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
#include "wx/thread.h"
#include "wx/dir.h"
#include "pyemb.h"
#include "pywrappers.h"
#include "pygamequery.h"
#include <Python.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <wx/arrimpl.cpp>
#include "Fragmon.h"

WX_DEFINE_OBJARRAY(GameList);
WX_DEFINE_OBJARRAY(BotList);

static wxEvtHandler *debug_target = NULL;
static IRCBase *irc_target = NULL;
static PyThreadState *main_thread = NULL;
static GameList query_list;
static BotList bot_list;
static PyObject *irc_hook_list = NULL;
static PyObject *socket_module = NULL;
static wxString module_path;
static bool running_headless = true;

DEFINE_EVENT_TYPE(wxEVT_PYDEBUG)
DEFINE_EVENT_TYPE(wxEVT_GAMEQUERY)

static bool load_modules();
static void load_plugins();
static void print_startup_info();
static int sort_query_list(GameType **a, GameType **b);
static int sort_bot_list(BotType **a, BotType **b);

/* embed functions */
static PyObject *py_debug_msg(PyObject *self, PyObject *args)
{
    char *s;
    int err = 0;

    if (!PyArg_ParseTuple(args, "s|i:debug_msg", &s, &err))
        return NULL;

    wxString msg(s);
    PyPrintDebugNoFmt(err, msg);

    Py_RETURN_NONE;
}

static int sort_query_list(GameType **a, GameType **b)
{
    return (*a)->name.CmpNoCase((*b)->name);
}

static int sort_bot_list(BotType **a, BotType **b)
{
    return (*a)->name.CmpNoCase((*b)->name);
}

//Register a query:
//fragmon.register_query('Game Name', 'GameAbrev' (game id), defaultport, 'exec conn', 'exec pw', 'exec name' overview_cb, all_cb)
//
//Register Game Name on default port.
//'exec str' is the execution string to launch game: '{game_path} +set password {password} +connect {host}:{port}'
//overview_cb is a function which returns a ping and basic overview
//info on the server. all_cb returns all info: ping, overview, rules, players, etc.
//Both functions are called as func(gameid, address, port)
//
//Example:
//fragmon.register_query('Quake 4', 'q4', 28004, q4_query_overview, q4_query_all)
//
//Games are commonly identified on Abbrev so it must be a unique identifier
static PyObject *py_register_query(PyObject *self, PyObject *args, PyObject *kwargs)
{
    char *gname, *gabbrev;
    int port;
    PyObject *over_cb = NULL;
    PyObject *all_cb = NULL;
    char *exec_order = "", *exec_conn = ""; 
    char *exec_pass = "", *exec_name = "";

    static char *kwlist[] = {"name", "abbrev", "port", "over_cb", "all_cb",  
                            "exec_order", "exec_conn", "exec_pass", "exec_name", 
                            NULL}; 
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "ssiOO|ssss:register_query", kwlist,
                &gname, &gabbrev, &port, &over_cb, &all_cb,
                &exec_order, &exec_conn, &exec_pass, &exec_name)) 
        return NULL;
   
    if (!PyCallable_Check(over_cb))
        return PyErr_Format(PyExc_TypeError, "overview cb must be callable");

    if (!PyCallable_Check(all_cb))
        return PyErr_Format(PyExc_TypeError, "all cb must be callable");

    Py_INCREF(over_cb);
    Py_INCREF(all_cb);

    query_list.Add(new GameType(0, gname, gabbrev, port, over_cb, all_cb,
                    exec_order, exec_conn, exec_pass, exec_name));
    
    query_list.Sort(sort_query_list);

    //XXX: this is to make do with broken QueryInput control. the id field shouldn't need to exist
    for (int i = 0; i < query_list.GetCount(); i++)
        query_list[i].id = i;
    
    Py_RETURN_NONE;
}

static PyObject *py_register_scorebot(PyObject *self, PyObject *args)
{
    PyObject *klass = NULL;
    PyObject *name = NULL;

    if (!PyArg_ParseTuple(args, "O:register_scorebot", &klass))
        return NULL;

    int ret = PyObject_IsSubclass(klass, (PyObject *)&PyScoreBotType);
    if (ret < 0)
        return NULL;
    if (!ret)
        return PyErr_Format(PyExc_TypeError, "registered scorebot must descend from fragmon.ScoreBot"); 

    //pluck out name
    if (!PyObject_HasAttrString(klass, "bot_name"))
        return PyErr_Format(PyExc_TypeError, "registered scorebot must have a bot_name field"); 
    name = PyObject_GetAttrString(klass, "bot_name");
    if (!name)
        return NULL;

    if (!PyString_Check(name))
    {
        Py_DECREF(name);
        return PyErr_Format(PyExc_TypeError, "bot_name field must be string");  
    }

    BotType bot(PyString_AS_STRING(name), klass);
    
    //read default options
    if (PyObject_HasAttrString(klass, "bot_options"))
    {
        PyObject *opts = PyObject_GetAttrString(klass, "bot_options");
        if (!opts)
            return NULL;

        if (!PyList_Check(opts))
        {
            Py_DECREF(opts);
            return PyErr_Format(PyExc_TypeError, "bot_options field must be a list");
        }

        for (int i = 0; i < PyList_Size(opts); i++)
        {
            char *key = "", *value = "";
            PyObject *item = PyList_GET_ITEM(opts, i);
            if (!PyArg_ParseTuple(item, "s|s:register_scorebot_parse_opts", &key, &value))
            {
                Py_DECREF(opts);
                return NULL;
            }

            bot.options.Add(new GameRule(key, value));
        }

        Py_DECREF(opts);
    }

    //Finally add in the new bot
    Py_INCREF(klass);
    bot_list.Add(bot); //this will make a copy
    bot_list.Sort(sort_bot_list);
    
    Py_DECREF(name);
    Py_RETURN_NONE;
}

static PyObject *py_get_millis(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":get_millis"))
        return NULL;

    wxLongLong millis = wxGetLocalTimeMillis();

    return PyLong_FromLong(millis.GetLo());
}

static PyObject *py_get_threadid(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":get_threadid"))
        return NULL;

    return PyLong_FromLong(wxThread::GetCurrentId());
}

static PyObject *py_irc_quote(PyObject *self, PyObject *args)
{
    IRCEvent ie;

    if (!irc_target)
        return PyErr_Format(PyExc_RuntimeError, "IRC disconnected");
    
    int i;
    for (i=0; i < PyTuple_Size(args); i++)
    {
        PyObject *arg = PyTuple_GetItem(args, i);
        PyObject *pstr = PyObject_Str(arg);
        if (!pstr)
            return NULL;

        ie << PyString_AS_STRING(pstr);
        Py_DECREF(pstr);
    }

    wxPostEvent(irc_target, ie);

    Py_RETURN_NONE;
}

static PyObject *py_irc_add_hook(PyObject *self, PyObject *args)
{
    PyObject *func;

    if (!PyArg_ParseTuple(args, "O:irc_add_hook", &func))
        return NULL;
    
    if (!PyCallable_Check(func))
        return PyErr_Format(PyExc_TypeError, "hook must be callable");
    
    if (PySequence_Contains(irc_hook_list, func))
        return PyErr_Format(PyExc_IndexError, "hook function already added");

    PyList_Append(irc_hook_list, func);

    Py_RETURN_NONE;
}

static PyObject *py_irc_del_hook(PyObject *self, PyObject *args)
{
    PyObject *func;
    int i;

    if (!PyArg_ParseTuple(args, "O:irc_del_hook", &func))
        return NULL;

    if ((i = PySequence_Index(irc_hook_list, func)) >= 0)
    {
        if (PySequence_DelItem(irc_hook_list, i) < 0)
            return NULL;
    }
    else
        return PyErr_Format(PyExc_IndexError, "cannot delete hook function");
        
    Py_RETURN_NONE;
}

static PyObject *py_irc_get_mask(PyObject *self, PyObject *args)
{
    if (!irc_target)
        return PyErr_Format(PyExc_RuntimeError, "IRC disconnected");

    IRCMask m = irc_target->GetHostmask();

    return PyString_FromString(m.MakeString().c_str());
}

static PyObject *py_irc_get_nick(PyObject *self, PyObject *args)
{
    if (!irc_target)
        return PyErr_Format(PyExc_RuntimeError, "IRC disconnected");

    return PyString_FromString(irc_target->GetNick().c_str());
}

static PyObject *py_irc_get_channels(PyObject *self, PyObject *args)
{
    if (!irc_target)
        return PyErr_Format(PyExc_RuntimeError, "IRC disconnected");

    wxArrayString chans = irc_target->GetChanList();
    PyObject *list = PyList_New(0);
    if (!list)
        return NULL;

    for (int i = 0; i < chans.GetCount(); i++)
    {
        PyObject *str = PyString_FromString(chans[i].c_str());
        if (!str)
        {
            Py_DECREF(list);
            return NULL;
        }

        PyList_Append(list, str);
    }

    return list;
}

static PyMethodDef EmbMethods[] = {
    {"debug_msg", py_debug_msg, METH_VARARGS, "Send msg to screen"},
    {"register_query", (PyCFunction)py_register_query, METH_VARARGS | METH_KEYWORDS, "Register query protocol support"},
    {"get_millis", py_get_millis, METH_VARARGS, "Get current time in ms"},
    {"get_threadid", py_get_threadid, METH_VARARGS, "Get our thread id"},
    {"register_scorebot", py_register_scorebot, METH_VARARGS, "Register scorebot"},
    {NULL, NULL, 0, NULL}
};

static PyMethodDef EmbIRCMethods[] = {
    {"quote", py_irc_quote, METH_VARARGS, "Send message to IRC"},
    {"add_hook", py_irc_add_hook, METH_VARARGS, "Add message output hook"},
    {"del_hook", py_irc_del_hook, METH_VARARGS, "Remove message output hook"},
    {"get_nick", py_irc_get_nick, METH_NOARGS, "Get current nick"},
    {"get_mask", py_irc_get_mask, METH_NOARGS, "Get current mask"},
    {"get_channels", py_irc_get_channels, METH_NOARGS, "Get list of channels"},
    {NULL, NULL, 0, NULL}
};

void PyGetGameList(GameList &gl)
{
    gl = query_list;
}

void PyGetBotList(BotList &bl)
{
    bl = bot_list;
}

void PyPrintDebugNoFmt(bool err, const char *buf)
{
    PyPrintDebug(err, "%s", buf);
}

void PyPrintDebug(bool err, const char *format, ...)
{
    if (!debug_target)
        return;

    wxString msg; 

    va_list va;
    va_start(va, format);
    msg.PrintfV(format, va);

#ifdef __WXDEBUG__
    vprintf(format, va);
#endif

    
    va_end(va);
   
    if (running_headless)
        return;
    
    wxCommandEvent evt(wxEVT_PYDEBUG, -1);
    evt.SetString(msg);
    evt.SetInt(err);
    wxPostEvent(debug_target, evt);
}

#ifdef Py_DEBUG
int PyEmbGetTotalRefcount()
{
    int refcount = -1;
    PyObject *sys, *pyrc;
    PyGILState_STATE gilstate = PyGILState_Ensure();

    sys = pyrc = NULL;

    sys = PyImport_ImportModule("sys");
    if (!sys)
        goto error;

    pyrc = PyObject_CallMethod(sys, "gettotalrefcount", NULL);
    if (!pyrc)
        goto error;

    refcount = PyInt_AsLong(pyrc);

error:
    Py_XDECREF(sys);
    Py_XDECREF(pyrc);

    if (PyErr_Occurred())
        PyErr_Print();

    PyGILState_Release(gilstate);
    return refcount;
}
#endif

void PySetSocketTimeout(float timeout)
{
    if (!socket_module)
        return;

    PyGILState_STATE gilstate = PyGILState_Ensure();
            
    PyObject *ret;
    ret = PyObject_CallMethod(socket_module, "setdefaulttimeout", "(f)", timeout);
    if (ret)
        Py_DECREF(ret);
    else
        PyErr_Print();

    PyGILState_Release(gilstate);
}

float PyGetSocketTimeout()
{
    float to = -1.0;
   
    if (!socket_module)
        return -1.0;

    PyGILState_STATE gilstate = PyGILState_Ensure();
            
    PyObject *ret;
    ret = PyObject_CallMethod(socket_module, "getdefaulttimeout", "", NULL);
    if (ret)
    {
        if (PyFloat_Check(ret))
            to = PyFloat_AS_DOUBLE(ret);

        Py_DECREF(ret);
    }
    else
        PyErr_Print();

    PyGILState_Release(gilstate);

    return to;
}

GameType *PyEmbFindQuery(int id)
{
    GameList &gl = query_list;
    for (int i = 0; i < gl.GetCount(); i++)
    {
        if (gl[i].id == id)
            return &gl[i];
    }

    return NULL;
}

GameType *PyEmbFindQuery(const wxString &abbrev)
{
    GameList &gl = query_list;
    for (int i = 0; i < gl.GetCount(); i++)
    {
        if (gl[i].abbrev == abbrev)
            return &gl[i];
    }

    return NULL;
}

BotType *PyEmbFindBot(const wxString &name)
{
    BotList &bl = bot_list;
    for (int i = 0; i < bl.GetCount(); i++)
    {
        if (bl[i].name == name)
            return &bl[i];
    }

    return NULL;
}

void PyEmbSetDebugTarget(wxEvtHandler *eh)
{
    if (eh)
        running_headless = false;
    debug_target = eh;
}

wxEvtHandler *PyEmbGetDebugTarget()
{
    return debug_target;
}

void PyEmbSetIrcTarget(IRCBase *irc)
{
    irc_target = irc;
}

IRCBase *PyEmbGetIrcTarget()
{
    return irc_target;
}

void PyEmbCallIrcHooks(IRCEvent &evt)
{
    int i, len;
    PyObject *arglist = NULL;

    PyGILState_STATE gilstate = PyGILState_Ensure();

    if (!irc_hook_list)
        goto endtag;

    len = PyList_Size(irc_hook_list);
    if (!len)
        goto endtag;

    arglist = PyList_New(0);
    if (!arglist)
    {
        PyErr_Print();
        goto endtag;
    }

    for (i = 0; i < evt.Args.GetCount(); i++)
    {
        PyObject *arg = PyString_FromString(evt.Args[i].c_str());
        if (!arg)
        {
            PyErr_Print();
            goto endtag;
        }
        else
        {
            PyList_Append(arglist, arg);
            Py_DECREF(arg);
        }
    }

    //calls each function with: (bool error, srcprefix, command, [arg1,arg2,...])
    for (i = 0; i < len; i++)
    {
        PyObject *func = PyList_GET_ITEM(irc_hook_list, i);
        PyObject *ret = PyObject_CallFunction(func, "(issO)", 
                evt.IsError?1:0, evt.Source.c_str(), evt.Command.c_str(), arglist); 
        if (!ret)
            PyErr_Print();
        else
            Py_DECREF(ret);
    }

endtag:
    Py_XDECREF(arglist);
    PyGILState_Release(gilstate);
}

void PyEmbSetModulePath(const wxString &path)
{
    module_path = path;
}

wxString PyEmbGetModulePath()
{
    return module_path;
}

bool PyEmbIsHeadless()
{
    return running_headless;
}

wxString PyEmbGetExcStr()
{
    PyObject *type, *value, *trace;
    wxString ret;

    PyGILState_STATE gilstate = PyGILState_Ensure();
   
    PyErr_Fetch(&type, &value, &trace);
  
    if (value)
    {
        PyObject *str = PyObject_Str(value);
        if (str)
        {
            ret = PyString_AS_STRING(str);
            Py_DECREF(str);
        }
    }
    
    PyErr_Restore(type, value, trace);
    
    PyGILState_Release(gilstate);

    return ret;
}


static bool load_modules()
{
    PyObject *fragmon = NULL, *irc = NULL;

    fragmon = Py_InitModule("fragmon", EmbMethods);
    PyEmbAddWrappers(fragmon);
    irc = Py_InitModule("fragmon.irc", EmbIRCMethods);
    Py_INCREF(irc);
    PyModule_AddObject(fragmon, "irc", irc);
    PyModule_AddStringConstant(fragmon, "APPNAME", FM_APPNAME);
    PyModule_AddStringConstant(fragmon, "VERSION", FM_VERSION);
    PyModule_AddStringConstant(fragmon, "WEBSITE", FM_WEBSITE);
    PyModule_AddObject(fragmon, "MODPATH", PyString_FromString(module_path.c_str()));

    PyObject *path = PySys_GetObject("path");
    if (path)
    {
        wxFileName minilib;
        minilib.AssignDir(module_path);
        minilib.AppendDir("minilib");
        minilib.Normalize();

        PyList_Append(path, PyString_FromString(module_path.c_str()));
        PyList_Append(path, PyString_FromString(minilib.GetFullPath().c_str()));

        PyPrintDebug(false, "Python paths:\n");
        for (int i=0; i < PyList_Size(path); i++)
        {
            char *s = PyString_AsString(PyList_GET_ITEM(path, i));
            if (!s)
                PyErr_Print();
            else
                PyPrintDebug(false, "%s\n", s);
        }
    }
    else
    {
        PyPrintDebug(true, "Fatal: Can't set path\n");
        return false;
    }

    if (!PyImport_ImportModule("startup"))
    {
        PyPrintDebug(true, "Fatal: Cant load startup module\n");
        return false;
    }

    if (!(socket_module = PyImport_ImportModule("socket")))
    {
        PyPrintDebug(true, "Fatal: Can't load socket module\n");
        return false;
    }

    return true;
}

static void load_plugins()
{
    //import all query modules
    //Query modules should call fragmon.register_query(...) to get noticed
    wxDir plugins(module_path);
    wxString fname;
    bool got = plugins.GetFirst(&fname, "query_*.py", wxDIR_FILES);
    while (got)
    {
        char *name = strdup(fname.Left(fname.Len()-3));    
        if (!PyImport_ImportModule(name))
        {
            PyPrintDebug(true, "can't load query module %s\n", name);
            PyErr_Print();   
        }
        delete [] name;

        got = plugins.GetNext(&fname);
    }
}

static void print_startup_info()
{
    PyPrintDebug(false, "\n");

    //Debug: print loaded modules
    GameList gl;
    PyGetGameList(gl);
    for (int i=0; i < gl.GetCount(); i++)
        PyPrintDebug(false, "Registered query: %s, abbrev %s\n", gl[i].name.c_str(), gl[i].abbrev.c_str());
    
    PyPrintDebug(false, "\n");

    //Debug: print scorebots
    BotList bl;
    PyGetBotList(bl);
    for (int i = 0; i < bl.GetCount(); i++)
    {
        BotType &item = bl[i];

        PyPrintDebug(false, "Registered bot: %s, klass %p\n", item.name.c_str(), item.klass);
        for (int j = 0; j < item.options.GetCount(); j++)
            PyPrintDebug(false, "\tOption %s = '%s'\n", item.options[j].key.c_str(), item.options[j].value.c_str());
    }
}

int PyEmbInit()
{
    Py_InitializeEx(0);
    //Py_SetProgramName(argv[0]);
    //PySys_SetArgv(argc, argv);
    GameQueryThread::InitCount(); 
    running_headless = false;
    irc_hook_list = PyList_New(0);

    if (!irc_hook_list || !load_modules())
    {
        wxMessageBox("Failed to initialize Python", "Error");
        if (PyErr_Occurred())
            PyErr_Print();
        return -1;
    }

    PySetSocketTimeout(1.0);
 
    load_plugins(); 
    print_startup_info();
    
    PyEval_InitThreads();
    main_thread = PyEval_SaveThread(); //UNLOCK GIL and save tstate
    return 0;
}

void PyEmbFinalize()
{
    running_headless = true;

    //XXX: is this aight?
    GameQueryThread::WaitForThreads();
   
    if (main_thread)
        PyEval_RestoreThread(main_thread); //REAQUIRE LOCK and restore tstate

    Py_XDECREF(irc_hook_list);

    GameList &gl = query_list;
    for (int i = 0; i < gl.GetCount(); i++)
    {
        Py_XDECREF(gl[i].over_cb);
        Py_XDECREF(gl[i].all_cb);
    }
    gl.Clear();

    for (int i = 0; i < bot_list.GetCount(); i++)
        Py_XDECREF(bot_list[i].klass);
    bot_list.Clear();

    Py_Finalize();
 
    GameQueryThread::FiniCount();
   
    main_thread = NULL;
    irc_hook_list = NULL;
}
