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
#include "wx/config.h"
#include "RuleListCtrl.h"
#include "pyemb.h"
#include "flipper.h"
#include "Fragmon.h"

static int SortKeyA(GameRule **a, GameRule **b)
{
    return (*a)->key.Cmp((*b)->key);
}
static int SortKeyB(GameRule **a, GameRule **b)
{
    return (*b)->key.Cmp((*a)->key);
}
static int SortValA(GameRule **a, GameRule **b)
{
    return (*a)->value.Cmp((*b)->value);
}
static int SortValB(GameRule **a, GameRule **b)
{
    return (*b)->value.Cmp((*a)->value);
}


BEGIN_EVENT_TABLE(RuleListCtrl, FlipList)
    EVT_GAMEQUERY(-1, RuleListCtrl::OnGameQuery)
END_EVENT_TABLE()

RuleListCtrl::RuleListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
            const wxSize& size, long style, const wxValidator& validator, const wxString& name)
        :FlipList(parent, id, pos, size, 0, validator, name), 
        m_rules(NULL)
{
    wxArrayString names;
    names.Add("Key");
    names.Add("Value");
    ColSetup("Rule", names, wxGetApp().GetArrows());
}

RuleListCtrl::~RuleListCtrl()
{
   if (m_rules) delete m_rules; 
}

void RuleListCtrl::OnGameQuery(PyEvent &evt)
{
    PyPrintDebug(false, "Rules list got query\n");

    GameData *gdata = evt.GetGdata();
    wxASSERT(gdata != NULL);

    if (m_rules)
        delete m_rules;
    
    m_rules = gdata->rules;
    gdata->rules = NULL;
    wxASSERT(m_rules != NULL);
    NewData(m_rules->GetCount());
}

void RuleListCtrl::OnSort(int col, bool asc) 
{
    if (m_sortcol == 0)
        if (m_sort)
            m_rules->Sort(SortKeyA);
        else
            m_rules->Sort(SortKeyB);
    else
        if (m_sort)
            m_rules->Sort(SortValA);
        else
            m_rules->Sort(SortValB);
}

wxString RuleListCtrl::OnGetItemText(long item, long col) const
{
    if (col == 0)
        return m_rules->Item(item).key;
    else if (col == 1)
        return m_rules->Item(item).value;
}

int RuleListCtrl::OnGetItemImage(long item) const
{
    return -1;
}
