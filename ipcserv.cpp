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
#include "wx/ipc.h"
#include "pyemb.h"
#include "ipcserv.h"

DEFINE_EVENT_TYPE(wxEVT_IPC_PING)


bool ServConnection::OnExecute(const wxString& topic, char* data, int size, wxIPCFormat format) 
{
    PyPrintDebug(false, "IPC Serv: pinging..\n");
    wxString server(data,size);

    wxCommandEvent evt(wxEVT_IPC_PING);
    evt.SetString(server);
    wxPostEvent(m_target, evt);
    
    return true;
}

wxConnectionBase *ServServer::OnAcceptConnection(const wxString& topic) 
{
    if (topic != "PING")
    {
        PyPrintDebug(true, "IPC Serv: got bad topic %s\n", topic.c_str()); 
        return NULL;
    }
    
    return new ServConnection(m_target);
}

wxString ServServer::GetService()
{
    //return service name: either DDE name or domain socket name for unix
    wxString service = "fragmon-" + wxGetUserId();

#if !defined(__WIN32__)
    service.Prepend("/tmp/");
#endif

    return service;
}
