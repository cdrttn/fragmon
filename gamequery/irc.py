import fragmon.irc as fi
import types
import fnmatch

#CTCP utils
CTM = '\x01'

def is_ctcp_req(text):
    """ is this a CTCP request? """
    return text[0] == CTM and text[-1] == CTM

def mk_ctcp_req(cmd, *args):
    """ same as above, but tuple args """
    return mk_ctcp_req2(cmd, args)

def mk_ctcp_req2(cmd, args):
    #FIXME: do quoting
    """ create CTCP request """
    return CTM + cmd + ' ' + ' '.join(args) + CTM

def parse_ctcp_req(text, preserve=False):
    """ parse ctcp request """
    #FIXME: do unquoting
    text = text[1:-1]
    if preserve:
        req = text.split(' ', 1)
    else:
        req = text.split(' ')

    return req


class Mask:
    def __init__(self, prefix):
        self.prefix = prefix
        self.nick = prefix
        self.user = ''
        self.host = ''
        
        pos1 = prefix.find('!')
        pos2 = prefix.find('@')
        if pos1 >= 0 and pos2 >= 0 and pos2 > pos1:
            self.nick = prefix[:pos1]
            self.user = prefix[pos1+1:pos2]
            self.host = prefix[pos2+1]

#basic commands

def msg(target, msg):
    fi.quote('privmsg', target, msg)

def notice(target, msg):
    fi.quote('notice', target, msg)

def ctcp(target, cmd, *args):
    ctcp2(target, cmd, args)

def ctcp2(target, cmd, args):
    cmd = mk_ctcp_req2(cmd, args)
    fi.quote('notice', target, cmd)

def nick(n):
    fi.quote('nick', n)

def join(channel, passwd=""):
    fi.quote('join', channel, passwd)

def part(channel):
    fi.quote('part', channel)

def mode(channel, m, *args):
    fi.quote('mode', m, *args)

def topic(channel, top=""):
    fi.quote('topic', channel, top)

def invite(nick, chan):
    fi.quote('invite', nick, chan)

def kick(chan, nick, comment=""):
    fi.quote('kick', chan, nick, comment)

def away(text=""):
    fi.quote('away', text)

def get_nick():
    return fi.get_nick()

def get_mask():
    return Mask(fi.get_mask())

def get_channels():
    return fi.get_channels()

#Callbacks

class BaseCall(object):
    """wrap a callable"""
    def __init__(self, location, match, cb):
        self.location = location.lower()
        self.match = match.lower()
        self.cb = cb
        if hasattr(cb, 'im_self'):
            self.im_self = cb.im_self

    def __eq__(self, other):
        return (self.cb == other)


class TextCall(BaseCall):
    """ handle text matching """
    def __init__(self, *args, **kwargs):
        BaseCall.__init__(self, *args, **kwargs)

    def __call__(self, err, prefix, cmd, args):
        location = args[0].lower()
        text = args[1].lower()

        if fnmatch.fnmatch(location, self.location) and fnmatch.fnmatch(text, self.match):
            self.cb(prefix, args[0], args[1])


class CtcpCall(BaseCall):
    """ handle CTCP user request matching """
    def __init__(self, *args, **kwargs):
        BaseCall.__init__(self, *args, **kwargs)
        self.nested = False

    def __call__(self, err, prefix, cmd, args):
        location = args[0].lower()
        text = args[1]

        if fnmatch.fnmatch(location, self.location) and is_ctcp_req(text):
            req = parse_ctcp_req(text, preserve=self.nested)
            reqcmd = req[0].lower()
            if fnmatch.fnmatch(reqcmd, self.match):
                reqargs = req[1:]

                if self.nested:
                    #this callback is called similarly to normal callbacks to allow nesting with TextCall
                    reqargs.insert(0, args[0])
                    self.cb(err, prefix, reqcmd, reqargs)
                else:
                    self.cb(prefix, args[0], reqcmd, reqargs)


class CBDict(dict):
    def __init__(self):
        dict.__init__(self)

    def add(self, key, cb):
        key = key.lower()
        if self.has_key(key):
            if cb in self[key]:
                raise IndexError, 'callback already installed'
            self[key].append(cb)
        else:
            self[key] = [cb]

    def remove(self, item):
        """remove all callbacks refering to item (including instance methods of item)"""
        for k in self:
            for i in self[k]:
                if i == item or hasattr(i, 'im_self') and i.im_self == item:
                    self[k].remove(i)
                    
    def __call__(self, err, prefix, cmd, args):
        lcmd = cmd.lower()
        if self.has_key(lcmd):
            for cb in self[lcmd]:
                cb(err, Mask(prefix), cmd, args)


cmd_callbacks = CBDict()
try:
    fi.add_hook(cmd_callbacks)
except IndexError:
    pass


def remove_all(cb):
    cmd_callbacks.remove(cb)

def add_cmd_hook(cmd, cb):
    """ Add a callback for an IRC command """
    cmd_callbacks.add(cmd, cb)

def on_text(location, match, cb):
    cmd_callbacks.add('privmsg', TextCall(location, match, cb))

def on_notice(location, match, cb):
    cmd_callbacks.add('notice', TextCall(location, match, cb))

def on_ctcp(location, cmdmatch, cb):
    caller = CtcpCall(location, cmdmatch, cb)
    cmd_callbacks.add('privmsg', caller)
    cmd_callbacks.add('notice', caller)

def on_action(location, match, cb):
    caller = CtcpCall(location, 'action', TextCall('*', match, cb))
    caller.nested = True
    cmd_callbacks.add('privmsg', caller)
    cmd_callbacks.add('notice', caller)


