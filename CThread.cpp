#pragma warning( disable : 4183 )	// 'missing return type'

#include "CThread.h"

CThread::CThread()
{
	d_threadID = 0 ;
	d_threadHandle = NULL ;
	d_bIsRunning = false ;
}

CThread::~CThread()
{
	EndThreadProc() ;
}

void CThread::BeginThreadProc()
{
	if (d_threadHandle)
		EndThreadProc() ;  // just to be safe.

	// Start the thread.
	d_threadHandle = CreateThread(NULL,
                                  0,
                                  (LPTHREAD_START_ROUTINE)CThreadProc,
                                  this,
                                  0,
                                  (LPDWORD)&d_threadID) ;
	if( d_threadHandle == NULL )
	{
	    // Arrooga! Dive, dive!  And deal with the error, too!
	}
	/*SetThreadPriority(d_threadHandle, THREAD_PRIORITY_HIGHEST) ;*/
	d_bIsRunning = true ;
}

bool CThread::ThreadIsRunning()
{
	DWORD dw ;
	GetExitCodeThread(d_threadHandle, &dw) ;

	return (dw == STILL_ACTIVE) ;
}

static DWORD WINAPI CThreadProc(CThread *pThis)
{
	return pThis->ThreadProc() ;
}

DWORD CThread::ThreadProc()
{
	return 0 ;
}

void CThread::EndThreadProc()
{
	if (d_threadHandle != NULL)
	{
		d_bIsRunning = false ;
		WaitForSingleObject(d_threadHandle, 1000/*INFINITE*/) ;
		CloseHandle(d_threadHandle) ;
		d_threadHandle = NULL ;
	}
}

 
 

