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



#include "ircbase.h"
#include "ircresp.h"

DEFINE_EVENT_TYPE(wxEVT_IRC)
IMPLEMENT_DYNAMIC_CLASS(IRCEvent,wxEvent)

BEGIN_EVENT_TABLE(IRCBase, wxEvtHandler)
    EVT_IRC(-1, IRCBase::OnUserMsgM)
END_EVENT_TABLE()

void IRCBase::OnUserMsgM(IRCEvent &evt)
{
    OnUserMsg(evt);
}

void IRCBase::JoinOk(const wxString &chan)
{
    IRCEvent ie;
    ie << RPL_JOINOK << chan;
    wxPostEvent(m_target, ie);
}
