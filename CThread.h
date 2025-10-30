#ifndef __CThread__
#define __CThread__

#include <windows.h>

class CThread
{
protected:
	DWORD   d_threadID ;
	bool    d_bIsRunning ;

public:
	HANDLE  d_threadHandle ;
	CThread() ;
	virtual ~CThread() ;

	void BeginThreadProc() ;
	void EndThreadProc() ;
	bool ThreadIsRunning() ;

	virtual DWORD ThreadProc() ;
} ;

static DWORD WINAPI CThreadProc(CThread *pThis) ;

#endif