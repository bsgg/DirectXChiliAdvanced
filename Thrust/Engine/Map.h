#pragma once

#include "Ship.h"
#include "TriangleStrip.h"

class Map
{

public:
	Map(std::string filename)
		:
		boundaries(filename),
		model(boundaries.ExtractStripVertices(wallWidth))
	{}

	TriangleStrip::Drawable GetDrawable() const
	{
		return model.GetDrawable();

	}

	// Update has deltaTime to avoid use all the math operations according at the frame rate (frame speed)
	// The frame rate depends on the monitor, with deltaTime (seconds) 
	void Update(float deltaTime)
	{
		
	}

	void HandleCollision(CollidableCircle& object)
	{
		boundaries.HandleCollision(object);
	}

private:
	const float wallWidth = 40.0f;
	PolyClosed boundaries;
	TriangleStrip model;

};
