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



#include "pyscorebot.h"
#include "pywrappers.h"

PyScoreBot::~PyScoreBot()
{
    Cleanup();
}

void PyScoreBot::Cleanup()
{
    if (!m_self)
        return;

    PyGILState_STATE gs = PyGILState_Ensure();

    //call the cleanup method to instruct the instance to 
    //remove any references to itself (IE irc callbacks)
    PyObject *ret = PyObject_CallMethod(m_self, "__cleanup__", "", NULL);
    if (ret)
        Py_DECREF(ret);
    else
        PyErr_Print();
    
    PyScoreBot_Detach(m_self); 
    Py_DECREF(m_self);

    m_self = NULL;
    
    PyGILState_Release(gs);
}

void PyScoreBot::Save()
{
    //Save options to list
    RuleList &rl = m_type.options;
    for (int i = 0; i < rl.GetCount(); i++)
        rl[i].value = GetOption(rl[i].key);
   
    Cleanup();
    m_type.klass = NULL;
}

bool PyScoreBot::Reload()
{
    BotType *bt = PyEmbFindBot(m_type.name);

    //was the bot removed?
    if (!bt)
       return false;

    //reinitialize, and restore options
    RuleList opts = m_type.options;
    if (!Init(bt))
        return false;

    SetOptions(opts);

    return true;
}

bool PyScoreBot::Init(BotType *bt)
{
    bool ok = false;
    if (m_self)
        Cleanup();

    PyGILState_STATE gs = PyGILState_Ensure();

    //dbl check
    int ret = PyObject_IsSubclass(bt->klass, (PyObject *)&PyScoreBotType);
    if (ret < 1)
        goto error; 
    
    //make instance of class 
    m_self = PyObject_CallFunction(bt->klass, "", NULL);
    if (!m_self)
        goto error;

    //set up our instance
    PyScoreBot_Attach(m_self, this, false);
    m_type = *bt;
    
    ok = true;

error:
    if (PyErr_Occurred())
        PyErr_Print();

    PyGILState_Release(gs);
    return ok;
}

wxString PyScoreBot::GetOption(const wxString &key)
{
    wxASSERT(m_self);
    wxString sret;

    PyGILState_STATE gs = PyGILState_Ensure();
    
    PyObject *ret = PyObject_CallMethod(m_self, "get_option", "s", key.c_str());
    if (ret)
    {
        PyObject *pystr = PyObject_Str(ret);
        if (pystr)
        {
            sret = PyString_AS_STRING(pystr);
            Py_DECREF(pystr);
        }
        else
            PyErr_Print();

        Py_DECREF(ret);
    }
    else
        PyErr_Print();

    PyGILState_Release(gs);

    return sret;
}

PyObject *PyScoreBot::wrap_GetOption(PyObject *self, PyObject *args)
{
    wxASSERT(m_self);

    PyObject *key, *value;
    PyObject *sval;

    if (!PyArg_ParseTuple(args, "S", &key))
        return NULL;

    value = PyObject_GetAttr(m_self, key);
    if (!value)
        return NULL;
   
    sval = PyObject_Str(value);
    Py_DECREF(value);

    if (!sval)
        return NULL;
    
    return sval;
}

void PyScoreBot::SetOptions(const RuleList &rules)
{
    for (int i = 0; i < rules.GetCount(); i++)
        SetOption(rules[i].key, rules[i].value);
}

wxString PyScoreBot::SetOption(const wxString &key, const wxString &value)
{
    wxASSERT(m_self);
    wxString sret;

    PyGILState_STATE gs = PyGILState_Ensure();
    
    PyObject *ret = PyObject_CallMethod(m_self, "set_option", "ss", key.c_str(), value.c_str());
    if (ret)
    {
        if (PyString_Check(ret))
            sret = PyString_AS_STRING(ret);

        Py_DECREF(ret);
    }
    else
        PyErr_Print();

    PyGILState_Release(gs);
   
    return sret;
}

