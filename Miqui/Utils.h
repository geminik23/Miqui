#pragma once
#include "BaseHeader.h"

namespace Miqui
{

inline bool isInRegion(const D2D1_POINT_2F& p, const D2D1_RECT_F& rect)
{
	return (rect.left <= p.x
			&& rect.right >= p.x
			&& rect.top <= p.y
			&& rect.bottom >= p.y
			);
}
}