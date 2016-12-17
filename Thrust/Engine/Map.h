#pragma once

#include "Ship.h"

class Map
{

public:
	Map(std::string filename)
		:
		model(filename)
	{}

	PolyClosed::Drawable GetDrawable() const
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
		model.HandleCollision(object);
	}

private:

	PolyClosed model;

};
