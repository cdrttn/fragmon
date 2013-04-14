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



#include "scorebot.h"
#include "irc.h"
#include "CopyFmt.h"
#include "BroadcastFmt.h"

#include "pyemb.h" //dbg

BEGIN_EVENT_TABLE(ScoreBot, wxEvtHandler)
    EVT_GAMEQUERY(-1, ScoreBot::OnGameQueryM)
    EVT_TIMER(-1, ScoreBot::OnTimerPulseM)
END_EVENT_TABLE()

static int sort_players(GamePlayer **aa, GamePlayer **bb)
{
    GamePlayer *a = *aa;
    GamePlayer *b = *bb;

    if (a->teamid == b->teamid)
        return b->score - a->score;
    return a->teamid - b->teamid;
}

//Basic formating
wxString ScoreBot::SubFormat(const wxString &format, VarMap &vmap) 
{
    if (m_last_info)
        CopyFmt::LoadFormat(m_last_info, vmap);
    BroadcastFmt::LoadFormat(*this, vmap);

    return VarSubst(format, vmap);
}

wxString ScoreBot::SubFormat(VarMap &vmap)
{
    return SubFormat(GetFormat(), vmap);
}

wxString ScoreBot::SubFormat()
{
    VarMap vmap;
    return SubFormat(GetFormat(), vmap);
}
    
void ScoreBot::OnTimerPulseM(wxTimerEvent &evt)
{
    if (m_alert)
    {
        evt.SetId(m_alert_id);
        m_alert->ProcessEvent(evt);
    }

    OnTimerPulse(evt);
}

void ScoreBot::OnTimerPulse(wxTimerEvent &evt)
{
    new GameQueryThread(this, -1, m_game, m_host, m_port, GameQueryThread::ALL);
}

void ScoreBot::OnGameQueryM(PyEvent &evt)
{
    if (m_alert)
    {
        evt.SetId(m_alert_id);
        m_alert->ProcessEvent(evt);
    }

    GameData *gdata = evt.GetGdata();

    if (m_last_info) 
        delete m_last_info;

    m_last_info = NULL;
    m_last_players.Clear();
    m_last_rules.Clear();

    if (gdata)
    {
        if (gdata->info)
            m_last_info = new GameInfo(*gdata->info);

        //Some controls may have changed the data, so get fresh copies
        if (gdata->players)
            m_last_players = *gdata->players;
        else if (m_players)
            m_players->GetPlayers(m_last_players);

        if (gdata->rules)
            m_last_rules = *gdata->rules;
        else if (m_rules)
            m_rules->GetRules(m_last_rules);

        m_last_players.Sort(sort_players); 
    }

    OnGameQuery(evt);
}

void ScoreBot::OnGameQuery(PyEvent &evt)
{   
    if (evt.GetGdata())
        Broadcast();
}

//calls virutal methods 
//1. OnTimerPulse should generate at least start a game query 
//2. OnGameQuery should broadcast the query somehow

//default broadcast implementation
void ScoreBot::Broadcast()
{
    if (!m_broadcast)
        return;

    IRCEvent ie(m_broadcast_id);

    wxString out = SubFormat();
    printf("\n%s\n\n", out.c_str());
    ie << "privmsg" << m_irc_chan << out;

    wxPostEvent(m_broadcast, ie);
}


//broadcast latest player info (only makes sense for team)
void ScoreBot::BroadcastPlayers()
{
    if (!m_broadcast || m_last_players.IsEmpty())
        return;


    wxString teama = m_name_a + ": ";
    wxString teamb = m_name_b + ": ";
    int i;
    int count = m_last_players.GetCount();
    for (i = 0; i < count && m_last_players[i].teamid == 0; i++)
        teama << m_last_players[i].name << " " << m_last_players[i].score << "pts, ";

    for (; i < count && m_last_players[i].teamid == 1; i++)
        teamb << m_last_players[i].name << " " << m_last_players[i].score << "pts, ";

    teama.erase(teama.size()-3);
    teamb.erase(teamb.size()-3);
    
    IRCEvent ie;
    ie << "privmsg" << m_irc_chan << teama;
    wxPostEvent(m_broadcast, ie);

    ie.Clear();
    ie << "privmsg" << m_irc_chan << teamb;
    wxPostEvent(m_broadcast, ie);
    
}
