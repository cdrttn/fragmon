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
#include "pywrappers.h"
#include "pyscorebot.h"
#include <stdlib.h>
#include <errno.h>

template <class T>
static int set_num(T &num, PyObject *pyint)
{
    if (PyInt_Check(pyint))
        num = PyInt_AsLong(pyint);
    else if (PyLong_Check(pyint))
        num = PyLong_AsLong(pyint);
    else if (PyString_Check(pyint))
    {
        char *end;
        char *tmp = PyString_AS_STRING(pyint);
        num = strtol(tmp, &end, 0);
        if (errno == ERANGE || errno == EINVAL || end == tmp)
            PyErr_SetString(PyExc_TypeError, "member expects number (unable to convert)");
    }
    else
        PyErr_SetString(PyExc_TypeError, "member expects number");

    if (PyErr_Occurred())
        return 0;
    
    return 1;
}

template <class T>
static int set_unum(T &num, PyObject *pyint)
{
    if (PyInt_Check(pyint))
        num = PyInt_AsUnsignedLongMask(pyint);
    else if (PyLong_Check(pyint))
        num = PyLong_AsUnsignedLong(pyint);
    else if (PyString_Check(pyint))
    {
        char *end;
        char *tmp = PyString_AS_STRING(pyint);
        num = strtoul(tmp, &end, 0);
        if (errno == ERANGE || errno == EINVAL || end == tmp)
            PyErr_SetString(PyExc_TypeError, "member expects unsigned number (unable to convert)");
    }
    else
        PyErr_SetString(PyExc_TypeError, "member expects unsigned number");

    if (PyErr_Occurred())
        return 0;

    return 1;
}
#define set_int set_num<int>
#define set_uint set_unum<int>
#define set_long set_num<long>
#define set_ulong set_unum<unsigned long>

static int set_wxstr(wxString &wxstr, PyObject *pystr)
{
    if (!PyString_Check(pystr))
    {
        PyErr_SetString(PyExc_TypeError, "member expects string");
        return 0;
    }

    wxstr = PyString_AsString(pystr);
    return 1;
}

static int set_bool(bool &cbool, PyObject *pybool)
{
    bool conv = false;

    if (PyString_Check(pybool))
    {
        conv = true;
        pybool = PyNumber_Int(pybool);
        if (!pybool)
            goto error;
    }
    
    if (!PyBool_Check(pybool) && !PyInt_Check(pybool) && !PyLong_Check(pybool))
    {
        PyErr_SetString(PyExc_TypeError, "member expects boolean");
        goto error;
    }

error:
    if (conv)
        Py_XDECREF(pybool);
    if (PyErr_Occurred())
        return 0;
    
    cbool = PyInt_AsLong(pybool)? true : false;
    return 1;
}

//Generic getters/setters

template <class cT>
static PyData<cT> *check_getdata(PyObject *obj)
{
    PyData<cT> *self = (PyData<cT> *) obj;
    if (self->m_data == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "wrapped object was detatched");
        return NULL;
    }

    return self;
}

//wxString -> PyString
template <class cT, wxString cT::*memT>
static PyObject *PyData_GetString(PyObject *obj, void *)
{
    PyData<cT> *self = check_getdata<cT>(obj);

    return self? PyString_FromString((self->m_data->*memT).c_str()) : NULL;
}

template <class cT, wxString (cT::*memT)() const>
static PyObject *PyData_GetStringM(PyObject *obj, void *)
{
    PyData<cT> *self = check_getdata<cT>(obj);

    return self? PyString_FromString((self->m_data->*memT)().c_str()) : NULL;
}


//num -> PyInt
template <class cT, class dataT, dataT cT::*memT>
static PyObject *PyData_GetLong(PyObject *obj, void *)
{
    PyData<cT> *self = check_getdata<cT>(obj);

    return self? PyInt_FromLong(self->m_data->*memT) : NULL;
}
template <class cT, class dataT, dataT (cT::*memT)() const>
static PyObject *PyData_GetLongM(PyObject *obj, void *)
{
    PyData<cT> *self = check_getdata<cT>(obj);

    return self? PyInt_FromLong((self->m_data->*memT)()) : NULL;
}

