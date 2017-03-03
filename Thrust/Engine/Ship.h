#pragma once

#include "PolyClosed.h"
#include "Camera.h"
#include "CollidableCircle.h"

class Ship: public CollidableCircle
{
public:

	// Internal class to represent a Drawable object inside polyclosed
	class Drawable : public ::Drawable
	{
	public:
		Drawable(const Ship& parent)
			:parent(parent)
		{			
			// Transform matrix from model space (this model) into world space
			Transform(Mat3::Traslation(parent.pos) * Mat3::Rotation(parent.angle));
		}
		virtual void Rasterize(D3DGraphics & gfx) const override
		{			
			PolyClosed::Drawable drawableM = parent.model.GetDrawable();
			drawableM.Transform(trans);
			gfx.Draw(drawableM);

			// Transform the shield position from model position to world position
			// The center of the shield will be the center of the model (local position = 0,0)
			const Vec2 shieldCenter = trans * Vec2{ 0.0f,0.0f };
			gfx.DrawCircle(shieldCenter, parent.shieldRadius, parent.shieldColor);			
		}
	private:
		const Ship& parent;
	};


	Ship(std::string filename, Vec2 pos = { 0.0f,0.0f })
		:pos (pos),
		model( filename )
	{}

	Drawable GetDrawable() const
	{
		return Drawable(*this);
	}

	// Update has deltaTime to avoid use all the math operations according at the frame rate (frame speed)
	// The frame rate depends on the monitor, with deltaTime (seconds) 
	void Update(float deltaTime)
	{
		// angular (1th order than 0th order)
		// When angAccelDir is 0 StopSpinning smothly
		if (angAccelDir == 0.0f)
		{
			if (abs(angVel) <= (angAccel* deltaTime))
			{
				angVel = 0.0f;
			}
			else
			{
				// To make stop spinning, we substract angVel, the signed of angVel and the value of angAccel
				float sign = copysign(angAccel, angVel);
				angVel -= (sign * deltaTime);
			}
		}
		angVel += angAccel  * angAccelDir * deltaTime;
		angle += angVel * deltaTime;

		// Clamp angle to within 2.0fPI to avoid having large angles (2PI = 360º)
		// (Large angles == bad performance and weird behaviour)
		angle = fmodf(angle, 2.0f * PI);

		// linear (1th order than 0th order)
		vel += Vec2{ 0.0f, -1.0f }.Rotation(angle) * accel * thrust  * deltaTime;
		pos += vel  * deltaTime;
	}

	void FocusOn(Camera& cam) const
	{
		// Move camera where the ship is
		cam.MoveTo(pos);
	}

	// Control functions
	void Thurst()
	{
		thrust = 1.0f;
	}

	void StopThrusting()
	{
		thrust = 0.0f;
	}

	void Spin(float direction)
	{
		angAccelDir = direction;
	}

	void StopSpinning(float direction)
	{
		
		if (angAccelDir == direction)
		{
			angAccelDir = 0.0f;
		}		
	}

	// Collidible interface
	virtual RectF GetAABB() const override
	{
		return RectF(pos.y - shieldRadius, pos.y + shieldRadius, pos.x - shieldRadius, pos.x + shieldRadius);
	}
	virtual Vec2 GetVel() const override
	{
		return vel;
	}
	virtual Vec2 GetCenter() const override
	{
		return pos;
	}
	virtual float GetRadius() const override
	{
		return (float)shieldRadius;
	}
	virtual void Rebound(Vec2 normal) override
	{
		// The rebound velocity will be the projection of the vector in the normal velocity
		// Applying the next formula
		//vel -= normal * (vel * normal) * 2.0f;
		// 1º dot product between vel & normal (vel * normal) to get the projection
		// of vel vector into normal vector
		// 2ª We double that vector (vel * normal) * 2.0f
		float dotP = (vel * normal) * 2.0f;
		
		// 3º We multiply that for vector normal again to get the float 
		Vec2 reflectVel = normal * dotP;

		// 4º Subtrack that reflect vector into the velocity vector
		vel -= reflectVel;		
	}

private:

	// Structure
	PolyClosed model;

	// Ship Shield
	const int shieldRadius = 45;
	const Color shieldColor = GREEN;


	//linear movement
	Vec2 pos;
	Vec2 vel = { 0.0f,0.0f };
	const float accel = 0.2f * 60.0f * 60.0f;
	float thrust = 0.0f;

	//angular movement
	float angle = 0.0f;
	float angVel = 0.0f;
	const float angAccel = 0.002f* 60.0f * 60.0f;
	// 1.0 is going to be clockwise, -1.0 anticlockwise
	float angAccelDir = 0.0f;
};