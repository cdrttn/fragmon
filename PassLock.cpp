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
#include "wx/dc.h"
#include "wx/image.h"
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/brush.h>
#include "PassLock.h"
#include "fragmon_wdr.h"

DEFINE_EVENT_TYPE(wxEVT_GOTPASS)

BEGIN_EVENT_TABLE(PassLock, wxWindow)
    EVT_PAINT(PassLock::OnPaint)
    EVT_LEFT_DOWN(PassLock::OnClick)
    EVT_ENTER_WINDOW(PassLock::OnMouseOver)
    EVT_LEAVE_WINDOW(PassLock::OnMouseOut)
END_EVENT_TABLE()

PassLock::PassLock(wxWindow* parent, wxWindowID id, 
        const wxPoint& pos, const wxSize& size, long style, const wxString& name)
        : wxWindow(parent, id, pos, size, style, name), m_drawover(false), m_draw_unlocked(false)
{
    m_locked = LocksFunc(ID_LOCK_ON);
    m_unlocked = LocksFunc(ID_LOCK_OFF);
    
    SetSizeHints(m_locked.GetWidth(), m_locked.GetHeight());
    SetToolTip("Set password");
}

void PassLock::OnPaint(wxPaintEvent &evt)
{
    wxPaintDC cdc(this);    
    wxRect pos = GetRect();

    //cdc.Blit(pos.x, pos.y, bmw, bmh, &memdc, 0, 0, wxCOPY, true);
    //cdc.DrawBitmap(m_bm, 0, 0, true);

    if (m_drawover)
        cdc.SetLogicalFunction(wxCLEAR);
    else
        cdc.SetLogicalFunction(wxCOPY);

    if (m_draw_unlocked)
        cdc.DrawBitmap(m_unlocked, 0, 0, true);
    else
        cdc.DrawBitmap(m_locked, 0, 0, true);
}

wxSize PassLock::DoGetBestSize() const
{
    return wxSize(m_locked.GetWidth(), m_locked.GetHeight());
}

void PassLock::OnClick(wxMouseEvent &evt)
{

    wxPasswordEntryDialog pw(this, "Enter Server Password");
    if (pw.ShowModal() == wxID_OK)
        SetPassword(pw.GetValue());

    evt.Skip();
}

void PassLock::SetPassword(const wxString &pw)
{
    m_password = pw;
    if (m_password.Len())
        m_draw_unlocked = true;
    else
        m_draw_unlocked = false;

    Refresh();
    
    if (GetParent() != NULL)
    {
        wxCommandEvent pwevt(wxEVT_GOTPASS, GetId());
        pwevt.SetString(m_password);
        wxPostEvent(GetParent(), pwevt);
    }
}

void PassLock::OnMouseOver(wxMouseEvent &evt)
{
    m_drawover = true;
    Refresh();
}

void PassLock::OnMouseOut(wxMouseEvent &evt)
{
    m_drawover = false;
    Refresh();
}

