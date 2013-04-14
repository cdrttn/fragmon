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



#ifndef _PYDATACONV_H_
#define _PYDATACONV_H_
#include "wx/wx.h"
#include <Python.h>

class GameRule;
class GamePlayer;
class GameInfo;
class GameData;
WX_DECLARE_OBJARRAY(GameRule, RuleList);
WX_DECLARE_OBJARRAY(GamePlayer, PlayerList);

class GameRule
{
public:
    GameRule(const wxString &k = "", const wxString &v = ""): key(k), value(v) {}
    //~GameRule() {puts("IGOTHERE del gamerule");}
    //TESTING
   
    wxString key;
    wxString value;
};


//FYI team is a text representation of the player's team. It can be anything. It could
//be a clan name, actual team name, or whatever. It is not processed and appears directly
//under the Team column in output. teamid indicates which team the player is playing on,
//0 = teamA, 1 = teamB, 2 = team free. The user will be highlighted as such in output.
class GamePlayer
{
public:
    GamePlayer(const wxString &n = "", const int s = 0, const int p = 0, 
            const wxString &t = "", int tid = 2, int pid = -1)
        : name(n), score(s), ping(p), team(t), teamid(tid), id(pid) {}
    wxString name;
    int score;
    int ping;
    wxString team;
    int teamid;
    int id;
};

class GameInfo
{
public:
    GameInfo()
        : ping(0), port(0), curplayers(0), maxplayers(0), 
        password(false), teamAscore(0), teamBscore(0) {} 
    int port;
    wxString server;
    
    //Overview
    int ping;
    int curplayers;
    int maxplayers;
    bool password; 
    wxString hostname;
    wxString map;
    wxString type; //for user: CTF, tdm, whatever
    wxString mode; //for fragmon: 1v1, team, or "" for other 
    wxString version;
    wxString mod;
    wxString time;

    //Team info
    wxString teamAname;
    wxString teamBname;
    int teamAscore;
    int teamBscore;
    wxColour teamAcolor;
    wxColour teamBcolor;
};

class GameData
{
public:
    GameData(): info(NULL), players(NULL), rules(NULL) {}
    ~GameData()
    {
        if (info) delete info;
        if (players) delete players;
        if (rules) delete rules;
    }

    GameInfo *info;
    PlayerList *players;
    RuleList *rules;

    //implemented in pydataconv
    bool MakeGdata(PyObject *overview, const wxString &host = "", int port = -1, 
        PyObject *players = NULL, PyObject *rules = NULL);
    bool ParseOverview(PyObject *overview, const wxString &host = "", int port = -1);
    bool ParsePlayers(PyObject *players);
    bool ParseRules(PyObject *rules);
};


#endif
