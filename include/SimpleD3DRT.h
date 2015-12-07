#ifndef SIMPLED3DRT_H
#define SIMPLED3DRT_H

#include "win32helper.h"
#include "d3dhelper.h"

class SimpleD3DRTApp : public NPWin32Helper::SingleWindowApp
{
public:
	SimpleD3DRTApp(const char* name, const UINT32 width, const UINT32 height, const bool fullsrc)
		:NPWin32Helper::SingleWindowApp(name, width, height, fullsrc)
	{}

protected:
	virtual int onWindowInited();
	virtual int onWindowUpdated(float deltaTime);
	virtual int onWindowDestroyed();
	virtual int onWindowResize();

	NPD3DHelper::D3DContext m_D3dContext;
};
#endif