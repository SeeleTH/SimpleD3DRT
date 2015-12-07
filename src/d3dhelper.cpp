#include  "d3dhelper.h"

#include "macrohelper.h"

#include <Windows.h>
#include <assert.h>

namespace NPD3DHelper
{
	D3DContext::~D3DContext()
	{
		N_RELEASE(m_pRenderTargetView);
		N_RELEASE(m_pDepthStencilView);
		N_RELEASE(m_pSwapChain);
		N_RELEASE(m_pDepthStencilBuffer);

		if (m_pD3dImmediateContext)
			m_pD3dImmediateContext->ClearState();

		N_RELEASE(m_pD3dImmediateContext);
		N_RELEASE(m_pD3dDevice);
	}

	UINT32 D3DContext::Init(const HWND hwnd, const UINT32 width, const UINT32 height)
	{
		// Create the device and device context.

		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL featureLevel;
		HRESULT hr = D3D11CreateDevice(
			0,                 // default adapter
			m_D3dDriverType,
			0,                 // no software device
			createDeviceFlags,
			0, 0,              // default feature level array
			D3D11_SDK_VERSION,
			&m_pD3dDevice,
			&featureLevel,
			&m_pD3dImmediateContext);

		if (FAILED(hr))
		{
			MessageBox(0, "D3D11CreateDevice Failed.", 0, 0);
			return 0;
		}

		if (featureLevel != D3D_FEATURE_LEVEL_11_0)
		{
			MessageBox(0, "Direct3D Feature Level 11 unsupported.", 0, 0);
			return 0;
		}

		// Check 4X MSAA quality support for our back buffer format.
		// All Direct3D 11 capable devices support 4X MSAA for all render 
		// target formats, so we only need to check quality support.

		HR(m_pD3dDevice->CheckMultisampleQualityLevels(
			DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_ui4xMsaaQuality));
		assert(m_ui4xMsaaQuality > 0);

		// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.

		DXGI_SWAP_CHAIN_DESC sd;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// Use 4X MSAA? 
		if (m_bEnable4xMsaa)
		{
			sd.SampleDesc.Count = 4;
			sd.SampleDesc.Quality = m_ui4xMsaaQuality - 1;
		}
		// No MSAA
		else
		{
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
		}

		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.OutputWindow = hwnd;
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		// To correctly create the swap chain, we must use the IDXGIFactory that was
		// used to create the device.  If we tried to use a different IDXGIFactory instance
		// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
		// This function is being called with a device from a different IDXGIFactory."

		IDXGIDevice* dxgiDevice = 0;
		HR(m_pD3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

		IDXGIAdapter* dxgiAdapter = 0;
		HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

		IDXGIFactory* dxgiFactory = 0;
		HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

		HR(dxgiFactory->CreateSwapChain(m_pD3dDevice, &sd, &m_pSwapChain));

		N_RELEASE(dxgiDevice);
		N_RELEASE(dxgiAdapter);
		N_RELEASE(dxgiFactory);

		// The remaining steps that need to be carried out for d3d creation
		// also need to be executed every time the window is resized.  So
		// just call the OnResize method here to avoid code duplication.
		m_bIsInitialized = true;

		Resize(width, height);

		return 1;
	}

	UINT32 D3DContext::Resize(const UINT32 width, const UINT32 height)
	{
		assert(m_pD3dImmediateContext);
		assert(m_pD3dDevice);
		assert(m_pSwapChain);

		// Release the old views, as they hold references to the buffers we
		// will be destroying.  Also release the old depth/stencil buffer.

		N_RELEASE(m_pRenderTargetView);
		N_RELEASE(m_pDepthStencilView);
		N_RELEASE(m_pDepthStencilBuffer);


		// Resize the swap chain and recreate the render target view.

		HR(m_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
		ID3D11Texture2D* backBuffer;
		HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
		HR(m_pD3dDevice->CreateRenderTargetView(backBuffer, 0, &m_pRenderTargetView));
		N_RELEASE(backBuffer);

		// Create the depth/stencil buffer and view.

		D3D11_TEXTURE2D_DESC depthStencilDesc;

		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		// Use 4X MSAA? --must match swap chain MSAA values.
		if (m_bEnable4xMsaa)
		{
			depthStencilDesc.SampleDesc.Count = 4;
			depthStencilDesc.SampleDesc.Quality = m_ui4xMsaaQuality - 1;
		}
		// No MSAA
		else
		{
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
		}

		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		HR(m_pD3dDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer));
		HR(m_pD3dDevice->CreateDepthStencilView(m_pDepthStencilBuffer, 0, &m_pDepthStencilView));


		// Bind the render target view and depth/stencil view to the pipeline.

		m_pD3dImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


		// Set the viewport transform.

		m_ScreenViewport.TopLeftX = 0;
		m_ScreenViewport.TopLeftY = 0;
		m_ScreenViewport.Width = static_cast<float>(width);
		m_ScreenViewport.Height = static_cast<float>(height);
		m_ScreenViewport.MinDepth = 0.0f;
		m_ScreenViewport.MaxDepth = 1.0f;

		m_pD3dImmediateContext->RSSetViewports(1, &m_ScreenViewport);

		return 1;
	}
}