//bool -> PyBool
template <class cT, bool cT::*memT>
static PyObject *PyData_GetBool(PyObject *obj, void *)
{
    PyData<cT> *self = check_getdata<cT>(obj);
   
    return self? PyBool_FromLong(self->m_data->*memT?1 : 0) : NULL;
}
template <class cT, bool (cT::*memT)() const>
static PyObject *PyData_GetBoolM(PyObject *obj, void *)
{
    PyData<cT> *self = check_getdata<cT>(obj);
   
    return self? PyBool_FromLong((self->m_data->*memT)()?1 : 0) : NULL;
}

//wxColour -> PyInt
template <class cT, wxColour cT::*memT>
static PyObject *PyData_GetColor(PyObject *obj, void *)
{
    PyData<cT> *self = check_getdata<cT>(obj);
    unsigned long color;

    if (!self)
        return NULL;

    color = PyGetColor(self->m_data->*memT);
    return PyLong_FromLong(color);
}
template <class cT, wxColour (cT::*memT)() const>
static PyObject *PyData_GetColorM(PyObject *obj, void *)
{
    PyData<cT> *self = check_getdata<cT>(obj);
    unsigned long color;

    if (!self)
        return NULL;

    color = PyGetColor((self->m_data->*memT)());
    return PyLong_FromLong(color);
}

//Setters
template <class cT>
static PyData<cT> *check_setdata(PyObject *obj, PyObject *value)
{
    PyData<cT> *self = (PyData<cT> *) obj;

    if (self->m_data == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "wrapped object was detatched");
        return NULL;
    }
    
    if (value == NULL)
    {
        PyErr_SetString(PyExc_AttributeError, "can't delete member"); 
        return NULL;
    }

    return self;
}

//PyInt color -> wxColour
template <class cT, wxColour cT::*memT>
static int PyData_SetColor(PyObject *obj, PyObject *value, void *)
{
    PyData<cT> *self = check_setdata<cT>(obj, value);
    unsigned long color;

    if (!self || !set_ulong(color, value))
        return -1;

    self->m_data->*memT = PyMakeColor(color);

    return 0;
}
template <class cT, void (cT::*memT)(const wxColour&)>
static int PyData_SetColorM(PyObject *obj, PyObject *value, void *)
{
    PyData<cT> *self = check_setdata<cT>(obj, value);
    unsigned long color;

    if (!self || !set_ulong(color, value))
        return -1;

    (self->m_data->*memT)(PyMakeColor(color));

    return 0;
}

//PyString -> wxString
template <class cT, wxString cT::*memT>
static int PyData_SetString(PyObject *obj, PyObject *value, void *)
{
    PyData<cT> *self = check_setdata<cT>(obj, value);

    return (self && set_wxstr(self->m_data->*memT, value))? 0 : -1;
}
template <class cT, void (cT::*memT)(const wxString&)>
static int PyData_SetStringM(PyObject *obj, PyObject *value, void *)
{
    PyData<cT> *self = check_setdata<cT>(obj, value);
    wxString tmp;
    if (!self || !set_wxstr(tmp, value))
        return -1;

    (self->m_data->*memT)(tmp);
    return 0;
}

//PyLong -> some signed int
template <class cT, class dataT, dataT cT::*memT>
static int PyData_SetLong(PyObject *obj, PyObject *value, void *)
{
    PyData<cT> *self = check_setdata<cT>(obj, value);
   
    return (self && set_num<dataT>(self->m_data->*memT, value))? 0 : -1;
}
template <class cT, class dataT, void (cT::*memT)(dataT)>
static int PyData_SetLongM(PyObject *obj, PyObject *value, void *)
{
    PyData<cT> *self = check_setdata<cT>(obj, value);
    dataT num;
    if (!self || !set_num<dataT>(num, value))
        return -1;
    
    (self->m_data->*memT)(num);
    return 0;
}

//unsigned
template <class cT, class dataT, dataT cT::*memT>
static int PyData_SetULong(PyObject *obj, PyObject *value, void *)
{
    PyData<cT> *self = check_setdata<cT>(obj, value);

    return (self && set_unum<dataT>(self->m_data->*memT, value))? 0 : -1;
}
template <class cT, class dataT, void (cT::*memT)(dataT)>
static int PyData_SetULongM(PyObject *obj, PyObject *value, void *)
{
    PyData<cT> *self = check_setdata<cT>(obj, value);
    dataT num;
    if (!self || !set_unum<dataT>(num, value))
        return -1;
    
    (self->m_data->*memT)(num);
    return 0;
}

