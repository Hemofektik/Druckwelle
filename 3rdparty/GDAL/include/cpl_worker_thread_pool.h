/**********************************************************************
 * $Id$
 *
 * Project:  CPL - Common Portability Library
 * Purpose:  CPL worker thread pool
 * Author:   Even Rouault, <even dot rouault at spatialys dot com>
 *
 **********************************************************************
 * Copyright (c) 2015, Even Rouault, <even dot rouault at spatialys dot com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifndef _CPL_WORKER_THREAD_POOL_H_INCLUDED_
#define _CPL_WORKER_THREAD_POOL_H_INCLUDED_

#include "cpl_multiproc.h"
#include "cpl_list.h"
#include <vector>

class WorkerThreadPool;

typedef struct
{
    CPLThreadFunc      pfnInitFunc;
    void              *pInitData;
    WorkerThreadPool  *poTP;
    CPLJoinableThread *hThread;
} WorkerThread;

class CPL_DLL WorkerThreadPool
{
        std::vector<WorkerThread> aWT;
        CPLCond* hCond;
        CPLCond* hCondWarnSubmitter;
        CPLMutex* hCondMutex;
        volatile int bStop;
        CPLList* psJobQueue;
        volatile int nPendingJobs;

        static void WorkerThreadFunction(void* user_data);

    public:
        WorkerThreadPool();
       ~WorkerThreadPool();

        int  Setup(int nThreads,
                   CPLThreadFunc pfnInitFunc,
                   void** pasInitData);
        void SubmitJob(CPLThreadFunc pfnFunc, void* pData);
        void WaitCompletion(int nMaxRemainingJobs = 0);

        int GetThreadCount() const { return (int)aWT.size(); }
};

#endif // _CPL_WORKER_THREAD_POOL_H_INCLUDED_
