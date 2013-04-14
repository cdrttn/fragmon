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
#include "DebugFrame.h"
#include "pyemb.h"
#include "Fragmon.h"
#include "icon.xpm"

BEGIN_EVENT_TABLE(DebugFrame, wxFrame)
    EVT_PYDEBUG(-1, DebugFrame::OnDebug)
    EVT_CLOSE(DebugFrame::OnClose)
END_EVENT_TABLE()

#define CLEAR_EVTS 25

DebugFrame::DebugFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, 
            const wxSize& size, long style, const wxString& name)
        :wxFrame(parent, id, title, pos, size, style, name)
{
    wxConfigBase *config = wxGetApp().GetConf();
    SetIcon(wxICON(xicon));

    //XXX: should have a setting to limit total log length
    m_output = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_RICH|wxTE_MULTILINE|wxTE_READONLY);
    m_critical = false;

    config->SetPath("/Dimmensions");
    int width, height;
    config->Read("DebugFrameW", &width, 640);
    config->Read("DebugFrameH", &height, 480);
    SetSize(width,height);

    m_len = 0;
    m_lines = CLEAR_EVTS; //check to clear buffer every 25 events
    //XXX don't hardcode
    m_max_len = 25000;

    PyEmbSetDebugTarget(this);
}

DebugFrame::~DebugFrame()
{
    wxConfigBase *config = wxGetApp().GetConf();
    config->SetPath("/Dimmensions");
    int width, height;
    GetSize(&width, &height);
    config->Write("DebugFrameW", width);
    config->Write("DebugFrameH", height);
    
    
    PyEmbSetDebugTarget(NULL);
}


void DebugFrame::OnDebug(wxCommandEvent &evt)
{
    wxString text = evt.GetString();

    m_len += text.Len();
    m_lines -= 1;
    if (m_lines <= 0 && m_len > m_max_len)
    {
        m_lines = CLEAR_EVTS;

        long remlen = m_len - m_max_len;
        //remove top 1/3
        m_output->Remove(0, remlen-1);
        m_len -= remlen;
#ifdef __WXDEBUG__
        printf("max -> %ld, removed %ld -> %ld\n", m_max_len, remlen, m_len);
#endif
    }

    //XXX: should have fg/bg/err colors saved in conf?
    if (evt.GetInt())
    {
        m_output->SetDefaultStyle(wxTextAttr(*wxRED, *wxWHITE));
        m_output->AppendText(text);
        m_output->SetDefaultStyle(wxTextAttr());
    }
    else
        m_output->AppendText(text);
}

void DebugFrame::OnClose(wxCloseEvent &evt)
{
    if (evt.CanVeto() && !m_critical)
    {
        evt.Veto();
        Hide();
    }
    else
    {
        Destroy();
    }
}