//PyLong/PyBool -> bool
template <class cT, bool cT::*memT>
static int PyData_SetBool(PyObject *obj, PyObject *value, void *)
{
    PyData<cT> *self = check_setdata<cT>(obj, value);

    return (self && set_bool(self->m_data->*memT, value))? 0 : -1;
}
template <class cT, void (cT::*memT)(bool)>
static int PyData_SetBoolM(PyObject *obj, PyObject *value, void *)
{
    PyData<cT> *self = check_setdata<cT>(obj, value);
    bool tmp;
    if (!self || !set_bool(tmp, value))
        return -1;

    (self->m_data->*memT)(tmp);
    return 0;
}

//method proxies
template <class cT, PyObject *(cT::*memT)(PyObject *self, PyObject *args, PyObject *kwargs)>
static PyObject *PyData_MethKW(PyObject *obj, PyObject *args, PyObject *kwargs)
{
    PyData<cT> *self = check_getdata<cT>(obj);
    if (!self)
        return NULL;

    return (self->m_data->*memT)(obj, args, kwargs);
}

template <class cT, PyObject *(cT::*memT)(PyObject *self, PyObject *args)>
static PyObject *PyData_MethVA(PyObject *obj, PyObject *args)
{
    PyData<cT> *self = check_getdata<cT>(obj);
    if (!self)
        return NULL;

    return (self->m_data->*memT)(obj, args);
}

template <class cT, PyObject *(cT::*memT)(PyObject *self)>
static PyObject *PyData_MethNA(PyObject *obj, PyObject *args)
{
    PyData<cT> *self = check_getdata<cT>(obj);
    if (!self)
        return NULL;

    return (self->m_data->*memT)(obj);
}


//generic constructor/destructor
template <class cT>
static void PyData_dealloc(PyObject *obj)
{
    PyData<cT> *self = (PyData<cT> *) obj;

    //free owned heap memory here
    if (self->m_data && self->m_owned)
        delete self->m_data;

    self->ob_type->tp_free((PyObject*)self);
}

template <class cT>
static PyObject *PyData_new(PyTypeObject *type, PyObject *, PyObject *)
{
    PyData<cT> *self = (PyData<cT> *) type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    //perform allocation, etc
    if (!self->m_data)
    {
        self->m_data = new cT; 
        self->m_owned = 1;
    }
    
    return (PyObject *)self;
}
////////////


//Player item wrapper
static int
PyPlayer_init(PyData<GamePlayer> *self, PyObject *args, PyObject *kwds)
{
    int ret;
    char *name = "", *team = "";
    static char *kwlist[] = {"name", "score", "ping", "team", "teamid", "id", NULL};

    if (self->m_data == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "wrapped object was detatched");
        return -1;
    }

    //perform initialization... called from __init__
    ret = PyArg_ParseTupleAndKeywords(args, kwds, "|siisii", kwlist, 
            &name, &self->m_data->score, &self->m_data->ping, &team,
            &self->m_data->teamid, &self->m_data->id);

    if (!ret)
        return -1; 

    self->m_data->name = name;
    self->m_data->team = team;

    return 0;
}

#define player_get_str(mem) PyData_GetString<GamePlayer, &GamePlayer::mem>
#define player_get_int(mem) PyData_GetLong<GamePlayer, int, &GamePlayer::mem>
#define player_set_str(mem) PyData_SetString<GamePlayer, &GamePlayer::mem>
#define player_set_int(mem) PyData_SetLong<GamePlayer, int, &GamePlayer::mem>

//template <class pyT, cT, wxString cT::*memT>
//specialized getters/setters
static PyGetSetDef PyPlayer_getseters[] = {
    {"name",        player_get_str(name), player_set_str(name)},
    {"score",       player_get_int(score), player_set_int(score)},
    {"ping",        player_get_int(ping), player_set_int(ping)},
    {"team",        player_get_str(team), player_set_str(team)},
    {"teamid",      player_get_int(teamid), player_set_int(teamid)},
    {"id",          player_get_int(id), player_set_int(id)},
    {NULL}
};

PyTypeObject PyPlayerType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "fragmon.Player",  /*tp_name*/
    sizeof(PyData<GamePlayer>),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    PyData_dealloc<GamePlayer>, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PyPlayer objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    0,                      /* tp_methods */
    0,                      /* tp_members */
    PyPlayer_getseters,        /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyPlayer_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyData_new<GamePlayer>, /* tp_new */
};

