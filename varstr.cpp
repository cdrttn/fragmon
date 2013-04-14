/*
** Copyright (c) 2006 Christopher Davis
** 
** Permission is hereby granted, free of charge, to any person obtaining a copy of
** this software and associated documentation files (the "Software"), to deal in 
** the Software without restriction, including without limitation the rights to 
** use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
** the Software, and to permit persons to whom the Software is furnished to do so,
** subject to the following conditions:
** 
** The above copyright notice and this permission notice shall be included in all 
** copies or substantial portions of the Software.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
** FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
** COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
** IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
** 
** $Id$
*/



#include "wx/wx.h"
#include "wx/hashmap.h"
#include "varstr.h"

//substitute {var} with value
wxString VarSubst(const wxString &in, const VarMap &m_vmap, bool doempty)
{
    int len = in.Len();
    wxString out;
    out.Alloc(len * 3);

    wxString varname;
    bool copyvar = false;
    bool block = false;
    for (int i = 0; i < len; i++)
    {
        char c = in.GetChar(i);

        if (c == '\\' && !block) 
            block = true;
        else if (c == '{' && !block)
            copyvar = true;
        else if (c == '}' && !block && copyvar)
        {
            copyvar = false;
            VarMap::const_iterator iter = m_vmap.find(varname);
            if (iter == m_vmap.end())
            {
                //printf("Failed to find %s\n", varname.c_str());
                if (doempty)
                {
                    varname.Prepend("{");
                    varname.Append("}");
                    out.Append(varname);
                }
            }
            else
            {
                //printf("%s -> %s found var\n",  iter->first.c_str(), iter->second.c_str());
                out.Append(iter->second);
            }
            varname.Empty();
        }
        else
        {
            block = false;
            if (copyvar)
                varname.Append(c);
            else 
                out.Append(c);
        }

    }

    //printf("output -> %s\n", out.c_str());

    out.Shrink();
    return out;
}

/*
int main(int argc, char **argv)
{
    wxInitialize();
    //if (argc < 0)
    //{
        //puts("ERR substr k1 v1");

    wxString instr = "{here1} {}this {here2} is a magic {here3\\\\} variable string" ;
    printf("before -> %s\n", instr.c_str());

    VarMap vmap;

    vmap["here1"] = "Well i thin so";
    vmap["here2"] = "and a DUe";
    vmap["here3"] = " bye";

    wxString out = VarSubst(instr, vmap);

    printf("after -> %s\n", out.c_str());
    
    return 0;
}
*/
