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



#ifndef _IRCMASK_H_
#define _IRCMASK_H_

#include "wx/wx.h"

class IRCMask
{
public:
    IRCMask() {}
    IRCMask(const IRCMask &m): Nick(m.Nick), User(m.User), Host(m.Host) {}
    IRCMask(const wxString &nick, const wxString &user, const wxString &host): Nick(nick), User(user), Host(host) {}
    IRCMask(const wxString &hostmask) 
    {
        Parse(hostmask);
    }

    IRCMask &operator=(const IRCMask &m) 
    { 
        Nick = m.Nick; 
        User = m.User; 
        Host = m.Host;
        return *this;
    }

    IRCMask &operator=(const wxString &str)
    {
        Parse(str);
        return *this;
    }

    bool operator==(const IRCMask &m)
    { 
        return (Nick == m.Nick && User == m.User && Host == m.Host);
    }
    
    operator const wxString() const
    {
        return MakeString();
    }

    void Parse(const wxString &nick, const wxString &user, const wxString &host)
    {
        Nick = nick; 
        User = user; 
        Host = host;
    }
    
    bool Parse(const wxString &hostmask)
    {
        int nick = hostmask.find('!');
        int host = hostmask.find('@');

        if (nick == wxNOT_FOUND || host == wxNOT_FOUND || nick > host)
            return false;

        Nick = hostmask.substr(0, nick);
        User = hostmask.substr(nick+1, host-nick-1);
        Host = hostmask.substr(host+1);

        return true;
    }

    wxString MakeString() const
    {
        return wxString::Format("%s!%s@%s", Nick.c_str(), User.c_str(), Host.c_str());
    }

    wxString Nick;
    wxString User;
    wxString Host;
};

inline bool operator==(const wxString &s, const IRCMask &m)
    { return (m.MakeString() == s); }
inline bool operator!=(const wxString &s, const IRCMask &m)
    { return (m.MakeString() != s); }
inline bool operator==(const IRCMask &m, const wxString &s)
    { return (m.MakeString() == s); }
inline bool operator!=(const IRCMask &m, const wxString &s) 
    { return (m.MakeString() != s); }

#endif
