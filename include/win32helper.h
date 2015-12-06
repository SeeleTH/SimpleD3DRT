#ifndef WIN32HELPER_H
#define WIN32HELPER_H

#include "templatehelper.h"

#include <Windows.h>
#include <string>

#define WINBOX_ERROR(_MSG_) \
			    {\
    std::string msg(_MSG_); \
    MessageBox(NULL, \
    msg.c_str(), \
    "ERROR!", \
    MB_ICONWARNING | MB_OK\
    ); \
			    }

namespace NPWin32Helper
{
	class SingleWindowApp : public NPTemplateHelper::NonCopyable, NPTemplateHelper::Singleton<SingleWindowApp>
	{
	public:
		SingleWindowApp()
			: m_sAppName("N/A")
			, m_u32SizeW(800)
			, m_u32SizeH(600)
			, m_bIsReceiveQuitMsg(false)
			, m_bActivated(false)
			, m_bMinimized(false)
			, m_bMaximized(false)
			, m_bResizing(false)
		{}

		SingleWindowApp(const char* name, const UINT32 width, const UINT32 height, const bool fullsrc)
			: m_sAppName(name)
			, m_u32SizeW(width)
			, m_u32SizeH(height)
			, m_bIsReceiveQuitMsg(false)
			, m_bActivated(false)
			, m_bMinimized(false)
			, m_bMaximized(false)
			, m_bResizing(false)
		{}

		int Run();

		static LRESULT CALLBACK MsgProcWin32(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	protected:
		int initWin32();
		int destroyWin32();
		int loopWin32();
		int resizeWin32(UINT32 width, UINT32 height, UINT wParam);
		LRESULT msgProcWin32(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		virtual int onWindowInited(){ return 1; };
		virtual int onWindowUpdated(){ return 1; };
		virtual int onWindowDestroyed(){ return 1; };
		virtual int onWindowResize(){ return 1; }

		std::string m_sAppName;
		UINT32 m_u32SizeW;
		UINT32 m_u32SizeH;

		HINSTANCE m_hAppInst;
		HWND m_hMainWnd;
		bool m_bIsReceiveQuitMsg;
		UINT32 m_uQuitParam;
		BOOL m_bActivated;
		BOOL m_bMinimized;
		BOOL m_bMaximized;
		BOOL m_bResizing;
	};
}

#endif