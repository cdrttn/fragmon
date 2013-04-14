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
from q4 import *
import socket

#return (ping, hostname, map, curplayers, maxplayers, type)
def q4_query_overview(gid, host, port):
    return q4_query_all(gid, host, port)[0]

def q4_query_all(gid, host, port): 
    return q4_query(host, port)

fragmon.register_query(
    name        = 'Quake 4', 
    abbrev      = 'q4', 
    port        = 28004,
    over_cb     = q4_query_overview, 
    all_cb      = q4_query_all,
    exec_order  = '{password_opt} {name_opt} {connect_opt}',
    exec_conn   = '+connect {host}:{port}',  
    exec_pass   = '+set password {password}',
    exec_name   = '+set ui_name {name}',
)

