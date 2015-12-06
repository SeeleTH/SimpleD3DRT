#include "threadhelper.h"

namespace NPThreadHelper
{
	BOOL Runnable::Init(void)
	{
		m_hThread = CreateThread(
			NULL, // default security attributes
			0, // default stack size
			ThreadProc, // thread process
			this, // thread parameter is a pointer to the process
			CREATE_SUSPENDED, // default creation flags
			&m_dThreadID); // receive thread identifier
		if (m_hThread == NULL)
		{
			return false;
		}
		SetThreadPriority(m_hThread, m_iThreadPriority);
		m_bIsInitted = true;

		return onInit();
	}

	BOOL Runnable::Run()
	{
		if (!m_bIsInitted)
			Init();

		return ResumeThread(m_hThread) >= 0;
	}

	BOOL Runnable::Stop()
	{
		return SuspendThread(m_hThread) >= 0;
	}

	BOOL Runnable::Exit(DWORD dwExitCode)
	{
		if (TerminateThread(m_hThread, dwExitCode))
		{
			CloseHandle(m_hThread);
			onExit();
			m_hThread = 0;
			return true;
		}
		return false;
	}

	DWORD  Runnable::Wait(DWORD millisec)
	{
		return WaitForSingleObject(m_hThread, millisec);
	}

	DWORD WINAPI Runnable::ThreadProc(LPVOID lpParam)
	{
		Runnable *proc = static_cast<Runnable *>(lpParam);
		proc->onThreadUpdate();
		return true;
	}
}