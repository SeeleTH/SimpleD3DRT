#ifndef THREADHELPER_H
#define THREADHELPER_H

#include "templatehelper.h"

#include <Windows.h>
#include <queue>

namespace NPThreadHelper
{
	class CriticalSection : public NPTemplateHelper::NonCopyable
	{
	public:
		CriticalSection() { InitializeCriticalSection(&m_cs); }
		~CriticalSection() { DeleteCriticalSection(&m_cs); }
		void Lock() { EnterCriticalSection(&m_cs); }
		void Unlock() { LeaveCriticalSection(&m_cs); }

	protected:
		mutable CRITICAL_SECTION m_cs;
	};

	class ScopedCriticalSection : public NPTemplateHelper::NonCopyable
	{
	public:
		ScopedCriticalSection(CriticalSection& csResource) : m_csResource(csResource)
		{
			m_csResource.Lock();
		}

		~ScopedCriticalSection()
		{
			m_csResource.Unlock();
		}

	protected:
		CriticalSection &m_csResource;
	};

	template<typename Data>
	class ConcurrentQueue
	{
	private:
		std::queue<Data> m_queue;
		CriticalSection m_cs;
		HANDLE m_dataPushed;
	public:
		ConcurrentQueue() { m_dataPushed = CreateEvent(NULL, true, false, NULL); }
		void push(Data const& data)
		{
			{
				NScopedCriticalSection locker(m_cs);
				m_queue.push(data);
			}
			PulseEvent(m_dataPushed);
		}

		BOOL empty()
		{
			NScopedCriticalSection locker(m_cs);
			return m_queue.empty();
		}

		BOOL try_pop(Data& poppedValue)
		{
			NScopedCriticalSection locker(m_cs);
			if (m_queue.empty())
			{
				return false;
			}
			poppedValue = m_queue.front();
			m_queue.pop();
			return true;
		}

		void waitAndPop(Data& poppedValue)
		{
			NScopedCriticalSection locker(m_cs);
			while (m_queue.empty())
			{
				WaitForSingleObject(m_dataPushed);
			}

			poppedValue = m_queue.front();
			m_queue.pop();
		}
	};

	class Runnable
	{
	public:
		// Other prioities can be:
		// THREAD_PRIORITY_ABOVE_NORMAL
		// THREAD_PRIORITY_BELOW_NORMAL
		// THREAD_PRIORITY_HIGHEST
		// THREAD_PRIORITY_TIME_CRITICAL
		// THREAD_PRIORITY_LOWEST
		// THREAD_PRIORITY_IDLE
		//
		Runnable(int priority = THREAD_PRIORITY_NORMAL)
			: m_bIsInitted(false)
		{
			m_dThreadID = 0;
			m_iThreadPriority = priority;
		}

		virtual ~Runnable() {
			if (m_hThread != 0) {
				CloseHandle(m_hThread);
				onExit();
			}
		}

		BOOL Init();
		BOOL Run();
		BOOL Stop();
		BOOL Exit(DWORD dwExitCode);
		DWORD Wait(DWORD millisec = INFINITE);

		static DWORD WINAPI ThreadProc(LPVOID lpParam);

	protected:
		virtual BOOL onInit() { return true; }
		virtual void onThreadUpdate(void) = 0;
		virtual void onExit() {}

		bool m_bIsInitted;
		HANDLE m_hThread;
		DWORD m_dThreadID;
		INT32 m_iThreadPriority;
	};
}

#endif