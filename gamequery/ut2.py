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
import socket
import sys
import query
from fragmon import Overview, Player, Rule


SERVER_INFO = '\x78\x00\x00\x00\x00'
GAME_INFO = '\x78\x00\x00\x00\x01'
PLAYER_INFO = '\x78\x00\x00\x00\x02'
COMBO_INFO =  '\x78\x00\x00\x00\x03'

class UT2Info(Overview):
    def __init__(self, game_port=0, *args, **kwargs):
        Overview.__init__(self, *args, **kwargs)
        game_port = game_port


#parse string type 1 returns (parsed_str, end_pos)
def ut_parse_str(buf, pos=0):
    n = ord(buf[pos])
    #print 'LEN', n, repr(buf[pos])
    #print 'COPY', repr(buf[pos+1:pos+n])

    #make sure there are no null bytes
    copy = buf[pos+1:pos+n].replace('\x00', '') 
    pos += n+1
    return copy, pos

#check serverinfo
def ut2_get_info(sock):
    ms1 = fragmon.get_millis()
    sock.send(SERVER_INFO)
    sinf = sock.recv(65535)
    ms2 = fragmon.get_millis()
    ping = ms2 - ms1

    header = sinf[0]
    query_type = sinf[4]
    assert ord(query_type) == 0, 'invalid info reply'
   
    info = UT2Info(teamAname = 'Red', teamBname = 'Blue', teamAcolor=0xffb1b1, teamBcolor=0xa6acff)

    #start past headder and other fluff
    pos = 10            
    info.game_port,     pos = query.parse_num(sinf, pos, bytes=4) #get game port
    #move past more fluff 
    pos += 4 
    info.hostname,      pos = ut_parse_str(sinf, pos)
    info.map,           pos = ut_parse_str(sinf, pos)
    info.type,          pos = ut_parse_str(sinf, pos)
    info.curplayers,    pos = query.parse_num(sinf, pos, bytes=4)
    info.maxplayers,    pos = query.parse_num(sinf, pos, bytes=4)

    info.ping = ping
    gt = info.type.lower()

    if gt.find('ctf') >= 0 or gt.find('team') or gt.find('onslaught') >= 0:
        info.mode = "team"
    else:
        info.mode = "1v1"

    return ping, info

#update info with a rule list
def ut2_update_info(info, rules):
    for rule in rules:
        k = rule.key.lower()
        v = rule.value.lower()

        if k == 'gamepassword':
            if v == 'true':
                info.password = 1
            else:
                info.password = 0
        elif k == 'serverversion':
            info.version = rule.value

#process cvar rules
def ut2_get_rules(sock):
    if isinstance(sock, str):
        ping = 0
        sinf = sock
    else:
        ms1 = fragmon.get_millis()
        sock.send(GAME_INFO)
        sinf = sock.recv(65535)
        ms2 = fragmon.get_millis()
        ping = ms2 - ms1

    print 'RULES', repr(sinf)
    header = sinf[0]
    query_type = sinf[4]
    print 'RULEHEAD', ord(header), ord(query_type)
    assert ord(query_type) == 1, 'invalid server info reply'

    pos = 5
    cvars = []
    cvar = []
    key = True
    while pos < len(sinf):
        s, pos = ut_parse_str(sinf, pos)
        cvar.append(s)
        if not key:
            cvars.append(Rule(cvar[0], cvar[1]))
            cvar = []

        key = not key

    return ping, cvars


#process players
def ut2_get_players(sock):
    if isinstance(sock, str):
        ping = 0
        sinf = sock
    else:
        ms1 = fragmon.get_millis()
        sock.send(PLAYER_INFO)
        sinf = sock.recv(65535)
        ms2 = fragmon.get_millis()
        ping = ms2 - ms1

    header = sinf[0]
    query_type = sinf[4]
    assert ord(query_type) == 2, 'invalid server info reply'
    pos = 5

    #print repr(sinf)
    #print repr(sinf[pos:])

    players =[]
    while pos < len(sinf):
        plr = Player()
        plr.id, pos = query.parse_num(sinf, pos, bytes=4)
        plr.name, pos = ut_parse_str(sinf, pos)
        plr.ping, pos = query.parse_num(sinf, pos, bytes=4)
        plr.score, pos = query.parse_num(sinf, pos, bytes=4, signed=True)
        stats, pos = query.parse_num(sinf, pos, bytes=4)
        if (stats & 1L<<29):
            plr.team = "Red"
            plr.teamid = 0
        elif (stats & 1L<<30):
            plr.team = "Blue"
            plr.teamid = 1
        else:
            plr.team = ""
            plr.teamid = 2

        players.append(plr)
    
    return ping, players


def ut2_get_players_rules(sock):
    sock.send(COMBO_INFO)
    rules_all = []
    players_all = []

    while 1:
        oldto = sock.gettimeout()
        sock.settimeout(0.5)
        try:
            pack = sock.recv(65535)
            if ord(pack[4]) == 1:
                rules = ut2_get_rules(pack)
                rules_all += rules[1]
            else:
                players = ut2_get_players(pack)
                players_all += players[1]
        except socket.timeout:
            break
        sock.settimeout(oldto)

    return players_all, rules_all

if __name__ == '__main__':
    host = sys.argv[1]
    port = int(sys.argv[2]) #query port is game port +1 

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.connect((sys.argv[1], int(sys.argv[2])))

    ping, hash = ut2_get_info(sock)
    print ping, 'ms'
    #print repr(sinf)
    print 'Game port:\t\t%d' % hash['GamePort'] 
    print 'Host name:\t\t"%s"' % hash['Hostname'] 
    print 'map name:\t\t"%s"' % hash['Map']
    print 'game type:\t\t"%s"' % hash['GameType']
    print 'numplayers:\t\t%d' % hash['CurrentPlayers'] 
    print 'maxplayers:\t\t%d' % hash['MaxPlayers']

#    ping, rules = ut2_get_rules(sock)
#    print ping, 'ms'
#    for key, val in rules:
#        print key, '=', val
#
#    if hash['CurrentPlayers']:
#        ping, players = ut2_get_players(sock)
#    else:
#        players = []

    players, rules = ut2_get_players_rules(sock)

    print 'PLAYERS', players
    print
    print 'RULES', rules

    print
    print ping, 'ms'
    for pid, name, ping, score, stats, team in players:
        print 'Pid ', pid
        print 'Name', name
        print 'Ping', ping
        print 'Score', score
        print 'Stats', stats
        print 'Team', team
        print

