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



#ifndef FLIPPER_H
#define FLIPPER_H
#include "wx/listctrl.h"

//cols lists the column headers. images is the list of images for the list.
//The first images must be the up and down arrows. After that can be anything.
class FlipList : public wxListCtrl
{
public:
    FlipList(wxWindow* parent, wxWindowID id, 
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, 
            const wxValidator& validator = wxDefaultValidator, const wxString& name = wxListCtrlNameStr)
            :wxListCtrl(parent, id, pos, size, style | wxLC_REPORT | wxLC_VIRTUAL, validator, name),
            m_sortcol(-1), m_sort(false)
    {
    }

    void ColSetup(const wxString &prefix, const wxArrayString &names, wxImageList *images)
    {
        m_prefix = prefix;
        ColConfReader(names);    
        SetImageList(images, wxIMAGE_LIST_SMALL);
    }

    virtual ~FlipList()
    {
        ColConfWriter();
    }

protected:
    virtual void OnColClick(wxListEvent &evt);
    virtual void OnSort(int col, bool asc) = 0;
    virtual bool CanSort(int col, bool asc) = 0;

    //append = append/trim existing items. rest remain in order
    void NewData(long newcount);
    //void RefreshData(long newcount, bool append = false);
    void Sort();

    void ArrowFlipper(int col);
    void ArrowReset();
    void ColConfWriter();
    void ColConfReader(const wxArrayString &names);


protected:    
    wxString m_prefix;
    //long m_count;
    int m_sortcol;
    bool m_sort;

private:
    DECLARE_EVENT_TABLE();
};

void ArrowFlipper(wxListCtrl *ls, int col, int &curcol, bool &sort);
void ArrowReset(wxListCtrl *ls, int &curcol, bool &sort);
void ColConfWriter(wxListCtrl *ls, const wxString &prefix);
void ColConfReader(wxListCtrl *ls, const wxString &prefix, const wxArrayString &names);

#endif

