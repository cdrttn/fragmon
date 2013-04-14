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



#ifndef MYLOCK_H
#define MYLOCK_H
#include "wx/wx.h"
#include "wx/dc.h"
#include "wx/image.h"
#include <wx/dcclient.h>
#include <wx/dcmemory.h>

DECLARE_EVENT_TYPE(wxEVT_GOTPASS, -1)
#define EVT_GOTPASS(id, fn) EVT_COMMAND(id, wxEVT_GOTPASS, fn)

class PassLock: public wxWindow
{
public:
    PassLock(wxWindow* parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
            long style = 0 , const wxString& name = "lock");

    void OnPaint(wxPaintEvent &evt);
    void OnMouseOver(wxMouseEvent &evt);
    void OnMouseOut(wxMouseEvent &evt);
    void OnClick(wxMouseEvent &evt);

    wxString GetPassword() const { return m_password; }
    void SetPassword(const wxString &pw);

protected:
    wxBitmap m_locked;
    wxBitmap m_unlocked;
    bool m_drawover;
    bool m_draw_unlocked;
    wxString m_password;

    virtual wxSize DoGetBestSize() const;

private:
    DECLARE_EVENT_TABLE()
};

#endif
