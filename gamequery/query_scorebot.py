import fragmon
import irc
from fragmon import Player

#some CTCP command handlers
def on_version(mask, location, cmd, args):
    irc.ctcp(mask.nick, 'VERSION', fragmon.APPNAME, fragmon.VERSION, fragmon.WEBSITE)
irc.on_ctcp('*', 'version', on_version)


class SumBot(fragmon.ScoreBot):
    """ This is an example scorebot that can optionally sum player scores 
        to obtain the total team scores.  Notice the two class variables, 
        bot_name and bot_option. Both are required for the bot to be 
        registered. """

    #This name will appear in the list of custom scorebots in the scorebot dialog
    bot_name = 'Team Score Sum'

    #These options appear in the extended list of options in the scorebot dialog
    #The first in the tuple is the name of the option, and the second is its default value.
    #By default, the variables are accessable as members like self.do_sum
    bot_options = [
        ('do_sum', 'False'),
        ('private_players_display', 'False'),
    ]

    def __init__(self):
        """ Constructor. Call the base constructor, and set up IRC callbacks,
            etc. """

        fragmon.ScoreBot.__init__(self)
        irc.on_text('#*', '!players*', self.on_players)

    def __cleanup__(self):
        """ This is called by Fragmon before it destroys the bot's base
            object.  It should remove callbacks and references to members, 
            and perform other cleanup measures.

            If the reference count of the Python scorebot is nonzero
            after this point, there will be RuntimeErrors triggered if
            the bot attempts to use any of the inherited members, such
            as channel, sub_format(), get_players(), etc. """
            
        irc.remove_all(self)

    def set_option(self, key, value):
        """ Convert options from string values. 
            In this case there's only two options, both bool type. Conversion 
            errors, which will appear in a message box, can be triggered 
            by returning a string. """

        if key in ('private_players_display', 'do_sum'):
            value = value.strip().lower()
            if value in ('on', 'yes', 'true', '1'):
                value = True
            elif value in ('off', 'no', 'false', '0'):  
                value = False
            else:
                return 'invalid value for option "%s"' % key
        else:
            #this block should never be reached
            return 'unknown option, ' + key

        setattr(self, key, value)

    def get_option(self, key):
        """ With this bot, simply use the base get_option method, which
            calls getattr with string conversion, to obtain the option 
            value. More complex bots may wish to use their own system 
            to access options. """ 

        return fragmon.ScoreBot.get_option(self, key)

    def get_teams(self):
        players = self.get_players()
        alist = [plr for plr in players if plr.teamid == 0]
        blist = [plr for plr in players if plr.teamid == 1]
        flist = [plr for plr in players if plr.teamid not in (0,1)]
        
        return alist, blist, flist

    def on_players(self, mask, location, text):
        """ Simple 2 line broadcast of players.
            This is triggered by an IRC event. Note that the player
            data retreived here is from the last query of the server """

        #output the player list to the requester directly 
        #instead of the channel if the bot owner wishes it
        if self.private_players_display:
            recv = mask.nick
        else:
            recv = self.channel

        alist, blist, flist = self.get_teams()

        aout = [self.name_a, ': ']
        bout = [self.name_b, ': ']
        for ls, out in [(alist, aout), (blist, bout)]:
            if ls:
                for plr in ls:
                    s = '%s %dpts' % (plr.name, plr.score)
                    out.append(s)
                    out.append(', ')

                del out[-1]
            else:
                out.append(' <No players> ')
       
        aout = ''.join(aout)
        bout = ''.join(bout)

        irc.msg(recv, aout)
        irc.msg(recv, bout)

    def broadcast_players(self):
        """ Print detailed player list to channel.
            This is only triggerable directly by the Fragmon user, as it can be slow. 
            Note that the player data retreived here is from the last query of the server"""

        alist, blist, flist = self.get_teams() 
      
        for ls, name in [(alist, self.name_a), (blist, self.name_b), (flist, 'specs/other')]:
            if ls:
                irc.msg(self.channel, 'Pts   Ping  Name (%s)' % name)
                irc.msg(self.channel, '----------------')
                for plr in ls:
                    irc.msg(self.channel, '%-5d %-5d %s' % (plr.score, plr.ping, plr.name))

      
        if not alist and not blist and not flist:
            irc.msg(self.channel, 'No players')

    def sum_score(self, newlist):
        """ sum player scores """
        
        #XXX: note that this example is very basic and likely doesn't account for lineup changes
        score = 0
        for p in newlist:
            score += p.score

        return score
        
    def broadcast(self):
        """ Print scores to channel using the broadcast format configured by user.
            This adds two new format specifiers, 'best_name' and 'best_score'.
            This also does team sum if the do_sum option is set. """ 
    
        alist, blist, flist = self.get_teams() 

        if self.do_sum:
            self.score_a = str(self.sum_score(alist))
            self.score_b = str(self.sum_score(blist))

        bestplayer = Player(name='...', score=-999)
        search = alist + blist
        for plr in search:
            if plr.score > bestplayer.score:
                bestplayer = plr
       
        v = [
            ('best_name', bestplayer.name),
            ('best_score', str(bestplayer.score)),
        ]

        out = self.sub_format(vars=v)
        irc.msg(self.channel, out)

#This registers the scorebot in Fragmon.
#After this point, the bot should be available in the dropdown menu in the scorebot dialog
fragmon.register_scorebot(SumBot)

