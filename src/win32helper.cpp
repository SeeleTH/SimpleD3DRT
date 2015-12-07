#include "win32helper.h"
#include "oshelper.h"

namespace NPWin32Helper
{
	LRESULT CALLBACK SingleWindowApp::MsgProcWin32(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return SingleWindowApp::Instance().msgProcWin32(hwnd, msg, wParam, lParam);
	}

	int SingleWindowApp::Run()
	{
		if (!initWin32() || !onWindowInited())
		{
			WINBOX_ERROR("Initialize Win32 Window FAILED!!");
			return -1;
		}

		while (loopWin32())
		{
			float deltaTime = 0;
			__int64 curCount = NPOSHelper::GetCurPerfCount();
			if (m_i64CurPrefCount != 0 && m_bActivated)
			{
				deltaTime = (curCount - m_i64CurPrefCount) * NPOSHelper::GetPerfSecPerCount();
			}
			m_i64CurPrefCount = curCount;
			if (!onWindowUpdated(deltaTime))
				break;
		}

		if (!onWindowDestroyed() || !destroyWin32())
		{
			WINBOX_ERROR("Destroy Win32 Window FAILED!!");
			return -1;
		}

		return 1;
	}

	int SingleWindowApp::initWin32()
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MsgProcWin32;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hAppInst;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = m_sAppName.c_str();

		if (!RegisterClass(&wc))
		{
			MessageBox(0, "RegisterClass Failed.", 0, 0);
			return 0;
		}

		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT R = { 0, 0, m_u32SizeW, m_u32SizeH };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		INT32 width = R.right - R.left;
		INT32 height = R.bottom - R.top;

		m_hMainWnd = CreateWindow(m_sAppName.c_str(), m_sAppName.c_str(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInst, 0);
		if (!m_hMainWnd)
		{
			MessageBox(0, "CreateWindow Failed.", 0, 0);
			return 0;
		}

		ShowWindow(m_hMainWnd, SW_SHOW);
		UpdateWindow(m_hMainWnd);

		return 1;
	}

	int SingleWindowApp::destroyWin32()
	{
		return 1;
	}

	int SingleWindowApp::loopWin32()
	{
		if (m_bIsReceiveQuitMsg)
			return 0;

		MSG msg = { 0 };
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			m_bIsReceiveQuitMsg = true;
			m_uQuitParam = (UINT32)msg.wParam;
		}

		return 1;
	}

	int SingleWindowApp::resizeWin32(UINT32 width, UINT32 height, UINT wParam)
	{
		if (wParam == SIZE_MINIMIZED)
		{
			m_bActivated = false;
			m_bMinimized = true;
			m_bMaximized = false;
			return onWindowResize();
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			m_bActivated = true;
			m_bMinimized = false;
			m_bMaximized = true;
			return onWindowResize();
		}
		else if (wParam == SIZE_RESTORED)
		{
			// Restoring from minimized state?
			if (m_bMinimized)
			{
				m_bActivated = true;
				m_bMinimized = false;
				return onWindowResize();
			}
			// Restoring from maximized state?
			else if (m_bMaximized)
			{
				m_bActivated = false;
				m_bMaximized = false;
				return onWindowResize();
			}
			else if (m_bResizing)
			{
			}
			else
			{
				return onWindowResize();
			}
		}
		return 1;
	}

	LRESULT SingleWindowApp::msgProcWin32(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_ACTIVATE:
			m_bActivated = (LOWORD(wParam) != WA_INACTIVE);
			return 0;

		case WM_SIZE:
			resizeWin32(LOWORD(lParam), HIWORD(lParam), (UINT)wParam);
			return 0;

		case WM_ENTERSIZEMOVE:
			m_bActivated = false;
			m_bResizing = true;
			return 0;

		case WM_EXITSIZEMOVE:
			m_bActivated = true;
			m_bResizing = false;
			onWindowResize();
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);

		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

}