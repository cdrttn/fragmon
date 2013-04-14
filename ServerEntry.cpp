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
#include "ServerEntry.h"

//bleh?
static char exclude_chars[] = "; \t\n~!@#$%^&*()_+\\/'`\"<>,?{}\0";

ServerEntry::ServerEntry(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint& pos, const wxSize& size,
            long style, const wxValidator& validator, const wxString& name) 
            :wxTextCtrl(parent, id, value, pos, size, style, wxDefaultValidator, name)
{
    wxTextValidator val(wxFILTER_EXCLUDE_CHAR_LIST, &m_filtered);
    wxArrayString excl;
    char *p = exclude_chars;
    
    while (*p)
        excl.Add(*p++);

    val.SetExcludes(excl);
    SetValidator(val);

    InitDialog();
}

bool ServerEntry::ParseInput(wxString &host, int &port)
{
    wxString input = GetValue();
    //input.Trim(true);
    //input.Trim(false);

    if (input.Len() == 0)
    {
        return false;
    }

    int p = input.Find(':');
    long iport;
    
    if (p >= 0)
    {
        wxString sport = input.Mid(p+1);
        host = input.Left(p);
        if (!sport.ToLong(&iport))
        {
            return false;
        }
        
        port = (int)iport;
    }
    else
        host = input;

    return true;
}

