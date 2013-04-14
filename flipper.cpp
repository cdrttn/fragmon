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
#include "wx/listctrl.h"
#include "flipper.h"
#include "Fragmon.h"

BEGIN_EVENT_TABLE(FlipList, wxListCtrl)
    EVT_LIST_COL_CLICK(-1, FlipList::OnColClick)
END_EVENT_TABLE()

void FlipList::OnColClick(wxListEvent &evt)
{
    int col = evt.GetColumn();
    if (!CanSort(col, !m_sort))
        return;
    ArrowFlipper(col);
    Sort();
}

/*void FlipList::RefreshData(long newcount, bool append)
{
    //SetItemCount(newcount);

    if (!append)
        RefreshItems(0, newcount-1);
    
    //m_count = newcount;
}*/

void FlipList::Sort()
{
    if (m_sortcol >= 0)
        OnSort(m_sortcol, m_sort);
    if (GetItemCount())
        RefreshItems(0, GetItemCount()-1);
}

void FlipList::NewData(long newcount)
{
    SetItemCount(newcount);
    if (!newcount)
        DeleteAllItems();
    Sort();
}

void FlipList::ArrowFlipper(int col)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);

    if (m_sortcol != col && m_sortcol >= 0)
    {
        item.SetImage(-1);
        SetColumn(m_sortcol, item);
        m_sort = false;
    }
   
    m_sort = !m_sort;
    item.SetImage(m_sort ? 1 : 0);
    SetColumn(col, item);

    m_sortcol = col;
}


void FlipList::ArrowReset()
{
    if (m_sortcol < 0)
        return;

    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(-1);
    SetColumn(m_sortcol, item);

    m_sortcol = -1;
    m_sort = false;
}

void FlipList::ColConfWriter()
{
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/Dimmensions");
    wxListItem col;

    for (int i=0; i < GetColumnCount(); i++)
    {
        bool ret = GetColumn(i, col);
        wxASSERT(ret);
        wxString key;
        key.Printf("%s%dColW", m_prefix.c_str(), i);
        config->Write(key, col.GetWidth());
    }
}

void FlipList::ColConfReader(const wxArrayString &names)
{
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/Dimmensions");
    wxListItem col;
    col.SetAlign(wxLIST_FORMAT_LEFT);
    col.SetImage(-1);
    
    for (int i=0; i < names.GetCount(); i++)
    {
        int width;
        wxString key;
        key.Printf("%s%dColW", m_prefix.c_str(), i);
        config->Read(key, &width, 100);     
        col.SetText(names[i]);
        col.SetWidth(width);
        InsertColumn(i, col);
    }
}
/*
///compat
void ArrowFlipper(wxListCtrl *ls, int col, int &curcol, bool &sort)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);

    if (curcol != col && curcol >= 0)
    {
        item.SetImage(-1);
        ls->SetColumn(curcol, item);
        sort = false;
    }
   
    sort = !sort;
    item.SetImage(sort ? 0 : 1);
    ls->SetColumn(col, item);

    curcol = col;
}


void ArrowReset(wxListCtrl *ls, int &curcol, bool &sort)
{
    if (curcol < 0)
        return;

    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(-1);
    ls->SetColumn(curcol, item);

    curcol = -1;
    sort = false;
}

void ColConfWriter(wxListCtrl *ls, const wxString &prefix)
{
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/Dimmensions");
    wxListItem col;

    for (int i=0; i < ls->GetColumnCount(); i++)
    {
        bool ret = ls->GetColumn(i, col);
        wxASSERT(ret);
        wxString key;
        key.Printf("%s%dColW", prefix.c_str(), i);
        config->Write(key, col.GetWidth());
    }
}

void ColConfReader(wxListCtrl *ls, const wxString &prefix, const wxArrayString &names)
{
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/Dimmensions");
    wxListItem col;
    col.SetAlign(wxLIST_FORMAT_LEFT);
    col.SetImage(-1);
    
    for (int i=0; i < names.GetCount(); i++)
    {
        int width;
        wxString key;
        key.Printf("%s%dColW", prefix.c_str(), i);
        config->Read(key, &width, 100);     
        col.SetText(names[i]);
        col.SetWidth(width);
        ls->InsertColumn(i, col);
    }
}
*/
