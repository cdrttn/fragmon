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
#include "wx/dir.h"
#include "wx/filename.h"
#include "findapp.h"
#include <stdio.h>
#include <stdlib.h>

//datadir can be a file or directory. it mustn't have a trailing dir / char
wxString FindAppData(const wxString &argv0, const wxString &datadir)
{
    wxPathList path;
    wxFileName file;

    path.AddEnvList("PATH");
   
    wxString found = path.FindAbsoluteValidPath(argv0);
    if (!found.empty())
    {
        //Check in the dir of the executable
        file.Assign(found);
        file.Normalize();
        file.SetFullName(datadir);

        if (file.FileExists() || file.DirExists())    
            return file.GetFullPath();
#if !defined(__WIN32__)
        else
        {
            wxString me = wxFileName::FileName(argv0).GetFullName();
            file.AssignDir("/usr/lib");
            file.SetFullName(datadir);
            
            //Check /usr/lib/me/datadir, and /usr/local/lib/me/datadir
            file.AppendDir(me);
            file.Normalize();
            printf("F: %s\n", file.GetFullPath().c_str());

            if (file.FileExists() || file.DirExists())    
                return file.GetFullPath();

            file.InsertDir(1, "local");
            file.Normalize();
            printf("F: %s\n", file.GetFullPath().c_str());

            if (file.FileExists() || file.DirExists())    
                return file.GetFullPath();

            //Check /opt/me/datadir
            file.Clear();
            file.AssignDir("/opt");
            file.AppendDir(me);
            file.SetFullName(datadir);
            file.Normalize();
            printf("F: %s\n", file.GetFullPath().c_str());

            if (file.FileExists() || file.DirExists())    
                return file.GetFullPath();

            //Check ~/.me/datadir
            file.Clear();
            file.AssignHomeDir();
            file.AppendDir("." + me + "-data");
            file.SetFullName(datadir);
            file.Normalize();
            printf("F: %s\n", file.GetFullPath().c_str());

            if (file.FileExists() || file.DirExists())    
                return file.GetFullPath();
        }
#endif
    }

    return "";
}

/*
int main(int argc, char **argv)
{
    int num;
    wxInitialize();

    if (argc < 2)
    {
        puts("dt datadir");
        return 0;
    }
    
    wxString path = FindAppData(argv[0], argv[1]);

    if (!path.empty())
        printf("found -> %s\n", path.c_str());
    else
        printf("NOT FOUND -> %s\n", path.c_str());

    wxUninitialize();

    return 0;
}
*/
