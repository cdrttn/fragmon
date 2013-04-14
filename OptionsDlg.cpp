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
#include "OptionsDlg.h"
#include "CopyFmt.h"
#include "ExecInput.h"
#include "QueryOpts.h"
#include "BroadcastFmt.h"

BEGIN_EVENT_TABLE(OptionsDlg, wxDialog)
    EVT_BUTTON(wxID_OK, OptionsDlg::OnButtonHandler)
    EVT_BUTTON(wxID_APPLY, OptionsDlg::OnButtonHandler)
    EVT_BUTTON(wxID_CLOSE, OptionsDlg::OnButtonHandler)
    //EVT_NOTEBOOK_PAGE_CHANGED(-1, OptionsDlg::OnPage)
END_EVENT_TABLE()

OptionsDlg::OptionsDlg(wxWindow* parent, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style, const wxString& name) 
        : wxDialog(parent, id, title, pos, size, style, name)
{

    wxBoxSizer *box = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *buttons = new wxBoxSizer(wxHORIZONTAL);

    m_pages = new wxNotebook(this, -1);
    
    m_pages->AddPage(new QueryOpts(m_pages, -1), "Query Options");
    m_pages->AddPage(new ExecInput(m_pages, -1), "Game Paths");
    m_pages->AddPage(new CopyFmt(m_pages, -1), "Copy Format");
    m_pages->AddPage(new BroadcastFmt(m_pages, -1), "Broadcast Format");

    //m_current = m_pages->GetCurrentPage();

    
    buttons->Add(new wxButton(this, wxID_APPLY));
    buttons->AddSpacer(10);
    buttons->Add(new wxButton(this, wxID_OK));
    buttons->AddSpacer(10);
    buttons->Add(new wxButton(this, wxID_CLOSE));
    
    box->Add(m_pages, 1, wxEXPAND);
    box->AddSpacer(15);
    box->Add(buttons, 0, wxALIGN_CENTER);

    SetSizerAndFit(box);
}

void OptionsDlg::OnButtonHandler(wxCommandEvent &evt)
{
    evt.StopPropagation();

    if (evt.GetId() == wxID_OK) //apply on all windows
    {
        evt.SetId(wxID_APPLY);
        for (int i = 0; i < m_pages->GetPageCount(); i++)
            m_pages->GetPage(i)->ProcessEvent(evt);

        EndModal(wxID_OK);
    }
    else if (evt.GetId() == wxID_CLOSE)
        EndModal(wxID_CLOSE);
    else
        m_pages->GetCurrentPage()->ProcessEvent(evt); 
}

/*void OptionsDlg::OnPage(wxNotebookEvent &evt)
{
    m_current = m_pages->GetCurrentPage();
}*/


