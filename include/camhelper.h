#ifndef CAMHELPER_H
#define CAMHELPER_H

#include "mathhelper.h"

namespace NPCamHelper
{
	class ICamera
	{
	public:
		virtual void UpdateViewMatrix() = 0;
		virtual const float* GetViewMatrix() = 0;
	};
}
#endif