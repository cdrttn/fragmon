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



#ifndef _PYWRAP_H_
#define _PYWRAP_H_
#include <Python.h>

//wrap data
template <class T>
struct PyData: PyObject 
{
    T *m_data;
    int m_owned;
};

extern PyTypeObject PyPlayerType;
#define PyPlayer_Check(op) PyObject_TypeCheck(op, &PyPlayerType)

extern PyTypeObject PyRuleType;
#define PyRule_Check(op) PyObject_TypeCheck(op, &PyRuleType)

extern PyTypeObject PyOverviewType;
#define PyOverview_Check(op) PyObject_TypeCheck(op, &PyOverviewType)

extern PyTypeObject PyScoreBotType;
#define PyScoreBot_Check(op) PyObject_TypeCheck(op, &PyScoreBotType)


//funcs
//MUST use Py*_Check before using this function...
template <class T>
T *PyData_Detach(PyObject *obj)
{
    T *data;
    PyData<T> *pydata = (PyData<T> *)obj;

    data = pydata->m_data;
    pydata->m_data = NULL;

    return data;
}

#define PyPlayer_Detach(obj) PyData_Detach<GamePlayer>(obj)
#define PyRule_Detach(obj) PyData_Detach<GameRule>(obj)
#define PyOverview_Detach(obj) PyData_Detach<GameInfo>(obj)
#define PyScoreBot_Detach(obj) PyData_Detach<PyScoreBot>(obj)

template <class T>
bool PyData_Attach(PyObject *obj, T *data, bool owned)
{
    PyData<T> *pydata = (PyData<T> *)obj;

    if (pydata->m_data != NULL)
        return false;

    pydata->m_data = data;
    pydata->m_owned = owned?1:0;

    return true;
}

#define PyOverview_Attach(obj, data, owned) PyData_Attach<GameInfo>(obj, data, owned)
#define PyRule_Attach(obj, data, owned) PyData_Attach<GameRule>(obj, data, owned)
#define PyPlayer_Attach(obj, data, owned) PyData_Attach<GamePlayer>(obj, data, owned)
#define PyScoreBot_Attach(obj, data, owned) PyData_Attach<PyScoreBot>(obj, data, owned)

bool PyEmbAddWrappers(PyObject *m);
#endif