//Rule item wrapper
static int
PyRule_init(PyData<GameRule> *self, PyObject *args, PyObject *kwds)
{
    int ret;
    char *key = "", *value = "";
    static char *kwlist[] = {"key", "value", NULL};

    if (self->m_data == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "wrapped object was detatched");
        return -1;
    }

    //perform initialization... called from __init__
    ret = PyArg_ParseTupleAndKeywords(args, kwds, "|ss", kwlist, &key, &value);

    if (!ret)
        return -1; 

    self->m_data->key = key;
    self->m_data->value = value;

    return 0;
}

#define rule_get_str(mem) PyData_GetString<GameRule, &GameRule::mem>
#define rule_set_str(mem) PyData_SetString<GameRule, &GameRule::mem>

//specialized getters/setters
static PyGetSetDef PyRule_getseters[] = {
    {"key",        rule_get_str(key), rule_set_str(key)},
    {"value",      rule_get_str(value), rule_set_str(value)},
    {NULL}
};

PyTypeObject PyRuleType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "fragmon.Rule",  /*tp_name*/
    sizeof(PyData<GameRule>),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    PyData_dealloc<GameRule>, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PyRule objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    0,                      /* tp_methods */
    0,                      /* tp_members */
    PyRule_getseters,        /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyRule_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyData_new<GameRule>,                 /* tp_new */
};

//Overview wrapper module
static int
PyOverview_init(PyData<GameInfo> *self, PyObject *args, PyObject *kwds)
{
    int ret, pass = 0;
    char *hostname = "", *map = "", *type = "";
    char *mode = "", *version = "", *mod = "";
    char *server = "", *gtime = "";
    char *teamAname = "", *teamBname = "";
    unsigned long teamAcolor = 0xffffff, teamBcolor = 0xffffff;

    static char *kwlist[] = {"ping", "curplayers", "maxplayers", "password", 
        "hostname", "map", "type", "mode", "version", "mod",
        "port", "server", "time", "teamAname", "teamBname", 
        "teamAscore", "teamBscore", "teamAcolor", "teamBcolor", NULL};
    //perform initialization... called from __init__

    if (self->m_data == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "wrapped object was detatched");
        return -1;
    }

    ret = PyArg_ParseTupleAndKeywords(args, kwds, "|iiiissssssissssiikk", kwlist, 
             &self->m_data->ping, &self->m_data->curplayers, &self->m_data->maxplayers,
             &pass, &hostname, &map, &type, &mode, &version, &mod, 
             &self->m_data->port, &server, &gtime, &teamAname, &teamBname, 
             &self->m_data->teamAscore, &self->m_data->teamBscore, &teamAcolor, &teamBcolor);

    if (!ret)
        return -1; 

    self->m_data->password = pass? true : false;
    self->m_data->hostname = hostname;
    self->m_data->map = map;
    self->m_data->type = type;
    self->m_data->mode = mode;
    self->m_data->version = version;
    self->m_data->mod = mod;
    self->m_data->time = gtime;
    self->m_data->server = server;
    self->m_data->teamAname = teamAname;
    self->m_data->teamBname = teamBname;
    self->m_data->teamAcolor = PyMakeColor(teamAcolor);
    self->m_data->teamBcolor = PyMakeColor(teamBcolor);
        
    return 0;
}

#define over_get_str(mem) PyData_GetString<GameInfo, &GameInfo::mem>
#define over_get_int(mem) PyData_GetLong<GameInfo, int, &GameInfo::mem>
#define over_get_bool(mem) PyData_GetBool<GameInfo, &GameInfo::mem>
#define over_get_col(mem) PyData_GetColor<GameInfo, &GameInfo::mem>
#define over_set_str(mem) PyData_SetString<GameInfo, &GameInfo::mem>
#define over_set_int(mem) PyData_SetLong<GameInfo, int, &GameInfo::mem>
#define over_set_bool(mem) PyData_SetBool<GameInfo, &GameInfo::mem>
#define over_set_col(mem) PyData_SetColor<GameInfo, &GameInfo::mem>

