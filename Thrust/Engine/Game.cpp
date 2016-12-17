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

Game::Game( HWND hWnd,KeyboardServer& kServer,MouseServer& mServer )
:	gfx( hWnd ),
	audio( hWnd ),
	kbd( kServer ),
	mouse( mServer ),
	view(gfx, {0.0f, D3DGraphics::SCREENHEIGHT-1,0.0f, D3DGraphics::SCREENWIDTH-1} ),
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
	/*angle -= 0.1f;
	switch (mouse.ReadMouse().GetType())
	{
	case MouseEvent::WheelUp:
	scale *= 1.13;
	break;

	case MouseEvent::WheelDown:
	scale /= 1.13;
	break;
	}*/

}

void Game::ComposeFrame()
{
	/*PolyClosed::Drawable d = model.GetDrawable();
	d.Transform(Mat3::Traslation({ (float)mouse.GetMouseX(), (float)mouse.GetMouseY() }) *
		Mat3::Rotation(angle) * Mat3::Scaling(scale));
	cam.Draw(d);*/
	//model.Draw({ (float) mouse.GetMouseX(), (float) mouse.GetMouseY() }, angle, scale, gfx);
	
	


	// Test the triangle (top flat)
	Vec2 v0 = { 100.0f, 100.0f };
	Vec2 v1 = { 250.0f, 100.0f };
	Vec2 v2 = { 50.0f, 300.0f };
	// Slopes
	float m0 = (v0.x - v2.x) / (v0.y - v2.y);
	float m1 = (v1.x - v2.x) / (v1.y - v2.y);

	// intersects (b)
	float b0 = v2.x - (m0 * v2.y);
	float b1 = v2.x - (m1 * v2.y);

	//gfx.DrawFlatTriangle(v0.y, v2.y, m0, b0, m1, b1, BLUE);


	// Focus the ship (move the camera where the ship is)
	/*ship.FocusOn(cam);

	// Draw camera
	cam.Draw(map.GetDrawable());
	cam.Draw(ship.GetDrawable());	*/
}
