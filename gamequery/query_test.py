import fragmon
import fragmon.irc
#from fragmon import irc
import irc

def testhook(*args):
    print 'A', args

"""
def testhook2(err, prefix, cmd, args):
    nick = ''
    pos = prefix.find('!')
    if pos >= 0:
        nick = prefix[:pos]

    cmd = cmd.lower()
    if cmd == 'notice' or cmd == 'privmsg':
        tgt, msg = args
        if msg[0] == '\x01' and msg[-1] == '\x01':
            print 'GOT CTCP MSG'
            ctcp = msg[1:-1].lower()
            if ctcp == 'version' and nick: 
                fragmon.irc.quote('privmsg', nick, '\x01VERSION Fragmon Testing 01\x01')
            elif ctcp == 'yoo' and nick: 
                fragmon.irc.quote('privmsg', nick, 'Hi my name is DOG i have POo')
"""
fragmon.irc.add_hook(testhook)
#fragmon.irc.quote('join', '#booh')
#fragmon.irc.quote('privmsg', '#booh', 'i likee to party baybe')
#fragmon.irc.quote('privmsg', 'jig', '\x01VERSION\x01')


def on_acttext(mask, location, text):
    irc.msg(location, 'man you sure stink, ' + mask.nick)
irc.on_action('#*', '*smelly*', on_acttext)

#print 'SCOREBOT IS THIS I THINK',  fragmon.ScoreBot

class MyBot(fragmon.ScoreBot):
    bot_name = 'Live long and prosper'
    bot_options = [
        ('newbian', 'rugman'),
        ('daooo', '3'),
        ('myshit', '44'),
        ('rubba dub dub',)
    ]

    def __init__(self):
        fragmon.ScoreBot.__init__(self)

        #self.game = 'q3'
        #self.host = '216.15.134.200'
        #self.port = 27960 
        #self.interval = 10
        #self.start()


    def broadcast(self):
        print 'boohea broadcast'
        print 'Is running?', self.running
        overview = self.get_overview()
        overview.teamAname = 'juxtapose'
        overview.ping = 555
        """
        print 'OVERVIEW'
        for m in dir(overview):
            print m, getattr(overview, m)

        print 'TEAMS'
        for m in dir(teams):
            print m, getattr(teams, m)

        print 'PLAYERS'
        for pl in self.get_players():
            print pl.score, pl.name
        
        print 'RULES'
        for rl in self.get_rules():
            print rl.key, rl.value
        """
        print 'ping', overview.ping

        self.name_a = 'felbo'
        self.name_b = 'dillweed'
        self.score_a = '55'
        self.score_b = '33'
        self.title = 'lets ROCK'
        print 'FORMAT 1', self.sub_format(format="Ping = {ping}")
        vars = [
            ('mykey', 'my first key'),
            ('other', 'other'),
        ]

        out = 'FORMAT 2' + self.sub_format("mykey = '{mykey}', Ping = {ping}, other = '{other}'", vars)
        fragmon.irc.quote('privmsg', self.channel, out)

        self.format += " PING = {ping}"
        fragmon.ScoreBot.broadcast(self)
        #self.running = False

class MyBot2(fragmon.ScoreBot):
    bot_name = 'tester 2'
    bot_options = [
        ('option1', 'value'),
        ('option2', 'i like to dance'),
        ('option3', 'some str')
    ]

    def __init__(self):
        fragmon.ScoreBot.__init__(self)

        self.chan = '#roquet'
        irc.on_text('#*', '*love*', self.on_text)
        irc.on_notice('#*', '*love*', self.on_text)

        irc.join(self.chan)
        irc.msg(self.chan, 'i love you bayebe')

    def __cleanup__(self):
        print 'CLEAN up'
        irc.remove_all(self)
        print irc.cmd_callbacks

    def on_text(self, mask, location, text):
        irc.msg(mask.nick, 'got msg from %s -> %s' % (location, text))
        irc.msg(mask.nick, 'thanks, i love you too, ' + mask.nick)
        
    def set_option(self, key, value):
        print 'PY set option %s = %s' % (key, value)
        if value == 'gin':
            return 'dont drink gin bitch'

        fragmon.ScoreBot.set_option(self, key, value)

    def get_option(self, key):
        opt = fragmon.ScoreBot.get_option(self, key)
        print 'Py Get option', opt
        return opt

    def broadcast(self):
        print 'GOT broadcast'
        fragmon.ScoreBot.broadcast(self)

    def broadcast_players(self):
        print 'GOT broadcast_players'

        players = self.get_players()
        alist = [plr for plr in players if plr.teamid == 0]
        blist = [plr for plr in players if plr.teamid == 1]
        flist = [plr for plr in players if plr.teamid not in (0,1)]
      
        for ls, name in [(alist, self.name_a), (blist, self.name_b), (flist, 'specs/other')]:
            if ls:
                irc.msg(self.channel, 'Pts   Ping  Name (%s)' % name)
                irc.msg(self.channel, '----------------')
                for plr in ls:
                    irc.msg(self.channel, '%-5d %-5d %s' % (plr.score, plr.ping, plr.name))


fragmon.register_scorebot(MyBot)
fragmon.register_scorebot(MyBot2)
#mb = MyBot()
