/****************************************************************************************** 
 *	Chili DirectX Framework Version 14.03.22											  *	
 *	Game.cpp																			  *
 *	Copyright 2014 PlanetChili.net <http://www.planetchili.net>							  *
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
#include "Game.h"
#include "Mat3.h"
#include "TriangleStrip.h"
#include <array>


Game::Game( HWND hWnd,KeyboardServer& kServer,MouseServer& mServer )
:	gfx( hWnd ),
	audio( hWnd ),
	kbd( kServer ),
	mouse( mServer ),
	view(gfx, {0, D3DGraphics::SCREENHEIGHT-1,0, D3DGraphics::SCREENWIDTH-1} ),
	cam(view,view.GetWidth(), view.GetHeight() ),
	//ship("shipd.dxf",{D3DGraphics::SCREENWIDTH / 2.0f, D3DGraphics::SCREENHEIGHT / 2.0f}),
	ship("shipd.dxf", { -2026.0f, 226.0f }),
	map("map.dxf")
{
}

Game::~Game()
{
}

void Game::Go()
{
	/*Mat3 m = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	Mat3 b = { 1, 0, 0, 0, 2, 0, 0, 0, 3 };
	Mat3 c = m * b;*/

	HandleInput();
	UpdateModel();
	

	gfx.BeginFrame();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel( )
{
// Depends on Debug or release we choose delta time according to the monitor, or fake deltatime
#if NDEBUG
	
	const float deltaTime = timer.GetTimerSec();
	timer.StartWatch();
#else
	const float deltaTime = 1.0f / 60.0f;
#endif

	ship.Update(deltaTime);
	map.HandleCollision(ship);
}

void Game::HandleInput()
{
	// Handle keyboard to move the ship
	const KeyEvent key = kbd.ReadKey();
	switch (key.GetCode())
	{
		case VK_LEFT:
			if (key.IsPress())
			{
				ship.Spin(1.0f);
			}
			else
			{
				ship.StopSpinning(1.0f);
			}
			break;
		case VK_RIGHT:
			if (key.IsPress())
			{
				ship.Spin(-1.0f);
			}
			else
			{
				ship.StopSpinning(-1.0f);
			}
			break;
		case VK_SPACE:
			if (key.IsPress())
			{
				ship.Thurst();
			}
			else
			{
				ship.StopThrusting();
			}
		break;
	}

	// Mouse handle
	//angle -= 0.1f;
	switch (mouse.ReadMouse().GetType())
	{
	case MouseEvent::WheelUp:
	scale *= 1.13;
	break;

	case MouseEvent::WheelDown:
	scale /= 1.13;
	break;
	}

}

void Game::ComposeFrame()
{

	// Ship game
	// Focus the ship (move the camera where the ship is)
	ship.FocusOn(cam);

	// Draw camera
	cam.Draw(map.GetDrawable());
	cam.Draw(ship.GetDrawable());



	/*PolyClosed::Drawable d = model.GetDrawable();
	d.Transform(Mat3::Traslation({ (float)mouse.GetMouseX(), (float)mouse.GetMouseY() }) *
		Mat3::Rotation(angle) * Mat3::Scaling(scale));
	cam.Draw(d);*/
	//model.Draw({ (float) mouse.GetMouseX(), (float) mouse.GetMouseY() }, angle, scale, gfx);	


	// Test triangle
	/*Vec2 m = { (float)mouse.GetMouseX(), (float)mouse.GetMouseY()};
	Vec2 v0 = { 50.0f,50.0f };
	Vec2 v1 = { 750.0f,50.0f };
	Vec2 v2 = { 50.0f,550.0f };
	Vec2 v3 = { 750.0f,550.0f };
	
	RectI clip = { 0, D3DGraphics::SCREENHEIGHT - 1,0, D3DGraphics::SCREENWIDTH - 1 };
	gfx.DrawTriangle(v0, m, v2, clip, BLUE);
	gfx.DrawTriangle(v0, v1, m, clip, RED);
	gfx.DrawTriangle(v1, v3, m, clip, GREEN);
	gfx.DrawTriangle(v2, v3, m, clip, PURPLE);*/

	


	// Test triangle list
	/*TriangleStrip strip({ {0.0f, 0.0f}, {100.0f,0.0f}, {50.0f, 50.0f}, {200.0f,200.0f}, {100.0f,400.0f} });
	view.Draw(strip.GetDrawable());*/
	/*PolyClosed p({ {50.0f,50.0f}, {100.0f,50.0f}, {100.0f,100.0f }, {50.0f,100.0f} });
	TriangleStrip strip(p.ExtractStripVertices(10.0f));
	view.Draw(strip.GetDrawable());*/

	/*const float toRadians = 2 * PI;
	const Mat3 trans = Mat3::Traslation({ 400.0f, 300.0f }) * Mat3::Scaling(scale) * 
		Mat3::Rotation(toRadians * (float(mouse.GetMouseX()) / 800.0f));

	const unsigned int numberVertex = 4;
	std::array<Vertex, numberVertex> quad;
	quad[0].v = { -80.0f, -135.0f };
	quad[0].t = { 0.0f,0.0f };

	quad[1].v = { 79.0f, -135.0f };
	quad[1].t = { 159.0f,0.0f };

	quad[2].v = { 79.0f,  134.0f };
	quad[2].t = { 159.0f,269.0f };

	quad[3].v = { -80.0f, 134.0f };
	quad[3].t = { 0.0f, 269.0f };

	std::array<Vertex, numberVertex> quadTrans;
	for (int i = 0; i < numberVertex; i++)
	{
		quadTrans[i].t= quad[i].t;
		quadTrans[i].v = trans * quad[i].v;
	}

	gfx.DrawTriangleTex(quadTrans[0], quadTrans[1], quadTrans[3], 
		{0,D3DGraphics::SCREENHEIGHT -1 , 0,D3DGraphics::SCREENWIDTH -1 }, image);

	gfx.DrawTriangleTex(quadTrans[1], quadTrans[2], quadTrans[3],
	{ 0,D3DGraphics::SCREENHEIGHT - 1 , 0,D3DGraphics::SCREENWIDTH - 1 }, image);
	*/

	//gfx.TransBlt(image, trans);

}
