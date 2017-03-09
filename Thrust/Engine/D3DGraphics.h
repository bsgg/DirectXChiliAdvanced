/****************************************************************************************** 
 *	Chili DirectX Framework Version 14.03.22											  *	
 *	D3DGraphics.h																		  *
 *	Copyright 2014 PlanetChili <http://www.planetchili.net>								  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#pragma once

#include <d3d9.h>
#include "Vec2.h"
#include "Colors.h"
#include "DrawTarget.h"
#include "Surface.h"
#include "Vertex.h"

class D3DGraphics: public DrawTarget
{
public:
	D3DGraphics( HWND hWnd );
	~D3DGraphics();
	void PutPixel( int x,int y,Color c );
	void PutPixelAlpha(int x, int y, Color c);
	Color GetPixel( int x,int y ) const;

	

	inline void DrawLine( Vec2 pt1,Vec2 pt2,Color c ) 
	{
		DrawLine( (int)pt1.x,(int)pt1.y,(int)pt2.x,(int)pt2.y,c );
	}
	void DrawLine( int x1,int y1,int x2,int y2,Color c );
	void DrawLineClip(Vec2 p0, Vec2 p1, Color c, const RectF& clip);

	// Add DrawCircle template function to use any kind of vector
	template<typename T>
	inline void DrawCircle(_Vec2<T> center, int radius, Color c)
	{
		DrawCircle((int)center.x, (int)center.y, radius, c);
	}
	void DrawCircle( int centerX,int centerY,int radius,Color c );
	void DrawTriangle(Vec2 v0, Vec2 v1, Vec2 v2, const RectI& clip, Color c);
	//void TransBlt(const Surface& surf, const Mat3& trans);
	void DrawTriangleTex(Vertex v0, Vertex v1, Vertex v2, const RectI& clip,const Surface& text);

	void BeginFrame();
	void EndFrame();

	// This is the last method that we will call before put the pixels in the screen
	// the target object Drawable will call rasterize with this D3DGraphics
	virtual void Draw(Drawable& object) override
	{		
		object.Rasterize(*this);
	}

private:	

	void DrawFlatTopTriangleTex(Vertex v0, Vertex v1, Vertex v2, const RectI& clip, const Surface& text);
	void DrawFlatBottomTriangleTex(Vertex v0, Vertex v1, Vertex v2, const RectI& clip, const Surface& text);
	// Draw a triangle with the bottom or the top flat (y0 == y1)
	// yStart, yEnd: points in the triangle between the flat part and the other point
	// m0, m1 = slope
	void DrawFlatTriangle(float y0, float y1, float m0, float b0, float m1, float b1, const RectI& clip, Color c);

public:
	static const unsigned int	SCREENWIDTH =	800;
	static const unsigned int	SCREENHEIGHT =	600;
private:
	const Color		FILLVALUE =		BLACK;
	IDirect3D9*			pDirect3D;
	IDirect3DDevice9*	pDevice;
	IDirect3DSurface9*	pBackBuffer;
	Surface			    sysBuffer;

};