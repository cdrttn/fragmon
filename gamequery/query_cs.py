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
import cssrc
import socket

def cs_query_overview(gid, host, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.connect((host, port)) 
    ping, info = cssrc.css_get_info(sock)

    return info

def cs_query_all(gid, host, port):
    #TEST:
    #import time
    #while 1:
    #    print 'aah'
    #    time.sleep(0.5)
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.connect((host, port)) 

    xx, info = cssrc.css_get_info(sock)
    xx, players, chal = cssrc.css_get_players(sock)
    xx, rules, chal = cssrc.css_get_rules(sock, chal)
    
    return info, players, rules
  

fragmon.register_query('Counter-Strike Source (HL2)', 'cs2', 27015, cs_query_overview, cs_query_all)
fragmon.register_query('Counter-Strike (HL1)', 'cs1', 27015, cs_query_overview, cs_query_all)

