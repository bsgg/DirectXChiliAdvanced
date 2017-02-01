#pragma once

#include "Vec2.h"
#include "Colors.h"
#include "D3DGraphics.h"
#include "dxflib\dl_creationadapter.h"
#include "dxflib\dl_dxf.h"
#include "Mat3.h"
#include "Drawable.h"
#include "CollidableCircle.h"
#include <vector>
#include <memory>
#include <algorithm>


class PolyClosed
{
private:

	class Loader : public DL_CreationAdapter
	{
	public:

		Loader(std::string filename)
		{
			// Create dxf object on the heap memory with a pointer and a new, instead of the stack
			// Creation in heap memory: DL_Dxf* dxf = new DL_Dxf();
			// Creation in stack memory: DL_Dxf dxf 
			// c++ 11 version for creation with a smart pointer
			// 1ª way to create smart pointer:
			// std::unique_ptr<DL_Dxf> pDxf( new DL_Dxf());
			// 2º way to create a smart pointer (it seems that this form is faster and better than use new)
			std::unique_ptr<DL_Dxf> pDxf = std::make_unique<DL_Dxf>(); // We can use auto instead of std::unique_ptr<DL_Dxf>

			pDxf->in(filename, this);

			// Remove the object in the heap if the creation was with new, but with a smart pointer
			// when the progam passes this scope, the smart pointer will automatically be deleted
			//delete dxf;

		}

		virtual void addVertex(const DL_VertexData& data) override
		{
			vertices.push_back({ (float)data.x, (float)-data.y });
		}

		operator std::vector< Vec2 >&&() 
		{
			// Use move to copy the vertices into vertices
			return std::move(vertices);
		}

	private:
		std::vector< Vec2> vertices;

	};

public:

	// Internal class to represent a Drawable object inside polyclosed
	class Drawable : public :: Drawable
	{
	public:
		Drawable(const PolyClosed& parent)
			:parent (parent)
		{}
		virtual void Rasterize(D3DGraphics & gfx) const override
		{
			// Draw the current element and the next one until the one before the last one
			// Rotate, scale and move
			// scale (just multiply for a scaler!)
			// Translation is the last element, that's the rule. It doesn´t matter if you rotate and scaler or scale and rotate,
			// but ranslation or movement comes last

			// VECTORS OPTIONS:
			//gfx.DrawLineClip(  it->Rotation(angle)  * scale + pos, (it+1)->Rotation(angle) * scale + pos, color);
			//std::vector <const Vec2>::const_iterator it --> this long sentence could be replaced by auto
			// The compiler will figure out the type behind auto
			for (auto it = parent.vertices.begin(), end = parent.vertices.end() - 1; it != end; it++)
			{
				// Draw the last one to the first one

				// VECTORS OPTIONS:
				//gfx.DrawLineClip(vertices.back().Rotation(angle) * scale + pos, vertices.front().Rotation(angle)* scale + pos, color);
				// TRANSFORMATION MATRIX OPTION:
				gfx.DrawLineClip(trans * (*it), trans * (*(it + 1)), parent.color, clip);
			}	
			// Draw the last one to the first one

			// VECTORS OPTIONS:
			//gfx.DrawLineClip(vertices.back().Rotation(angle) * scale + pos, vertices.front().Rotation(angle)* scale + pos, color);

			// TRANSFORMATION MATRIX OPTION:
			gfx.DrawLineClip(trans * parent.vertices.back(), trans * parent.vertices.front(), parent.color, clip);
		}
	private:
		const PolyClosed& parent;
	};

public:
	PolyClosed(std::initializer_list<Vec2> vList, D3DCOLOR color = WHITE)
		:
		vertices(vList),
		color (color)
		