PyObject *PyScoreBot::wrap_SetOption(PyObject *self, PyObject *args)
{
    wxASSERT(m_self);

    PyObject *key, *value;
    
    if (!PyArg_ParseTuple(args, "SS", &key, &value))
        return NULL;

    printf("PyScoreBot: Setting KV, %s = %s\n", PyString_AS_STRING(key), PyString_AS_STRING(value));
    
    if (PyObject_SetAttr(m_self, key, value) < 0)
        return NULL;
    
    return PyString_FromString("");
}

//These can be overridden by python code
void PyScoreBot::Broadcast()
{
    wxASSERT(m_self);

    PyGILState_STATE gs = PyGILState_Ensure();
    
    PyObject *ret = PyObject_CallMethod(m_self, "broadcast", "", NULL);
    if (ret)
        Py_DECREF(ret);
    else
        PyErr_Print();

    PyGILState_Release(gs);
}

void PyScoreBot::BroadcastPlayers()
{
    wxASSERT(m_self);

    PyGILState_STATE gs = PyGILState_Ensure();

    PyObject *ret = PyObject_CallMethod(m_self, "broadcast_players", "", NULL);
    if (ret)
        Py_DECREF(ret);
    else
        PyErr_Print();

    PyGILState_Release(gs);
}

//GameData accessors. These make copies (owned by the wrapper object) for safety's sake.
//A Python scorebot can save as much data as it wants, as memory lasts.

PyObject *PyScoreBot::wrap_GetOverview(PyObject *self)
{
    if (!m_last_info)
        Py_RETURN_NONE;
    
    PyObject *wrap = PyOverviewType.tp_alloc(&PyOverviewType, 0);
    if (!wrap)
        return NULL;

    PyOverview_Attach(wrap, new GameInfo(*m_last_info), true);

    return wrap;
}

PyObject *PyScoreBot::wrap_GetPlayers(PyObject *self)
{
    PyObject *list = PyList_New(0);
    if (!list) 
        goto error;

    for (int i = 0; i < m_last_players.GetCount(); i++)
    {
        PyObject *wrap = PyPlayerType.tp_alloc(&PyPlayerType, 0);
        if (!wrap)
            goto error;

        PyPlayer_Attach(wrap, new GamePlayer(m_last_players[i]), true);
        
        PyList_Append(list, wrap);
        Py_DECREF(wrap);
    }

    return list;

error:
    Py_XDECREF(list);
    return NULL;
}

PyObject *PyScoreBot::wrap_GetRules(PyObject *self)
{
    PyObject *list = PyList_New(0);
    if (!list) 
        goto error;

    for (int i = 0; i < m_last_rules.GetCount(); i++)
    {
        PyObject *wrap = PyRuleType.tp_alloc(&PyRuleType, 0);
        if (!wrap)
            goto error;

        PyRule_Attach(wrap, new GameRule(m_last_rules[i]), true);
        
        PyList_Append(list, wrap);
        Py_DECREF(wrap);
    }

    return list;

error:
    Py_XDECREF(list);
    return NULL;
}

PyObject *PyScoreBot::wrap_SubFormat(PyObject *self, PyObject *args, PyObject *kwargs)
{
    char *format = NULL;
    PyObject *vars = NULL;
    VarMap vmap;

    static char *kwlist[] = {"format", "vars", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|sO:sub_format", kwlist, &format, &vars))
        return NULL;

    if (vars)
    {
        if (!PyList_Check(vars))
            return PyErr_Format(PyExc_TypeError, "vars must be a list");

        for (int i = 0; i < PyList_Size(vars); i++)
        {
            char *key, *value;
            if (!PyArg_ParseTuple(PyList_GET_ITEM(vars, i), "ss:sub_format_parse", &key, &value))
                return NULL;

            vmap[key] = value;
        }
    }

    wxString fmt, ret;

    if (format) 
        fmt = format;
    else
        fmt = GetFormat();

    ret = SubFormat(fmt, vmap);

    return PyString_FromString(ret.c_str());
}

