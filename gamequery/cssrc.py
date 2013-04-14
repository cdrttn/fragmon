"""
Copyright (c) 2006 Christopher Davis

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

$Id$

"""

import fragmon
import query
import socket
import sys
from fragmon import Rule, Player, Overview

GET_CHAL = '\xff\xff\xff\xffW'
GET_SINFO = '\xff\xff\xff\xffTSource Engine Query\x00'
GET_PLAYERS =  '\xff\xff\xff\xffU'
GET_RULES = '\xff\xff\xff\xffV'

class HL1Overview(Overview):
    def __init__(self, game_ip='', game_dir='', dedicated=0, os='', is_mod=0,
                mod_urlinfo='', mod_urldl='', mod_vers=0, mod_size=0, mod_svonly=0, 
                mod_cldll=0, secure=0, numbots=0, *args, **kwargs):
        Overview.__init__(self, *args, **kwargs)
        self.is_mod = is_mod
        self.mod_vers = mod_vers
        self.mod_size = mod_size
        self.mod_svonly = mod_svonly
        self.mod_cldll= mod_cldll
        self.mod_urlinfo = mod_svinfo
        self.mod_urldl = mod_urldl
        self.secure = secure
        self.dedicated = dedicated
        self.numbots = numbots
        self.os = os
        self.game_dir = game_dir
        self.game_ip = game_ip

class HL2Overview(Overview):
    def __init__(self, game_dir='', appid='', numbots=0, 
                dedicated=0, os='', secure=0, *args, **kwargs):
        Overview.__init__(self, *args, **kwargs)
        self.game_dir = game_dir
        self.appid = appid
        self.numbots = numbots
        self.dedicated = dedicated
        self.secure = secure
        self.os = os


#checks buf to see if it is apart of a multi packet response returns (req_id, pcur, ptot, buf) or (None, None, None, buf)
def css_check_multi_packet(buf):
    req_id = pcur = ptot = None
    if buf[0] == '\xfe':
        req_id = query.parse_num(buf, 4, bytes=4)[0]
        pnum = ord(buf[8])
        pcur = pnum >> 4
        ptot = pnum & 0x0f
        buf = buf[9:]
    return req_id, pcur, ptot, buf

#handle a multi packet response from a css server
def css_recv_multi_packet(sock):
    buf = sock.recv(0xffff)
    print 'GOT BUF', repr(buf)
    req_id, pcur, ptot, buf = css_check_multi_packet(buf)
    if req_id == None:
        return buf
    assert pcur == 0, 'why is pcur not 0?'
    ptot -= 1
    bufs = [buf]
    while ptot:
        buf2 = sock.recv(0xffff)
        print '\nGOT BUFext', repr(buf2)
        req_id2, pcur, ptot2, buf2 = css_check_multi_packet(buf2)
        assert req_id == req_id2, 'recvd packet with bad request'
        ptot -= 1
        bufs.append(buf2)
    return ''.join(bufs)

#some servers dont reply to challenge so...
def css_get_challenge(sock):
    oto = sock.gettimeout()
    sock.settimeout(1)
    try:
        sock.send(GET_CHAL)
        chal = css_recv_multi_packet(sock)[5:]
    except socket.timeout:
        print 'timeout waiting for challenge'
        chal = ''
    sock.settimeout(oto)
    return chal

def css_get_info_source(buf, pos):
    info = HL2Overview(teamAname = 'Terrorists', teamBname = 'Counter Terrorists')

    info.hostname,      pos = query.parse_str(buf, pos)
    info.map,           pos = query.parse_str(buf, pos)
    info.game_dir,      pos = query.parse_str(buf, pos)
    info.type,          pos = query.parse_str(buf, pos)
    info.appid,         pos = query.parse_num(buf, pos, bytes=2)
    info.curplayers,    pos = query.parse_num(buf, pos, bytes=1)
    info.maxplayers,    pos = query.parse_num(buf, pos, bytes=1)
    info.numbots,       pos = query.parse_num(buf, pos, bytes=1)
    info.dedicated,     pos = buf[pos], pos + 1 
    info.os,            pos = buf[pos], pos + 1 
    info.password,      pos = query.parse_num(buf, pos, bytes=1)
    info.secure,        pos = query.parse_num(buf, pos, bytes=1)
    info.version,       pos = query.parse_str(buf, pos)

    return info

