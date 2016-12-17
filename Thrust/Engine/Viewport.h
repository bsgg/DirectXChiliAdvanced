#pragma once
#include "DrawTarget.h"

class Viewport : public DrawTarget
{

public:
	Viewport(DrawTarget& viewport,const RectF& rect)
		:
		next(viewport),
		clip( rect )
	{}

	virtual void Draw(Drawable& object) override
	{
		object.Transform(Mat3::Traslation({ clip.left, clip.top } ));
		object.Clip(clip);
		next.Draw(object);
	}

	float GetWidth() const
	{
		return clip.GetWidth();
	}

	float GetHeight() const
	{
		return clip.GetHeight();
	}

private:
	DrawTarget& next;
	RectF  clip;
};