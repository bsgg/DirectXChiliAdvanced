#pragma once
#include "DrawTarget.h"
#include "Vec2.h"

class Camera: public DrawTarget
{

public:
	Camera(DrawTarget& viewport, float width, float height)
		:
		next (viewport),
		pos({0.0f,0.0f}),
		toCenter({width/2.0f, height/2.0f})
	{}

	virtual void Draw(Drawable& object) override
	{
		object.Transform(Mat3::Traslation(-pos));
		next.Draw(object);
	}

	void MoveTo(Vec2 newPos)
	{
		pos = newPos - toCenter;
	}
private:
	DrawTarget& next;
	Vec2 pos;

	// Vec that goes from the pos of the camera to the center of the camera
	Vec2 toCenter;
};