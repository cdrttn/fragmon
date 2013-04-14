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



#ifndef _IRCBASE_H_
#define _IRCBASE_H_
#include "wx/wx.h"
#include "ircevent.h"
#include "ircmask.h"

//Basic Base class for IRC.. 
//Helps to make querying channels and other basic data synchronous
//Send other arbitrary irc commands to the event handler
//
class IRCBase : public wxEvtHandler
{
public:
    IRCBase(wxEvtHandler *tgt, int id = -1)
        :m_target(tgt), m_target_id(id), 
        m_target_save(NULL), m_target_id_save(-1) {}

    virtual ~IRCBase() {}

    virtual bool IsConnected() const = 0;
    virtual wxArrayString GetChanList() const = 0;
    virtual wxString GetNick() const = 0;
    virtual IRCMask GetHostmask() const = 0;
    virtual void Disconnect() = 0;
    
    void SetTempHandler(wxEvtHandler *tgt, int id = -1)
    {
        m_target_save = m_target;
        m_target_id_save = m_target_id;

        m_target = tgt;
        m_target_id = id;
    }
   
    void PopTempHandler()
    {
        wxASSERT(m_target_save);
        m_target = m_target_save;
        m_target_id = m_target_id_save;

        m_target_save = NULL;
        m_target_id_save = -1;
    }
    
protected:
    virtual void OnUserMsg(IRCEvent &evt) = 0;
    void OnUserMsgM(IRCEvent &evt);
    void JoinOk(const wxString &chan); //acknowledge a succesful join

protected:
    wxEvtHandler *m_target;
    wxEvtHandler *m_target_save;
    int m_target_id;
    int m_target_id_save;

private:
    DECLARE_EVENT_TABLE()
};

#endif
