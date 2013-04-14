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


import socket
if __name__ == '__main__':
    import dummy as fragmon
else:
    import fragmon

from fragmon import Rule, Player, Overview
from query import Plusinfo

class Q3Player(Player):
    def __init__(self, colorname='', *args, **kwargs):
        Player.__init__(self, *args, **kwargs)
        self.colorname = colorname


QUERYCMD = "\377\377\377\377getStatus\n"
QUERYRSP = "\377\377\377\377statusResponse\n"
MAXDGRAM = 65507

#this takes care of regular colored names and OSP enhanced names
def q3_strip_name(name):
    do = 0
    out = [] 

    for c in name:
        if c == '^' and do is not -1:
            do = -1
        elif do < 0:
            if c.lower() == 'x':
                do = 6
            elif c == '^': ##literal ^
                out.append(c)
            else:
                do = 0
        elif do > 0:
            do -= 1
        else:
            out.append(c)

    return ''.join(out)

def q3_make_goods(rules):
    headers = []
    overview = Overview(teamAname='Red', teamBname='Blue', teamAcolor=0xffb1b1, teamBcolor=0xa6acff)
    activels = []
    pi = Plusinfo()

    #From Commander keen's q3 console page:
    #0 - Free For All  	1 - Tournament  	2 - Single Player
    #3 - Team Deathmatch 	4 - Capture the Flag
    #5 - One Flag CTF  	6 - Overload  	7 - Harvester (Team Arena only)
    #XXX: 5 is CA in osp
    #make the headers and overview
    splited = rules.split('\\')[1:]
    for i in range(0, len(splited), 2):
        r = Rule(key=splited[i], value=splited[i+1])
        k = r.key
        v = r.value

        types = ['FFA', '1v1', 'Single player', 'TDM', 'CTF', 'CA', 'Overload', 'Harvester']
        k = k.lower()
        if k == 'sv_hostname':
            overview.hostname = v
        elif k == 'g_gametype':
            overview.type = v
            try:
                overview.type = types[int(v)]
            except KeyError,ValueError:
                pass
            if overview.type in ('CTF', 'TDM', 'CA'):
                overview.mode = 'team'
            elif overview.type == '1v1':
                overview.mode = '1v1'

        elif k == 'sv_maxclients':
            overview.maxplayers = v
        elif k == 'mapname':
            overview.map = v
        elif k == 'version':
            overview.version = v
        elif k == 'g_needpass':
            overview.password = v
        elif k == 'gameversion':
            overview.mod = v
        elif k == 'gamename' and not overview.mod:
            overview.mod = v
        elif k == 'players_red':
            pi.add_map(0, v)
        elif k == 'players_blue':
            pi.add_map(1, v)
        elif k == 'players_active':
            activels = Plusinfo.make_list(v)
        elif k == 'score_time':
            overview.time = v
        elif k == 'score_red':
            overview.teamAscore = v
        elif k == 'score_blue':
            overview.teamBscore = v

        headers.append(r)

    #if there's two active players in duel mode, assume that they are dueling
    if overview.mode == '1v1' and len(activels) == 2:
        pi.add_map(0, activels[0:1])
        pi.add_map(1, activels[1:2])
    else:
        pi.add_map(0, activels)

    #start iter off at 1 because player ids in OSP list are inclusive of 1..max unlike ufreeze
    if overview.mod[:3].lower() == 'osp':
        pi.next()
    return pi, headers, overview 

def q3_make_players(s_players, pi):
    #list the players (score ping name), add team info as found
    splited = s_players.split('\n')
    #players = map(q3_map_player, splited)
    players = []
    for spl in splited:
        pl = Q3Player(id=pi.iter) 
        data = spl.split(' ', 2)
        pl.score = data[0]
        pl.ping = data[1]
            
        #kill the quote marks around the name
        left = data[2].find('\"')
        right = data[2].rfind('\"')
        if right >= 0 and left >= 0:
            name = data[2][left+1:right]
            pl.name = q3_strip_name(name)
            pl.colorname = name

        pl.teamid = pi.get_team_next()
        players.append(pl)
        
    return players
        
def q3_query(host, port):
    headers = []
    players = []

    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect((host, port))

    ms1 = fragmon.get_millis()
    s.send(QUERYCMD)
    rawdata = s.recv(MAXDGRAM)
    ms2 = fragmon.get_millis()
    ping = ms2 - ms1

    rsp = rawdata[:len(QUERYRSP)]
    assert rsp == QUERYRSP, 'bad response from q3 server'
    
    rawdata = rawdata[len(QUERYRSP):]

    #split the data
    delim = rawdata.find('\n')
    s_headers = rawdata[:delim]
    s_players = rawdata[delim+1:rawdata.rfind('\n')]

    pi, rules, overview = q3_make_goods(s_headers)
    players = []
    if s_players:
        players = q3_make_players(s_players, pi)

    overview.ping = ping
    overview.curplayers = len(players)
    return overview, players, rules


if __name__ == '__main__':
    import sys
    ping, headers, players = q3_query(sys.argv[1], int(sys.argv[2]))
    print headers
    for p in players:
        print p
