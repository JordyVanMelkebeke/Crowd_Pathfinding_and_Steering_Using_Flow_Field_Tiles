//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../Obstacle.h"
#include "framework\EliteMath\EMatrix2x3.h"


using namespace Elite;

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	//variabelen
	float const arivalRadius = 1.f;


	Vector2 toTarget = m_Target.Position - pAgent->GetPosition();
	float const distance = toTarget.Magnitude();

	//calc distance to target
	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();

	if (distance < arivalRadius)
	{
		steering.LinearVelocity = ZeroVector2;
	}
	else
	{
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	}


	//debug rendering

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 1,0,0,0.5f },0.4f);
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), pAgent->GetLinearVelocity(), 5.f, { 0,1,0,0.5f }, 0.4f);
	}

	return steering;
}

//FLEE
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	auto distanceToTarget = Distance(pAgent->GetPosition(), m_Target.Position);
	if (distanceToTarget > m_FleeRadius)
	{
		return SteeringOutput(ZeroVector2, 0.f, false);
	}
	else
	{
		//calc distance to target
		steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
		steering.LinearVelocity.Normalize();
		steering.LinearVelocity *= -pAgent->GetMaxLinearSpeed();
	}

	//debug rendering
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 1,0,0,0.5f }, 0.4f);
		//DEBUGRENDERER2D->DrawSegment()
	}

	return steering;
}

//Arrive
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	//variabelen
	float const arivalRadius = 1.f;
	float const slowdownRadius = 15.f;

	//calc distance to target
	Vector2 toTarget = m_Target.Position - pAgent->GetPosition();
	float const distance = toTarget.Magnitude();

	//calc distance to target
	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();

	if (distance < arivalRadius)
	{
		steering.LinearVelocity = ZeroVector2;
	}
	else if (distance < slowdownRadius)
	{
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed() * distance / slowdownRadius;
	}
	else
	{
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	}


	//debug rendering
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 1,0,0,0.5f }, 0.4f);
		DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), 7.5f, { 0,1,0,0.5f }, 0.45f);
	}

	return steering;
}

//Face
SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	float angle{ pAgent->GetRotation() };

	//Get current rotation
	const float curentAngle{ pAgent->GetRotation() };
	//Get vector to target
	const Vector2 targetVector = m_Target.Position - pAgent->GetPosition();
	//Get radius based on targetvector
	const float radius{ targetVector.Magnitude() };
	//Get the vector where the agent is looking
	const Vector2 viewVector{ cosf(curentAngle) * radius, sinf(curentAngle) * radius };
	//Get angle
	const float setAngle{ AngleBetween(targetVector,viewVector) + curentAngle };

	if (angle > setAngle)
	{
		pAgent->SetRotation(++angle);
	}
	else if (angle < setAngle)
	{
		pAgent->SetRotation(++angle);
	}

	
	pAgent->SetLinearVelocity(ZeroVector2);
	steering.LinearVelocity = targetVector;

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), pAgent->GetLinearVelocity(), 5.f, { 0,1,0,0.5f }, 0.4f);
	}

	return steering;
}

SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	Vector2 distanceToTarget = m_Target.Position - pAgent->GetPosition();


	Vector2 displacement = { 0,m_Radius };

	SetVectorRot(displacement, m_WanderAngle);

	//m_WanderAngle += (Elite::randomFloat(-m_MaxAngleChange, m_MaxAngleChange));
	m_WanderAngle += Elite::randomFloat(m_MaxAngleChange) - (m_MaxAngleChange * 0.5f);

	Vector2 circleCenter;
	circleCenter = pAgent->GetPosition() + (pAgent->GetDirection().GetNormalized() * (m_Radius + m_OffsetDistance));

	 
	Vector2 futurePos;
	futurePos = circleCenter + displacement - pAgent->GetPosition();
	futurePos.Normalize();
	futurePos *= pAgent->GetMaxLinearSpeed();
	m_Target = pAgent->GetPosition() + futurePos;

	steering.LinearVelocity = futurePos;

	
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 1,0,0,0.5f }, 0.4f);
		DEBUGRENDERER2D->DrawPoint(circleCenter + displacement, 2.5f, { 0,1,0,0.5f }, 0.45f);
		DEBUGRENDERER2D->DrawCircle(circleCenter, m_Radius, { 1,0,0,0.5f }, 0.4f);
		DEBUGRENDERER2D->DrawDirection(circleCenter, displacement, m_Radius, { 0,0,1,0.5f }, 0.4f);
	}

	
	return Seek::CalculateSteering(deltaT,pAgent);
}

void Wander::SetVectorRot(Vector2& vector, float angle)
{
	vector.x = cos(atan2(vector.y, vector.x) + angle) * vector.Magnitude();
	vector.y = sin(atan2(vector.y, vector.x) + angle) * vector.Magnitude();
}

//Persuit
SteeringOutput Persuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2 distanceToTarget = m_Target.Position - pAgent->GetPosition();
	auto t = distanceToTarget.Magnitude() / pAgent->GetMaxLinearSpeed();

	Vector2 futurePos = m_Target.Position + m_Target.LinearVelocity * t;

	m_Target = futurePos;

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 1,0,0,0.5f }, 0.4f);
		DEBUGRENDERER2D->DrawPoint(m_Target.Position + m_Target.LinearVelocity*t, 2.f, {0,1,0,0.5f},0.45f);
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), pAgent->GetLinearVelocity(), 5.f, { 0,0,1,0.5f }, 0.4f);
	}

	return Seek::CalculateSteering(deltaT,pAgent);
}

//Evade
SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2 distanceToTarget = m_Target.Position - pAgent->GetPosition();
	auto t = distanceToTarget.Magnitude() / pAgent->GetMaxLinearSpeed();
	Vector2 futurePos = m_Target.Position + m_Target.LinearVelocity * t;

	m_Target = futurePos;

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 1,0,0,0.5f }, 0.4f);
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), pAgent->GetLinearVelocity(), 5.f, { 0,1,0,0.5f }, 0.4f);
	}


	return Flee::CalculateSteering(deltaT,pAgent);
}