	{
		// Make the vertices clockwise
		MakeClockWise();	
	}
	PolyClosed(std::string filename, D3DCOLOR color = WHITE)
		:
		// instead of copy a vector from 1 to this one, just move it, it´s faster and better
		vertices( Loader(filename) ),
		color(color)
	{
		// Make the vertices clockwise
		MakeClockWise();
	}
	void HandleCollision(CollidableCircle& obj)
	{
		// Circle elements
		const RectF objAABB = obj.GetAABB();
		const Vec2 center = obj.GetCenter();
		const float radius = obj.GetRadius();

		// Previous vertice in the polyclosed
		Vec2 prev = vertices.back();
		for (auto i = vertices.begin(), end = vertices.end(); i < end; i++)
		{
			const Vec2 current = *i;
			// Each polyclose will have collision for each two consecutive vertices in its shape
			// Create a rectanble between previous vertices and the current one
			const RectF lineAABB(prev, current);
			// Check collision between obj and each bounding box between two consecutive vertices
			if (objAABB.Overlaps(lineAABB))
			{
				// Gets the velocity of the obj, and the normal form the rect
				const Vec2 objVel = obj.GetVel();

				// The normal of those two points is the vector, rotate 90 degrees in clockwise direction 
				// and then	we need to normalize everthing.
				const Vec2 lineNormal = (current - prev).CW90().Normalize();

				// Check the the dot prodcut between objVel and lineNormal
				// If it's negative, obj is aproaching to the line, 
				// if not, we don´t have to do anything else because obj is going away
				const float dot = objVel * lineNormal;
				if (dot < 0.0f)
				{
					const std::vector <Vec2> points = CalculateIntersectionPoints(center, current, prev, radius );
					// If we have at least 2 points the circle bounding box should be colliding with the segment
					// We have to check those points to see if they are 
					if (points.size() == 2)
					{
						const bool contains0 = lineAABB.Contains(points[0]);
						const bool contains1 = lineAABB.Contains(points[1]);

						

						// Check situation where the circle is colliding with the end of the point. If the midpoint between the point
						// 0 and 1 is contained in the line, and both points are different, then make an special rebound in that case
						if (contains0 != contains1 && !lineAABB.Contains(points[0].MidpointWith(points[1])))
						{
							// Collision with the end point of the line
							// Gets d1 and d2, center to each points in the line, and the get the lenght from the center of each element
							// D1 = Center of circle to previous point in the line, and then get the lenght of that vector
							const Vec2 d1 = center - prev;
							const float dSquare1 = d1.LenSq();

							// D2 = Center of circle to current point in the line, and then get the lenght of that vector
							const Vec2 d2 = center - current;							
							const float dSquare2 = d2.LenSq();

							// Check wich "end point of the line" we are colliding with, the previous one or the current one
							// We can get that comparing its distances, the less distance is the one that we are colliding to
							float dSquareCloset;
							Vec2 dClosest;
							if (dSquare1 <= dSquare2)
							{
								dClosest = d1;
								dSquareCloset = dSquare1;
							}
							else
							{
								dClosest = d2;
								dSquareCloset = dSquare2;
							}

							// Dot product between the closest point and the velocity
							// If it´s less than 0, we are aproaching, if it´s not, we are leaving
							// We only have to do the rebaound when the circle is approaching
							if (dClosest * obj.GetVel() < 0.0f)
							{
								obj.Rebound(dClosest /  sqrt(dSquareCloset));
							}
						}
						else if (contains0 || contains1) // It one of those points are colliding
						{
							obj.Rebound(lineNormal);	
						}
					}
				}
			}

			prev = current;
		}
	}
	// Gets the drawable internal class
	Drawable GetDrawable() const
	{
		return Drawable(*this);
	}

	std::vector<Vec2 > ExtractStripVertices(float width) const
	{
		std::vector<  Vec2 > strip;

		// Get all the vertices from the beginning to almost the end
		// End is a special case
		for (auto i = vertices.begin(), end = vertices.end() - 2; i != end; i++)
		{
			// Push the next one
			strip.push_back( *(i + 1));

			// Get the normal from current to prev vertices
			const Vec2 normal0 = (*(i+1) - *(i+0)).CCW90().Normalize();

			// Get the normal from next curret to current vertices
			const Vec2 normal1 = (*(i + 2) - *(i + 1)).CCW90().Normalize();

			// Get normal between these two normals
			const Vec2 b = (normal0 + normal1).Normalize();

			const float k = width /(b* normal0);

			// Vector 2 q, perpendicular to current vertex (between prev vertex and next vertex)
			const Vec2 q = *(i+1) + (b * k);

			// Push back q, the outside vertex between prev and current
			strip.push_back(q);
		}

		// 4, 5
		{
			// Push the next one
			strip.push_back(vertices.back());

			// Get the normal from current to prev vertices
			const Vec2 normal0 = (vertices.back() - *(vertices.end() - 2)).CCW90().Normalize();

			// Get the normal from next curret to current vertices
			const Vec2 normal1 = (vertices.front() - vertices.back()).CCW90().Normalize();

			// Get normal between these two normals
			const Vec2 b = (normal0 + normal1).Normalize();

			const float k = width / (b* normal0);

			// Vector 2 q, perpendicular to current vertex (between prev vertex and next vertex)
			const Vec2 q = vertices.back() + (b * k);

			// Push back q, the outside vertex between prev and current
			strip.push_back(q);
		}

		// 6,7 (last 2)
		{
			// Push the next one
			strip.push_back(vertices.front());

			// Get the normal from current to prev vertices
			const Vec2 normal0 = (vertices.front() - vertices.back()).CCW90().Normalize();

			// Get the normal from next curret to current vertices
			const Vec2 normal1 = (*(vertices.begin() + 1) - vertices.front()).CCW90().Normalize();

			// Get normal between these two normals
			const Vec2 b = (normal0 + normal1).Normalize();

			const float k = width / (b* normal0);

			// Vector 2 q, perpendicular to current vertex (between prev vertex and next vertex)
			const Vec2 q = vertices.front() + (b * k);

			// Push back q, the outside vertex between prev and current
			strip.push_back(q);
		}

		// Push the first 2 elements
		strip.push_back(strip[0]);
		strip.push_back(strip[1]);

		return strip;
	}


private:
	// Evaluates if the poligon is clockwise or not
	bool IsClockwiseWinding() const
	{
		float area = 0.0f;

		// Check all the vertices in the poligon
		Vec2 previous = vertices.back();
		for (auto i = vertices.begin(), end = vertices.end(); i != end; i++)
		{
			const Vec2 current = *i;

			// Calculate the area with all the vertices
			area += (current.x - previous.x) * (current.y + previous.y);
			previous = current;
		};

		return area <= 0.0f;
	}

	void MakeClockWise()
	{
		// If not cw, reverse all the vertices
		if (!IsClockwiseWinding())
		{
			std::reverse(this->vertices.begin(), this->vertices.end());
		}
	}

private:
	D3DCOLOR color;
	std::vector<Vec2> vertices;
	


};
