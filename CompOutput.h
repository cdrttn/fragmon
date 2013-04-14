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



/////////////////////////////////////////////////////////////////////////////
// Name:        CompOutput.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __CompOutput_H__
#define __CompOutput_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "CompOutput.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "fragmon_wdr.h"
#include "pyemb.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CompOutput
//----------------------------------------------------------------------------

class CompOutput: public wxPanel
{
public:
    // constructors and destructors
    CompOutput( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
    void AddDirty(unsigned long d) { m_dirty |= d; }
    void ClearDirty(unsigned long d) { m_dirty &= ~d; }
    bool IsDirty(unsigned long d) const { return (m_dirty & d) > 0; }
    void SetDirty(unsigned long d) { m_dirty = d; }
    unsigned long GetDirty() const { return m_dirty; }

    wxString GetTime() { return GetCompTime()->GetValue(); }
    wxString GetNameA() { return GetCompNameA()->GetValue(); }
    wxString GetNameB() { return GetCompNameB()->GetValue(); }
    wxString GetScoreA() { return GetCompScoreA()->GetValue(); } 
    wxString GetScoreB() { return GetCompScoreB()->GetValue(); } 
    
    void SetTime(const wxString &time) { m_setval = true; if (!IsDirty(TIME_DIRTY)) GetCompTime()->SetValue(time); m_setval = false; }
    void SetNameA(const wxString &name) { m_setval = true; if (!IsDirty(NAMEA_DIRTY)) GetCompNameA()->SetValue(name); m_setval = false; }
    void SetNameB(const wxString &name) { m_setval = true; if (!IsDirty(NAMEB_DIRTY)) GetCompNameB()->SetValue(name); m_setval = false; }
    void SetScoreA(const wxString &score) { m_setval = true; if (!IsDirty(SCOREA_DIRTY)) GetCompScoreA()->SetValue(score); m_setval = false; }
    void SetScoreB(const wxString &score) { m_setval = true; if (!IsDirty(SCOREB_DIRTY)) GetCompScoreB()->SetValue(score); m_setval = false; }
    void SetScoreA(int score) { m_setval = true; if (!IsDirty(SCOREA_DIRTY)) GetCompScoreA()->SetValue(wxString::Format("%d", score)); m_setval = false; }
    void SetScoreB(int score) { m_setval = true; if (!IsDirty(SCOREB_DIRTY)) GetCompScoreB()->SetValue(wxString::Format("%d", score)); m_setval = false; }

public:
    enum
    {
        CLEAR_DIRTY = 0,
        TIME_DIRTY = 1<<0,
        SCOREA_DIRTY = 1<<1,
        SCOREB_DIRTY = 1<<2,
        NAMEA_DIRTY = 1<<3,
        NAMEB_DIRTY = 1<<4,
    };
    
private:    
    bool m_setval;
    unsigned long m_dirty;
    // WDR: method declarations for CompOutput
    wxTextCtrl* GetCompTime()  { return (wxTextCtrl*) FindWindow( ID_COMP_TIME ); }
    wxTextCtrl* GetCompScoreB()  { return (wxTextCtrl*) FindWindow( ID_COMP_SCORE_B ); }
    wxTextCtrl* GetCompScoreA()  { return (wxTextCtrl*) FindWindow( ID_COMP_SCORE_A ); }
    wxTextCtrl* GetCompNameB()  { return (wxTextCtrl*) FindWindow( ID_COMP_NAME_B ); }
    wxTextCtrl* GetCompNameA()  { return (wxTextCtrl*) FindWindow( ID_COMP_NAME_A ); }
    
private:
    // WDR: member variable declarations for CompOutput
    
private:
    // WDR: handler declarations for CompOutput
    void OnGameQuery(PyEvent &evt);
    void OnText(wxCommandEvent &evt);

private:
    DECLARE_EVENT_TABLE()
};




#endif
