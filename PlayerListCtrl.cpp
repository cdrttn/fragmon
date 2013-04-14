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
#include "PlayerListCtrl.h"
#include "pyemb.h"
#include "flipper.h"
#include "Fragmon.h"
#define INTCMP(a,b) (a-b)

//rightclick menu 
enum
{
    ID_PRC_MAKEA=20000,
    ID_PRC_MAKEB,
    ID_PRC_MAKEF,
    ID_PRC_CLEAR
};

BEGIN_EVENT_TABLE(PlayerListCtrl, FlipList)
    EVT_GAMEQUERY(-1, PlayerListCtrl::OnGameQuery)
    EVT_RIGHT_DOWN(PlayerListCtrl::OnRClick)
    EVT_MENU(ID_PRC_MAKEA, PlayerListCtrl::OnMakeA)
    EVT_MENU(ID_PRC_MAKEB, PlayerListCtrl::OnMakeB)
    EVT_MENU(ID_PRC_MAKEF, PlayerListCtrl::OnMakeF)
    EVT_MENU(ID_PRC_CLEAR, PlayerListCtrl::OnClear)
    EVT_LIST_KEY_DOWN(-1, PlayerListCtrl::OnKeyDown)
END_EVENT_TABLE()

//yea sux
static PlayerListCtrl *sort_object = NULL;
static int SortProxy(GamePlayer **a, GamePlayer **b)
{
    return sort_object->RunSort(*a, *b);
}

int PlayerListCtrl::RunSort(GamePlayer *a, GamePlayer *b)
{
    int ret = 0;

    switch (m_sortcol)
    {
    case 0:
        if (m_sort) ret = a->name.Cmp(b->name);
        else ret = b->name.Cmp(a->name);
        break;
    case 1:
        if (m_sort) ret = INTCMP(a->score, b->score);
        else ret = INTCMP(b->score, a->score);
        break;
    case 2:
        if (m_sort) ret = INTCMP(a->ping, b->ping);
        else ret = INTCMP(b->ping, a->ping);
        break;
    case 3:
        if (m_sort) ret = a->team.Cmp(b->team);
        else ret = b->team.Cmp(a->team);
        break;
    } 

    //atid = 3;
    //atid = 3;
    // -1 a < b
    // 0 a == b
    // 1 a > b
    if (m_groupteams && a->teamid != b->teamid)
    {
        //push specs, etc down, bring participants up
        if (a->teamid > 1 || b->teamid > 1 || m_favor < 0)
            ret = INTCMP(a->teamid, b->teamid);
        else 
        {
            if (a->teamid == m_favor)
                ret = -1;
            else
                ret = 1;
        }
    }

    return ret;
}

PlayerListCtrl::PlayerListCtrl(wxWindow* parent, wxWindowID id, CompOutput *coutput, const wxPoint& pos, 
            const wxSize& size, long style, const wxValidator& validator, const wxString& name)
        :FlipList(parent, id, pos, size, 0, validator, name), 
        m_players(NULL), m_groupteams(true), 
        m_colora(*wxWHITE), m_colorb(*wxWHITE),
        m_coutput(coutput) 
{
    wxArrayString names;
    names.Add("Name");
    names.Add("Score");
    names.Add("Ping");
    names.Add("Team");
    wxImageList *imgs = wxGetApp().GetArrows();
    m_imglen = imgs->GetImageCount();
    ColSetup("Player", names, imgs);

    //rclick menu
    m_menu.Append(ID_PRC_MAKEA, "Set Team &A", "Set selected players to join Team A");
    m_menu.Append(ID_PRC_MAKEB, "Set Team &B", "Set selected players to join Team B");
    m_menu.Append(ID_PRC_MAKEF, "Set Team &F", "Set selected players to join Team F (team free/spectators)");
    m_menu.AppendSeparator();
    m_menu.Append(ID_PRC_CLEAR, "&Clear Team Overrides", "Clear your team selections for this server");
}

PlayerListCtrl::~PlayerListCtrl() 
{ 
    if (m_players) delete m_players; 

    Overrides::iterator iter = m_overrides.begin();
    while (iter != m_overrides.end())
    {
        delete iter->second;
        iter++;
    }
}


void PlayerListCtrl::OnGameQuery(PyEvent &evt)
{
    PyPrintDebug(false, "players list got query\n");

    m_colora = *wxWHITE;
    m_colorb = *wxWHITE;

    GameData *gdata = evt.GetGdata();
    wxASSERT(gdata != NULL);

    if (m_players)
        delete m_players;

    //Get control over player list
    m_players = gdata->players;
    gdata->players = NULL;
 
    //Make a copy of the overview info
    m_info = *gdata->info;
   
    MergeOverrides();
    UpdateDuel();

    //Peak at the teamdesc
    if (m_info.mode == "team")
    {
        m_colora = m_info.teamAcolor;
        m_colorb = m_info.teamBcolor;
    }
    wxASSERT (m_players != NULL);
    NewData(m_players->GetCount());
}

