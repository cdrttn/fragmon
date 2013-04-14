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


# http://www.int64.org/docs/gamestat-protocols/doom3.html
# /usr/bin/python q4ping.py 216.224.120.88 28004

import fragmon
import socket
#import struct
import sys
import query

from query import Plusinfo
from fragmon import Rule, Player, Overview

QUERYCMD = '\xff\xffgetInfo'
QUERYRESP = '\xff\xffinfoResponse'
QUERYHEADLEN = 23
RESPSIZE = 65535 
STRCOUNT = 4

def get_player(start, buf):
    id = ord(buf[start])
    start += 1
    ping, start = query.parse_num(buf, start, bytes=2)
    rate, start = query.parse_num(buf, start, bytes=2)
    
    #ping, rate = struct.unpack('hh', buf[start+1:start+5])

    #start += 5 
    #fuck this shit
    strs = []
    for i in xrange(STRCOUNT):
        pos = buf[start:].find('\x00')    
        #print 'pos ->', pos
        assert pos >= 0
        s = buf[start:start+pos]
        #print 's ->', s
        start += pos + 1
        strs.append(s)  

    return start, id, ping, rate, strs 

def q4_strip_name(name):
    out = []

    skip = 0
    i = 0
    namel = len(name)
    while i < namel:
        if name[i] == '^':
            if i+1 < namel and name[i+1] in 'IicC':
                i += 5
            else:
                i += 2
        else:
            out.append(name[i])
            i += 1

    return ''.join(out)


def q4_make_goods(rbuf):
    overview = Overview(teamAname = 'Strogg', teamBname = 'Marines', teamAcolor = 0xffc37d, teamBcolor = 0x82ff8b)

    rules = []
    rbuf = rbuf.split('\x00') 
    if len(rbuf) % 2:
        rbuf.append('xxx')
    plusinfoitms = ('.players_strogg', '.players_marine', '.players_active', '.score_strogg', '.score_marine', '.score')   
    plusinfo = {}
    for item in plusinfoitms:
        plusinfo[item[1:]] = []

    for i in xrange(0, len(rbuf), 2):
        k = rbuf[i]
        v = rbuf[i+1]
        rules.append(Rule(k, v))

        k = k.lower()
        if k == 'si_name':
            overview.hostname = v
        elif k == 'si_gametype':
            overview.type = v
            v = v.lower()
            if v in ('team dm', 'ctf'):
                overview.mode = 'team'
            elif v in ('tourney', 'duel'):
                overview.mode = '1v1'
        elif k == 'si_maxplayers':
            overview.maxplayers = int(v)
        elif k == 'si_map':
            overview.map = v
        elif k == 'si_version':
            overview.version = v
        elif k == 'si_usepass':
            overview.password = int(v)
        elif k == 'gamename':
            overview.mod = v
        elif k in plusinfoitms:
            list = Plusinfo.make_list(v)
            plusinfo[k[1:]] = list
        elif k == '.score_time':
            overview.time = v

    #q4max stores the team score as the first item in the list, followed by the player ids
    if plusinfo['score_strogg']:
        overview.teamAscore = plusinfo['score_strogg'][0]
    if plusinfo['score_marine']:
        overview.teamBscore = plusinfo['score_marine'][0]

    pi = Plusinfo()
    pi.add_map(0, plusinfo['players_strogg'], plusinfo['score_strogg'][1:])
    pi.add_map(1, plusinfo['players_marine'], plusinfo['score_marine'][1:])
    pi.add_map(0, plusinfo['players_active'], plusinfo['score'])

    return pi, rules, overview 


def q4_make_players(pbuf, pi):
    buflen = len(pbuf) 
    start = 0
    players = []
    while start < buflen:
        #XXX: Format for q4 1.1 seems to have changed slightly. this hack makes it work
        try:
            start, pid, ping, rate, ss = get_player(start, pbuf)
        except IndexError: 
            break

        plr = Player()
        plr.name = q4_strip_name(ss[-2])
        plr.team = q4_strip_name(ss[-1])
        plr.ping = ping
        plr.id = pid

        #XXX: I believe q4max uses player ids to match players to teams
        plr.score = pi.get_score(pid)
        plr.teamid = pi.get_team(pid)

        #print '%2d %3d %4d %s %s' % (pid, p, rate, ss[-2], ss[-1])
        players.append(plr)

    return players


def q4_query(serv, port):
    ms1 = fragmon.get_millis()
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect((serv, port)) 
    s.send(QUERYCMD)
    respbuf = s.recv(RESPSIZE)
    ms2 = fragmon.get_millis()
    ping = ms2 - ms1

    assert respbuf[:len(QUERYRESP)] == QUERYRESP, 'invalid response from server'

    #print 'RESPBUF', repr(respbuf)

    respbuf = respbuf[QUERYHEADLEN:]   

    #find the end of the server variables
    varend = respbuf.find('\x00\x00\x00')
    assert varend >= 0, 'invalid response from server'

    vbuf = respbuf[:varend]
    pbuf = respbuf[varend+3:-5] #-5, last 5 bytes don't seem to be very meaningful...
 
    pi, rules, overview = q4_make_goods(vbuf)    
    players = q4_make_players(pbuf, pi)

    overview.ping = ping
    overview.curplayers = len(players)

    return overview, players, rules 


if __name__ == '__main__':
    rules, players = q4_query(sys.argv[1], int(sys.argv[2]))
    for k, v in rules:
        print k, '=', v

    print
    for p in players:
        print '%2d %3d %4d %s %s' % p #(pid, ping, rate, ss[-2], ss[-1])

