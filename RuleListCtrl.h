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



#ifndef RULELIST_H
#define RULELIST_H 
#include "wx/wx.h"
#include "flipper.h"
#include "pyemb.h"


class RuleListCtrl : public FlipList
{
public:
    RuleListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = wxLC_ICON, 
            const wxValidator& validator = wxDefaultValidator, const wxString& name = wxListCtrlNameStr);
    ~RuleListCtrl(); 

    void GetRules(RuleList &rl) { rl = *m_rules; } 

    //void InsertKeyVal(long id, const wxString &key, const wxString &val);

    void OnGameQuery(PyEvent &evt);
    virtual void OnSort(int col, bool asc);
    virtual bool CanSort(int col, bool asc) { return (m_rules != NULL)? true : false; }
    virtual wxString OnGetItemText(long item, long col) const;
    virtual int OnGetItemImage(long item) const;

    
protected:
    RuleList *m_rules;

private:
    DECLARE_EVENT_TABLE()
};

#endif
