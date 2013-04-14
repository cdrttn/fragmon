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

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(RuleList);
WX_DEFINE_OBJARRAY(PlayerList);

bool GameData::ParseOverview(PyObject *overview, const wxString &host, int port)
{
    int ping, curplayers, maxplayers, password;
    char *hostname = "", *map = "", *type = "", *mode = "";
    char *mod = "", *version = "", *time = "";

    if (!overview)
        return false;

    if (!PyOverview_Check(overview))
    {
        PyErr_SetString(PyExc_TypeError, "overview must be an instance of the overview object");
        return false;
    }
    
    this->info = PyOverview_Detach(overview);
    if (!this->info)
    {
        PyErr_SetString(PyExc_TypeError, "Error: overview object previously detached.");
        return false;
    }

    this->info->server = host; 
    this->info->port = port;
    
    return true;
}

wxColour PyMakeColor(unsigned long value)
{
    unsigned char r, g, b;
    r = (value & 0xff0000) >> 16;
    g = (value & 0xff00) >> 8;
    b = value & 0xff;
    wxColour ret(r,g,b);
   
    return ret;
}

unsigned long PyGetColor(const wxColour &color)
{
    return (color.Red() << 16 | color.Green() << 8 | color.Blue());
}

bool GameData::ParsePlayers(PyObject *players)
{
    int len, i;

    if (!players)
        return false;

    if (!PyList_Check(players))
    {
        PyErr_SetString(PyExc_TypeError, "player list must be a list");
        return false; 
    }
    
    len = PyList_Size(players);
    this->players = new PlayerList();
    this->players->Alloc(len);
    for (i=0; i < len; i++)
    {
        PyObject *playertup = PyList_GET_ITEM(players, i);

        if (PyPlayer_Check(playertup))
        {
            GamePlayer *gp = PyPlayer_Detach(playertup);
            if (!gp)
                PyPrintDebug(true, "Warning: skipping player list item %d. Player object detached.\n", i);
            else
                this->players->Add(gp);
        }
        else
            PyPrintDebug(true, "Warning: skipping player list item %d. It is not a player object.\n", i);
    }

    return true;
}

bool GameData::ParseRules(PyObject *rules)
{
    int len, i;

    if (!rules)
        return false;

    if (!PyList_Check(rules))
    {
        PyErr_SetString(PyExc_TypeError, "rules list must be a list");
        return false;
    }
    
    len = PyList_Size(rules);
    this->rules = new RuleList();
    this->rules->Alloc(len);
    for (i=0; i < len; i++)
    {
        PyObject *ruletup = PyList_GET_ITEM(rules, i);

        if (PyRule_Check(ruletup))
        {
            GameRule *gr = PyRule_Detach(ruletup);
            if (!gr)
                PyPrintDebug(true, "Warning: skipping rule list item %d. Rule object detached.\n", i);
            else
                this->rules->Add(gr);
        }
        else
            PyPrintDebug(true, "Warning: skipping rule list item %d. It is not a rule object.\n", i);
    }

    return true;
}

bool GameData::MakeGdata(PyObject *overview, const wxString &host, int port, 
        PyObject *players, PyObject *rules)
{
    if (!ParseOverview(overview, host, port)) 
        return false;

    if (players || rules)
    {
        if (!ParsePlayers(players))
            return false;
        if (!ParseRules(rules))
            return false;
    }

    return true;
}

