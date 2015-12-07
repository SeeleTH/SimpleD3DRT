#include "SimpleD3DRT.h"

#include <crtdbg.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	return (SimpleD3DRTApp("Simple Ray Tracing Application", 1600, 900, false)).Run();
}

int SimpleD3DRTApp::onWindowInited()
{
	m_D3dContext.Init(m_hMainWnd, m_u32SizeW, m_u32SizeH);
	return 1;
}

int SimpleD3DRTApp::onWindowUpdated(float deltaTime)
{
	float clearColor[] = { 0.f, 0.f, 0.f, 1.f };
	m_D3dContext.GetDevContext()->ClearRenderTargetView(m_D3dContext.GetRTV(), clearColor);
	m_D3dContext.GetSwapChain()->Present(0, 0);
	return 1;
}

int SimpleD3DRTApp::onWindowDestroyed()
{
	return 1;
}

int SimpleD3DRTApp::onWindowResize()
{
	if (m_D3dContext.GetIsInitialized())
		m_D3dContext.Resize(m_u32SizeW, m_u32SizeH);
	return 1;
}