//specialized getters/setters
static PyGetSetDef PyOverview_getseters[] = {
    {"port",        over_get_int(port),         over_set_int(port)},
    {"server",      over_get_str(server),       over_set_str(server)},
    {"ping",        over_get_int(ping),         over_set_int(ping)},
    {"curplayers",  over_get_int(curplayers),   over_set_int(curplayers)},
    {"maxplayers",  over_get_int(maxplayers),   over_set_int(maxplayers)},
    {"password",    over_get_bool(password),    over_set_bool(password)},
    {"hostname",    over_get_str(hostname),     over_set_str(hostname)},
    {"map",         over_get_str(map),          over_set_str(map)},
    {"type",        over_get_str(type),         over_set_str(type)},
    {"mode",        over_get_str(mode),         over_set_str(mode)},
    {"mod",         over_get_str(mod),          over_set_str(mod)},
    {"version",     over_get_str(version),      over_set_str(version)},
    {"time",        over_get_str(time),         over_set_str(time)},
    {"teamAname",   over_get_str(teamAname),    over_set_str(teamAname)},
    {"teamBname",   over_get_str(teamBname),    over_set_str(teamBname)},
    {"teamAscore",  over_get_int(teamAscore),   over_set_int(teamAscore)},
    {"teamBscore",  over_get_int(teamBscore),   over_set_int(teamBscore)},
    {"teamAcolor",  over_get_col(teamAcolor),   over_set_col(teamAcolor)},
    {"teamBcolor",  over_get_col(teamBcolor),   over_set_col(teamBcolor)},
    {NULL}
};

PyTypeObject PyOverviewType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "fragmon.Overview",  /*tp_name*/
    sizeof(PyData<GameInfo>),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    PyData_dealloc<GameInfo>, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PyOverview objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    0,                      /* tp_methods */
    0,                      /* tp_members */
    PyOverview_getseters,        /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyOverview_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyData_new<GameInfo>,                 /* tp_new */
};



//ScoreBot wrapper module

//for dbg
static PyObject *bottestnew(PyTypeObject *type, PyObject *, PyObject *)
{
    PyData<PyScoreBot> *self = (PyData<PyScoreBot> *) type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    if (!self->m_data)
    {
        self->m_data = new PyScoreBot; 
        self->m_owned = 1;
    }

    self->m_data->SetSelf(self);
    
    return (PyObject *)self;
}

#define bot_get_str(mem) PyData_GetStringM<ScoreBot, &ScoreBot::mem>
#define bot_get_int(mem) PyData_GetLongM<ScoreBot, int, &ScoreBot::mem>
#define bot_get_bool(mem) PyData_GetBoolM<ScoreBot, &ScoreBot::mem>
#define bot_set_str(mem) PyData_SetStringM<ScoreBot, &ScoreBot::mem>
#define bot_set_int(mem) PyData_SetLongM<ScoreBot, int, &ScoreBot::mem>
#define bot_set_bool(mem) PyData_SetBoolM<ScoreBot, &ScoreBot::mem>

#define bot_meth_va(mem) PyData_MethVA<PyScoreBot, &PyScoreBot::mem>
#define bot_meth_kw(mem) (ternaryfunc)PyData_MethKW<PyScoreBot, &PyScoreBot::mem>
#define bot_meth_na(mem) PyData_MethNA<PyScoreBot, &PyScoreBot::mem>

//specialized getters/setters
static PyGetSetDef PyScoreBot_getseters[] = {
    //should be immutable ??
    {"host",            bot_get_str(GetHost),        bot_set_str(SetHost)},
    {"port",            bot_get_int(GetPort),        bot_set_int(SetPort)},
    {"game",            bot_get_str(GetGame),        bot_set_str(SetGame)},

    //other properties from ScoreBot base
    {"running",         bot_get_bool(IsRunning)},
    {"format",          bot_get_str(GetFormat),      bot_set_str(SetFormat)},
    {"channel",         bot_get_str(GetChannel),     bot_set_str(SetChannel)},
    {"interval",        bot_get_int(GetInterval),    bot_set_int(SetInterval)},
    {"title",           bot_get_str(GetTitle),       bot_set_str(SetTitle)},
    {"name_a",          bot_get_str(GetNameA),       bot_set_str(SetNameA)},
    {"name_b",          bot_get_str(GetNameB),       bot_set_str(SetNameB)},
    {"score_a",         bot_get_str(GetScoreA),      bot_set_str(SetScoreA)},
    {"score_b",         bot_get_str(GetScoreB),      bot_set_str(SetScoreB)},
    {"time",            bot_get_str(GetTime),        bot_set_str(SetTime)},
    {NULL}
};

