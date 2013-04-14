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



#ifndef PLAYERLIST_H
#define PLAYERLIST_H 
#include "wx/wx.h"
#include "pyemb.h"
#include "flipper.h"
#include "CompOutput.h"
#include "wx/hashmap.h"

//the overrides hashtable stores a server:port key, and a PlayerList value.
//Overrides are inserted by the user. The id of players in the overrides
//list is compared to the id of players from each fresh query for the server
WX_DECLARE_STRING_HASH_MAP(PlayerList *, Overrides);

class PlayerListCtrl : public FlipList
{
public:
    PlayerListCtrl(wxWindow* parent, wxWindowID id, CompOutput *coutput, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = wxLC_ICON, 
            const wxValidator& validator = wxDefaultValidator, const wxString& name = wxListCtrlNameStr);

    ~PlayerListCtrl();

    void GetPlayers(PlayerList &pl) { pl = *m_players; }

    void SetGroupTeams(bool group) { m_groupteams = group; }
    bool GetGroupTeams() const { return m_groupteams; }
    int RunSort(GamePlayer *a, GamePlayer *b);
    void OnGameQuery(PyEvent &evt);
    void OnRClick(wxMouseEvent &evt);
    void OnMakeA(wxCommandEvent &evt);
    void OnMakeB(wxCommandEvent &evt);
    void OnMakeF(wxCommandEvent &evt); 
    void OnClear(wxCommandEvent &evt); 
    void OnKeyDown(wxListEvent &evt);
    
    virtual void OnSort(int col, bool asc);
    virtual bool CanSort(int col, bool asc) { return (m_players != NULL)? true : false; }
    virtual wxString OnGetItemText(long item, long col) const;
    virtual int OnGetItemImage(long item) const;
    virtual wxListItemAttr * OnGetItemAttr(long item) const;

    void AddOverride(const GamePlayer &gp);
    void MergeOverrides();
    void SetSelTeam(int teamid);
    void UpdateDuel();

protected:
    PlayerList *m_players;
    wxColour m_colora;
    wxColour m_colorb;
    int m_imglen;
    bool m_groupteams;
    CompOutput *m_coutput;
    //long m_score_cmp;
    int m_favor;

    //Override related
    wxMenu m_menu;
    Overrides m_overrides;
    GameInfo m_info;

private:
    DECLARE_EVENT_TABLE()
};

#endif
