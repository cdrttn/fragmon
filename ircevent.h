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



#ifndef _IRCEVET_H_
#define _IRCEVET_H_
#include "wx/wx.h"

DECLARE_EVENT_TYPE(wxEVT_IRC, 7777)

#define EVT_IRC(id, fn) \
        DECLARE_EVENT_TABLE_ENTRY(wxEVT_IRC, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(IRCEventFunction) &fn, NULL),

class IRCEvent : public wxEvent
{
public:
    IRCEvent(int winid = -1):wxEvent(winid, wxEVT_IRC), IsError(false) {ResumePropagation(wxEVENT_PROPAGATE_MAX);}
    IRCEvent(const IRCEvent &ime)
        :wxEvent(ime),IsError(ime.IsError),
        Source(ime.Source),Command(ime.Command),
        Args(ime.Args) {} 
    virtual wxEvent* Clone() const {return new IRCEvent(*this);}  
    wxString GetErrText() const 
    { 
        return Args.GetCount() > 1? GetString(1) : GetString(0);
    }

    bool IsError;
    wxString Source;
    wxString Command;
    wxArrayString Args;

    void Clear()
    {
        IsError = false;
        Source.Empty();
        Command.Empty();
        Args.Empty();
    }
   
    wxString GetString(int start = 0) const
    {
        wxString ret;
        for (int i = start; i < Args.GetCount(); i++)
            ret << Args[i] << " ";

        ret.Trim();
        
        return ret;
    }
    
    //to support this: evt << cmd <<arg1 << arg2 << arg3
    void Set(const wxString &arg)
    {
        if (Command.empty())
            Command = arg;
        else
            Args.Add(arg); 
    }

    IRCEvent &operator<<(const wxString &arg) { Set(arg); return *this; }
    IRCEvent &operator<<(const long arg) { Set(wxString::Format("%ld", arg)); return *this; }
    IRCEvent &operator<<(const int arg) { Set(wxString::Format("%d", arg)); return *this; }
    IRCEvent &operator<<(const double arg) { Set(wxString::Format("%.2f", arg)); return *this; }
    
private:
    DECLARE_DYNAMIC_CLASS(IRCEvent)
};

typedef void (wxEvtHandler::*IRCEventFunction)(IRCEvent&);

#endif