void PlayerListCtrl::UpdateDuel()
{
    if (!m_players)
        return;

    if (m_info.mode == "1v1") //update competition output
    {
        bool haveA = false;
        bool haveB = false;
        for (int i = 0; i < m_players->GetCount(); i++)
        {
            GamePlayer &gp = m_players->Item(i);
            if (gp.teamid == 0 && !haveA)
            {
                haveA = true;
                m_coutput->SetNameA(gp.name);
                m_coutput->SetScoreA(gp.score);
            }
            else if (gp.teamid == 1 && !haveB)
            {
                haveB = true;
                m_coutput->SetNameB(gp.name);
                m_coutput->SetScoreB(gp.score);
            }
        }
    }
}

void PlayerListCtrl::OnRClick(wxMouseEvent &evt)
{
    PopupMenu(&m_menu);
}

void PlayerListCtrl::AddOverride(const GamePlayer &gp)
{
    PlayerList *pl;
    wxString key = wxString::Format("%s%d", m_info.server.c_str(), m_info.port);
    Overrides::iterator iter = m_overrides.find(key);

    if (iter == m_overrides.end())
    {
        pl = new PlayerList;
        m_overrides[key] = pl;
    }
    else
        pl = iter->second;

    PlayerList &plist = *pl;

    //Alter old copy
    for (int i = 0; i < plist.GetCount(); i++)
    {
        if (plist[i].id == gp.id)
        {
            plist[i] = gp;
            return;
        }
    }

    //Add a copy of gp
    plist.Add(gp);
}

//Run this after each new query 
void PlayerListCtrl::MergeOverrides()
{
    wxASSERT(m_players);

    //player list overrides, player list fresh
    wxString key = wxString::Format("%s%d", m_info.server.c_str(), m_info.port);
    Overrides::iterator iter = m_overrides.find(key);

    //Return if there is no override data for this server
    if (iter == m_overrides.end())
        return;

    PlayerList &plo = *iter->second;
    PlayerList &plf = *m_players;

    //Merge in overrides
    for (int i = 0; i < plf.GetCount(); i++)
    {
        for (int j = 0; j < plo.GetCount(); j++)
        {
            if (plf[i].id == plo[j].id)
            {
                plf[i].teamid = plo[j].teamid;
                break;
            }
        }
    }
}

void PlayerListCtrl::SetSelTeam(int teamid)
{
    if (!m_players)
        return;

    long item = -1;
    while ((item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != -1)
    {
        GamePlayer &gp = m_players->Item(item);
        printf("player %s -> team %d\n", gp.name.c_str(), teamid);

        gp.teamid = teamid;
        AddOverride(gp);
    }

    UpdateDuel();
    NewData(m_players->GetCount());
}

void PlayerListCtrl::OnMakeA(wxCommandEvent &evt)
{
    SetSelTeam(0);
}

void PlayerListCtrl::OnMakeB(wxCommandEvent &evt)
{
    SetSelTeam(1);
}

void PlayerListCtrl::OnMakeF(wxCommandEvent &evt)
{
    SetSelTeam(2);
}

void PlayerListCtrl::OnClear(wxCommandEvent &evt) 
{
    wxString key = wxString::Format("%s%d", m_info.server.c_str(), m_info.port);
    Overrides::iterator iter = m_overrides.find(key);

    if (iter == m_overrides.end())
        return;

    delete iter->second;

    m_overrides.erase(iter);
}

void PlayerListCtrl::OnKeyDown(wxListEvent &evt)
{
    //printf("key down %c\n", evt.GetKeyCode());
    switch (evt.GetKeyCode())
    {
        case 'a':
        case 'A':
            SetSelTeam(0);
            break;
        case 'b':
        case 'B':
            SetSelTeam(1);
            break;
        case 'f':
        case 'F':
            SetSelTeam(2);
            break;
    }
}

void PlayerListCtrl::OnSort(int col, bool asc)
{
    sort_object = this;
    long scorea, scoreb;
    scorea = scoreb = 0;
    m_coutput->GetScoreA().ToLong(&scorea);
    m_coutput->GetScoreB().ToLong(&scoreb);
    //m_score_cmp = INTCMP(scorea, scoreb);
    m_favor = -1;
    if (scorea > scoreb)
        m_favor = 0;
    else if (scoreb > scorea)
        m_favor = 1;

    PyPrintDebug(false, "A(%d), B(%d), favor(%d)\n", scorea, scoreb, m_favor);

    m_players->Sort(SortProxy);
}

wxString PlayerListCtrl::OnGetItemText(long item, long col) const
{
    switch(col)
    {
    case 0: 
        return m_players->Item(item).name;
    case 1:
        return wxString::Format("%d", m_players->Item(item).score);
    case 2:
        return wxString::Format("%d", m_players->Item(item).ping);
    case 3:
        return m_players->Item(item).team;
    }
}

int PlayerListCtrl::OnGetItemImage(long item) const
{
    int img = m_players->Item(item).teamid + 2;
    if (img >= m_imglen)
        img = -1;

    return img;
}

wxListItemAttr *PlayerListCtrl::OnGetItemAttr(long item) const
{
    //XXX don't like having to return pointer
    static wxListItemAttr attr;
    switch (m_players->Item(item).teamid)
    {
    case 0:
        attr.SetBackgroundColour(m_colora);
        break;
    case 1:
        attr.SetBackgroundColour(m_colorb);
        break;
    //case 2:
    default:
        attr.SetBackgroundColour(*wxWHITE);
    }

    return &attr;
}