def css_get_info_hl1(buf, pos):
    info = HL1Overview(teamAname = 'Terrorists', teamBname = 'Counter Terrorists')

    info.game_ip,       pos = query.parse_str(buf, pos)
    info.hostname,      pos = query.parse_str(buf, pos)
    info.map,           pos = query.parse_str(buf, pos)
    info.game_dir,      pos = query.parse_str(buf, pos)
    info.type,          pos = query.parse_str(buf, pos)
    info.curplayers,    pos = query.parse_num(buf, pos, bytes=1)
    info.maxplayers,    pos = query.parse_num(buf, pos, bytes=1)
    version,            pos = query.parse_num(buf, pos, bytes=1)
    info.dedicated,     pos = buf[pos], pos + 1 
    info.os,            pos = buf[pos], pos + 1 
    info.password,      pos = query.parse_num(buf, pos, bytes=1)
    info.is_mod,        pos = query.parse_num(buf, pos, bytes=1)
    info.mod_urlinfo,   pos = query.parse_str(buf, pos)
    info.mod_urldl,     pos = query.parse_str(buf, pos)
    fluff,              pos = query.parse_str(buf, pos)
    info.mod_vers,      pos = query.parse_num(buf, pos, bytes=4)
    info.mod_size,      pos = query.parse_num(buf, pos, bytes=4)
    info.mod_svonly,    pos = query.parse_num(buf, pos, bytes=1)
    info.mod_cldll,     pos = query.parse_num(buf, pos, bytes=1)
    info.secure,        pos = query.parse_num(buf, pos, bytes=1)
    info.numbots,       pos = query.parse_num(buf, pos, bytes=1)

    info.version = str(version)

    return info
        
def css_get_info(sock):
    ms1 = fragmon.get_millis()
    sock.send(GET_SINFO)
    sinfo = css_recv_multi_packet(sock)
    ms2 = fragmon.get_millis()
    ping = ms2 - ms1

    header = sinfo[4]
    #CS:source or old half-life
    if header == 'I':
        cs_source = True
        info = css_get_info_source(sinfo, 6)
    elif header == 'm':
        cs_source = False
        info = css_get_info_hl1(sinfo, 5)
    else:
        assert False, 'Unknown HL packet!'
    
    info.ping = ping
    info.mode = 'team'

    return ping, info

def css_get_players(sock, chal=None):
    if chal == None:
        chal = css_get_challenge(sock)

    ms1 = fragmon.get_millis()
    sock.send(GET_PLAYERS + chal)
    sinfo = css_recv_multi_packet(sock)
    ms2 = fragmon.get_millis()
    ping = ms2 - ms1

    header = sinfo[4]
    assert header == 'D', 'bad header for get players'
    player_count = ord(sinfo[5])
    pos = 6

    players = []
    while pos < len(sinfo):
        plr = Player()

        plr.id, pos = query.parse_num(sinfo, pos, bytes=1)
        plr.name, pos = query.parse_str(sinfo, pos)
        plr.score, pos = query.parse_num(sinfo, pos, bytes=4)
        pos +=4 #XXX: player time not included

        players.append(plr)

    return ping, players, chal

def css_get_rules(sock, chal=None):
    if chal == None:
        chal = css_get_challenge(sock)

    ms1 = fragmon.get_millis()
    sock.send(GET_RULES + chal)
    sinfo = css_recv_multi_packet(sock)
    ms2 = fragmon.get_millis()
    ping = ms2 - ms1

    header = sinfo[4]
    assert header == 'E', 'bad header for get rules'

    pos = 5
    rules_count, pos = query.parse_num(sinfo, pos, bytes=2)
    rules = [] 
    while pos < len(sinfo):
        key, pos = query.parse_str(sinfo, pos)
        value, pos = query.parse_str(sinfo, pos)
        rules.append(Rule(key, value))

    return ping, rules, chal



if __name__ == '__main__':
    #XXX: this test is no longer current
    host = sys.argv[1]
    port = int(sys.argv[2])
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.connect((host, port))

    #print info
    info = css_get_info(sock)
    for k in info.keys():
        print '%20s: %s' % (k, info[k])

    #print players
    players, chal = css_get_players(sock) 
    for pid, name, kills in players:
        print '%20s: %s' % ('ID', pid)
        print '%20s: %s' % ('Name', name)
        print '%20s: %s' % ('Kills', kills)
        print

    #print cvars (reuse challenge code)
    rules, chal = css_get_rules(sock, chal)
    for key, val in rules:
        print '%20s: %s' % (key, val)

