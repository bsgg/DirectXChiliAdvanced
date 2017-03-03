#pragma once

#include "Vec2.h"
#include "Colors.h"
#include "D3DGraphics.h"
#include "Mat3.h"
#include "Drawable.h"
#include <vector>
#include <memory>
#include <algorithm>

class TriangleStrip
{
public:

	// Internal class to represent a Drawable object inside TriangleStrip
	class Drawable : public ::Drawable
	{
	public:
		Drawable(const TriangleStrip& parent)
			:parent(parent)
		{}
		virtual void Rasterize(D3DGraphics & gfx) const override
		{
			RectI clipI(clip);
			for (auto it = parent.vertices.begin(), end = parent.vertices.end() - 2 ; it != end; it++)
			{
				// Take the 3 first in the series to draw the triangle
				gfx.DrawTriangle(trans * (*it), trans * (*(it + 1)), trans * *(it+2), clipI, parent.color);
			}
		}
	private:
		const TriangleStrip& parent;
	};

public:
	/*TriangleStrip(std::initializer_list<Vec2> vList, Color color = WHITE)
		:
		vertices(vList),
		color(color)
	{}*/

	TriangleStrip(std::vector<Vec2>&& movable, Color color = WHITE)
		:
		vertices(movable),
		color(color)
	{}
	
	// Gets the drawable internal class
	Drawable GetDrawable() const
	{
		return Drawable(*this);
	}

private:
	Color color;
	std::vector<Vec2> vertices;

};
