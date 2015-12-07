#ifndef D3DHELPER_H
#define D3DHELPER_H

#include <D3DX11.h>
#include <DxErr.h>

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
		{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
				{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, #x, true); \
				}                                                      \
		}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

namespace NPD3DHelper
{
	class D3DContext
	{
	public:
		D3DContext()
			: m_D3dDriverType(D3D_DRIVER_TYPE_HARDWARE)
			, m_pD3dDevice(0)
			, m_pD3dImmediateContext(0)
			, m_pSwapChain(0)
			, m_pDepthStencilBuffer(0)
			, m_pRenderTargetView(0)
			, m_pDepthStencilView(0)
			, m_bEnable4xMsaa(false)
			, m_ui4xMsaaQuality(0)
			, m_bIsInitialized(false)
		{
			ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));
		}

		~D3DContext();

		UINT32 Init(const HWND hwnd, const  UINT32 width, const  UINT32 height);
		UINT32 Resize(const UINT32 width, const UINT32 height);

		inline ID3D11Device* GetDevice() { return m_pD3dDevice; }
		inline ID3D11DeviceContext* GetDevContext() { return m_pD3dImmediateContext; }
		inline IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
		inline ID3D11RenderTargetView* GetRTV() { return m_pRenderTargetView; }
		inline ID3D11DepthStencilView* GetDSV() { return m_pDepthStencilView; }
		inline const bool GetIsInitialized() { return m_bIsInitialized; }

	protected:
		ID3D11Device* m_pD3dDevice;
		ID3D11DeviceContext* m_pD3dImmediateContext;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11DepthStencilView* m_pDepthStencilView;
		D3D11_VIEWPORT m_ScreenViewport;

		D3D_DRIVER_TYPE m_D3dDriverType;
		bool m_bEnable4xMsaa;
		UINT m_ui4xMsaaQuality;

		bool m_bIsInitialized;
	};
}

#endif