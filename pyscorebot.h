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



#ifndef _PYSCOREBOBOT_H_
#define _PYSCOREBOBOT_H_

//Note:
//Methods are renamed to use an underscore convention: BroadcastPlayers in python is broadcast_players.
//Also, basic members such as NameA, etc, are entered into the getsetters table using the same
//convention

#include "scorebot.h"
#include "pyemb.h"

class PyScoreBot;

class PyScoreBot : public ScoreBot
{
public:
    PyScoreBot(wxEvtHandler *alert = NULL, int aid = -1, wxEvtHandler *broad = NULL, 
            int bid = -1, const wxString &server = "", int port = 0, const wxString &game = "")
        :ScoreBot(alert, aid, broad, bid, server, port, game), m_self(NULL)
    {
    }

    //need to free bot
    ~PyScoreBot();

    //Initialize with bot 
    bool Init(BotType *bt);
    void Cleanup();

    virtual void Save();
    virtual bool Reload();

    bool Check(BotType *bt) const { return (bt->klass == m_type.klass); }
    BotType GetType() const { return m_type; }

    //deal with scorebot options
    wxString GetOption(const wxString &key);
    void SetOptions(const RuleList &rules);
    //These return strings on error, emtpy strings on OK
    wxString SetOption(const wxString &key, const wxString &value);
    wxString SetOption(const GameRule &rule) { return SetOption(rule.key, rule.value); }

public:
    //calls python broadcast_*()
    virtual void Broadcast();
    virtual void BroadcastPlayers();

    PyObject *wrap_Start(PyObject *self) { Start(); Py_RETURN_NONE; }
    PyObject *wrap_Stop(PyObject *self) { Stop(); Py_RETURN_NONE; } 
    PyObject *wrap_Pulse(PyObject *self) { Pulse(); Py_RETURN_NONE; }  

    //default __cleanup__ handler
    PyObject *wrap_Cleanup(PyObject *self) { Py_RETURN_NONE; }  

    //Needs to handle the VarMap...
    PyObject *wrap_SubFormat(PyObject *self, PyObject *args, PyObject *kwargs);
   
    //default is to broadcast simply
    PyObject *wrap_Broadcast(PyObject *self) { ScoreBot::Broadcast(); Py_RETURN_NONE; }
    PyObject *wrap_BroadcastPlayers(PyObject *self) { ScoreBot::BroadcastPlayers(); Py_RETURN_NONE; } 
   
    //game data getters
    PyObject *wrap_GetOverview(PyObject *self);
    PyObject *wrap_GetPlayers(PyObject *self);
    PyObject *wrap_GetRules(PyObject *self);

    PyObject *wrap_SetOption(PyObject *self, PyObject *args);
    PyObject *wrap_GetOption(PyObject *self, PyObject *args);

    void SetSelf(PyObject *self) { m_self = self; }
    
protected:
    BotType m_type;
    PyObject *m_self;
};

#endif
