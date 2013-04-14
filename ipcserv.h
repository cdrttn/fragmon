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



#ifndef IPCSERV_H
#define IPCSERV_H
#include "wx/wx.h"
#include "wx/ipc.h"

DECLARE_EVENT_TYPE(wxEVT_IPC_PING, -1)
#define EVT_IPC_PING(fn) EVT_COMMAND(-1, wxEVT_IPC_PING, fn)

class ServConnection : public wxConnection
{
public:
    ServConnection(wxEvtHandler *target): wxConnection(), m_target(target) {}

private:
    wxEvtHandler *m_target;
    virtual bool OnExecute(const wxString& topic, char* data, int size, wxIPCFormat format);
};

class ServServer : public wxServer
{
public:
    ServServer(wxEvtHandler *target = NULL): wxServer(), m_target(target) {}
    void SetTarget(wxEvtHandler *target) { m_target = target; }
    static wxString GetService();

private:
    wxEvtHandler *m_target;
    virtual wxConnectionBase * OnAcceptConnection(const wxString& topic);
};

#endif
