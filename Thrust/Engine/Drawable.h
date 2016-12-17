#pragma once
#include "Mat3.h"
#include "Rect.h"

class Drawable
{

public:
	Drawable()
		:
		trans( Mat3::Identity() ),
		// initialize the clipping rectangle with the maximun area possible (float FLT_MAX)) (at the beginning nothing is clipped)
		clip(-INT_MAX, (INT_MAX - 100), -INT_MAX, (INT_MAX - 100))
	{}
	void Transform(const Mat3& mat)
	{
		trans = mat * trans;
	}
	void Clip(const RectF& rect)
	{
		clip.ClipTo(rect);
	}
	// Pure virtual function 
	// class word means foward declaration, instead of including D3DGraphics.h, we can use this type of declaration to avoid circular dependencies
	virtual void Rasterize(class D3DGraphics& gfx) const = 0;
protected:
	// Transformation matrix for transformations
	Mat3 trans;

	// Int rectangle for clipping
	RectF clip;
};