//Methods for object
static PyMethodDef PyScoreBot_methods[] = {
    {"start",               (PyCFunction)bot_meth_na(wrap_Start),           METH_NOARGS, "Start scorebot"},
    {"stop",                (PyCFunction)bot_meth_na(wrap_Stop),            METH_NOARGS, "Stop scorebot"},
    {"pulse",               (PyCFunction)bot_meth_na(wrap_Pulse),           METH_NOARGS, "Pulse scorebot"},
    {"broadcast",           (PyCFunction)bot_meth_na(wrap_Broadcast),       METH_NOARGS, "override to broadcast scores"},
    {"broadcast_players",   (PyCFunction)bot_meth_na(wrap_BroadcastPlayers), METH_NOARGS, "override to broadcast players"},
    {"__cleanup__",         (PyCFunction)bot_meth_na(wrap_Cleanup),         METH_NOARGS, "override to consider cleanup measures"},
    {"get_overview",        (PyCFunction)bot_meth_na(wrap_GetOverview),     METH_NOARGS, "Get (copy of) Overview data from last query"},
    {"get_players",         (PyCFunction)bot_meth_na(wrap_GetPlayers),      METH_NOARGS, "Get (copy of) Players data from last query"},
    {"get_rules",           (PyCFunction)bot_meth_na(wrap_GetRules),        METH_NOARGS, "Get (copy of) Rules data from last query"},
    {"sub_format",          (PyCFunction)bot_meth_kw(wrap_SubFormat),       METH_VARARGS|METH_KEYWORDS, "Substitute variables in a varstring"},
    {"get_option",          (PyCFunction)bot_meth_va(wrap_GetOption),       METH_VARARGS, "Get an option"},
    {"set_option",          (PyCFunction)bot_meth_va(wrap_SetOption),       METH_VARARGS, "Set an option"},
    {NULL}  /* Sentinel */
};

PyTypeObject PyScoreBotType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "fragmon.ScoreBot",  /*tp_name*/
    sizeof(PyData<PyScoreBot>),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    PyData_dealloc<PyScoreBot>, /*tp_dealloc*/
    //PyScoreBot_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PyScoreBot objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyScoreBot_methods,   /* tp_methods */
    0,                      /* tp_members */
    PyScoreBot_getseters,        /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    //bottestnew,                 /* tp_new */
    PyType_GenericNew,                 /* tp_new */
};


//functions
bool PyEmbAddWrappers(PyObject *m)
{
    if (PyType_Ready(&PyOverviewType) < 0)
        return false;

    if (PyType_Ready(&PyRuleType) < 0)
        return false;

    if (PyType_Ready(&PyPlayerType) < 0)
        return false;

    if (PyType_Ready(&PyScoreBotType) < 0)
        return false;

    Py_INCREF(&PyOverviewType);
    Py_INCREF(&PyRuleType);
    Py_INCREF(&PyPlayerType);
    Py_INCREF(&PyScoreBotType);

    PyModule_AddObject(m, "Overview", (PyObject *)&PyOverviewType);
    PyModule_AddObject(m, "Rule", (PyObject *)&PyRuleType);
    PyModule_AddObject(m, "Player", (PyObject *)&PyPlayerType);
    PyModule_AddObject(m, "ScoreBot", (PyObject *)&PyScoreBotType);

    return true;
}

#ifdef WRAPTEST
//testing

static PyObject *py_test_check(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *over = NULL, *teams = NULL, *rule = NULL, *player = NULL;
    static char *kwlist[] = {"overview", "teams", "rule", "player", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOOO", kwlist, &over, &teams, &rule, &player))
        return NULL;

    if (over)
        if (!PyOverview_Check(over))
            puts("non overview"); 
    if (teams)
        if (!PyTeams_Check(teams))
            puts("non teams"); 
    if (rule)
        if (!PyRule_Check(rule))
            puts("non rule"); 
    if (player)
        if (!PyPlayer_Check(player))
            puts("non player"); 

    if (!PyOverview_Detach(over))
        return NULL;

    if (!PyTeams_Detach(teams))
        return NULL;

    if (!PyRule_Detach(rule))
        return NULL;

    if (!PyPlayer_Detach(player))
        return NULL;
    
    Py_RETURN_NONE;
}

static PyMethodDef module_methods[] = {
    {"test_check", (PyCFunction)py_test_check, METH_VARARGS | METH_KEYWORDS, "test types"},
    {NULL}  /* Sentinel */
};

PyMODINIT_FUNC
initpywrappers(void) 
{
    PyObject* m;
    m = Py_InitModule3("pywrappers", module_methods,
                       "Example module that creates an extension type.");

    if (m == NULL)
      return;
    
    PyEmbAddWrappers(m);
}
#endif //TESTWRAP
