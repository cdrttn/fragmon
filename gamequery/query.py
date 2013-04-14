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

#common utils for game querying

#process num from buf starting from pos returns (num, end_pos)
def parse_num(buf, pos=0, bytes=2, signed=False, bigend=False):
    x = 0
    #num = buf[pos:pos+bytes]
    if bigend:
        y = bytes-1
        for i in xrange(bytes):
            x |= ord(buf[pos+i]) << (y - i)*8
    else:
        for i in xrange(bytes):
            x |= ord(buf[pos+i]) << i*8
    if signed:
        half = 0x80 << (bytes-1) * 8    
        #print half
        if x & half:
            x -= half*2
            #print 'here',x
    pos += bytes
    return x, pos 

#parse out a null terminated string returns (str, end_pos)
def parse_str(buf, pos=0):
    end = buf[pos:].find('\x00')
    str = buf[pos:pos+end]
    pos += end+1
    return str, pos


class Info:
    def __init__(self, score, teamid):
        self.score = score
        self.teamid = teamid


class Plusinfo:
    def __init__(self):
        self.iter = 0
        self.data = {}
        self.def_score = 0
        self.def_team = 2

    def add_map(self, teamid, teams, scores=[]):
        """teams should contain a list mapping a player id to a team
           scores should contain a list of scores for each player"""

        if isinstance(teams, str):
            teams = self.make_list(teams)
        if isinstance(scores, str):
            scores = self.make_list(scores)

        for i in xrange(len(teams)):
            pid = teams[i]
            score = 0
            try:
                score = scores[i]
            except:
                pass

            self.data[pid] = Info(score, teamid)

    def get_score(self, id=-1):
        if id < 0: id = self.iter
            
        score = self.def_score
        if self.data.has_key(id):
            score = self.data[id].score

        return score

    def get_team(self, id=-1):
        if id < 0: id = self.iter

        teamid = self.def_team
        if self.data.has_key(id):
            teamid = self.data[id].teamid
        
        return teamid
    
    def next(self):
        self.iter += 1

    def get_score_next(self):
        score = self.get_score()
        self.next()
        return score

    def get_team_next(self):
        team = self.get_team()
        self.next()
        return team

    @staticmethod
    def make_list(str):
        list = []
        try:
            list = [int(i) for i in str.split()]
        except ValueError:
            pass

        return list

