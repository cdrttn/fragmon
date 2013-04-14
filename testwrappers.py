from pywrappers import *

class TPlayer(Player):
    def __init__(self, newmem1='mem1', newmem2='mem2', *args, **kwargs):
        Player.__init__(self, *args,**kwargs)
        self.newmem1 = newmem1
        self.newmem2 = newmem2

print 'test Player empty constructor'
pl = Player()
print 'initial'
print 'player.name, %s' % pl.name
print 'player.score, %d' % pl.score
print 'player.ping, %d' % pl.ping
print 'player.team, %s' % pl.team
print 'player.teamid, %d' % pl.teamid
print 'player.id, %d' % pl.id
print

pl.name = 'donkie'
pl.score = 55
pl.ping = 40
pl.team = 'red'
pl.teamid = 0
pl.id = 55

print 'player.name, donkie == %s' % pl.name
print 'player.score, 55 == %d' % pl.score
print 'player.ping, 40 == %d' % pl.ping
print 'player.team, red == %s' % pl.team
print 'player.teamid, 0 == %d' % pl.teamid
print 'player.id, 55 == %d' % pl.id
print
print

print 'test Player with constructor'
pl = Player(name='donkie', score=55, ping=40, team='red', teamid=0, id=55)

print 'player.name, donkie == %s' % pl.name
print 'player.score, 55 == %d' % pl.score
print 'player.ping, 40 == %d' % pl.ping
print 'player.team, red == %s' % pl.team
print 'player.teamid, 0 == %d' % pl.teamid
print 'player.id, 55 == %d' % pl.id
print
print

print 'test Player Sublass with constructor'
tp = TPlayer(newmem1='iam mem1', newmem2='i am mem2', name='subaru', team='red', teamid=0)
print 'tplayer.name, subaru ==', tp.name
print 'tplayer.team, red ==', tp.team
print 'tplayer.teamid, 0 ==', tp.teamid
print 'tplayer.newmem1, iam mem1 == ', tp.newmem1
print 'tplayer.newmem2, i am mem2 == ', tp.newmem2
print
print

print 'test Rule empty constructor'
r = Rule()
r.key = 'mykey'
r.value = 'myvalue'

print 'rule.key, mykey ==', r.key
print 'rule.value, myvalue ==', r.value
print
print

print 'test Rule with constructor'
r=Rule(key='mykey', value='myvalue')

print 'rule.key, mykey ==', r.key
print 'rule.value, myvalue ==', r.value
print
print

print 'test Teams empty constructor'
t = Teams()
t.teamAname = 'Red'
t.teamBname = 'Blue' 
t.teamAscore = 55
t.teamBscore = 100
t.teamAcolor = 0xff00ff
t.teamBcolor = 0x00ff00

print 'teams.teamAname Red ==', t.teamAname
print 'teams.teamBname Blue ==', t.teamBname
print 'teams.teamAscore, 55 ==', t.teamAscore
print 'teams.teamBscore, 100 ==', t.teamBscore
print 'teams.teamAcolor, 0xff00ff ==', hex(t.teamAcolor)
print 'teams.teamBcolor, 0x00ff00 ==', hex(t.teamBcolor)
print
print

print 'test Teams with constructor'
t = Teams(teamAname='Red', teamBname='Blue', teamAscore=55, teamBscore=100, teamAcolor=0xff00ff, teamBcolor=0x00ff00)
print 'teams.teamAname Red ==', t.teamAname
print 'teams.teamBname Blue ==', t.teamBname
print 'teams.teamAscore, 55 ==', t.teamAscore
print 'teams.teamBscore, 100 ==', t.teamBscore
print 'teams.teamAcolor, 0xff00ff ==', hex(t.teamAcolor)
print 'teams.teamBcolor, 0x00ff00 ==', hex(t.teamBcolor)
print
print

print 'test Overview empty constructor'
o = Overview()
o.port = 5555
o.server = '192.124.23.4234'
o.ping = 55
o.curplayers = 10
o.maxplayers = 16
o.password = False
o.hostname = 'Donkiez nutz'
o.map = 'q3dm6'
o.type = 'TDM'
o.mode = 'team'
o.mod = 'osp'
o.version = '1.32'
o.time = '123:444'

print 'overview.port 5555 ==', o.port
print 'overview.server 192.124.23.4234 ==', o.server
print 'overview.ping 55 ==', o.ping
print 'overview.curplayers 10 ==', o.curplayers
print 'overview.maxplayers 16 ==', o.maxplayers
print 'overview.password False ==', o.password
print 'overview.hostname Donkiez nutz ==', o.hostname
print 'overview.map q3dm6 ==', o.map
print 'overview.type TDM ==', o.type
print 'overview.mode team ==', o.mode
print 'overview.mod osp ==', o.mod
print 'overview.version 1.32', o.version
print 'overview.time 123:444', o.time
print
print

print 'test Overview with constructor'
o = Overview(port=5555, server='192.124.23.4234', ping=55, 
    curplayers=10, maxplayers=16, password=False, hostname='Donkiez nutz',
    map='q3dm6', type='TDM', mode='team', mod='osp', version='1.32', 
    time='123:444')

print 'overview.port 5555 ==', o.port
print 'overview.server 192.124.23.4234 ==', o.server
print 'overview.ping 55 ==', o.ping
print 'overview.curplayers 10 ==', o.curplayers
print 'overview.maxplayers 16 ==', o.maxplayers
print 'overview.password False ==', o.password
print 'overview.hostname Donkiez nutz ==', o.hostname
print 'overview.map q3dm6 ==', o.map
print 'overview.type TDM ==', o.type
print 'overview.mode team ==', o.mode
print 'overview.mod osp ==', o.mod
print 'overview.version 1.32', o.version
print 'overview.time 123:444', o.time
print
print


print 'conversions test'

t = Teams()
t.teamAcolor = '32423'
o = Overview()
o.password = '0'
print 'team.teamAcolor =', hex(t.teamAcolor)
print 'team.teamAcolor =', t.teamAcolor
print 'overview.password =', o.password

print 'test typecheck'
t = Teams()
o = Overview()
p = Player()
r = Rule()

#test_check(teams=t, overview=o, player=p, rule=r)
