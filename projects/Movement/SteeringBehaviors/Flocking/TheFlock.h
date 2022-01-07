#pragma once
#include "../SteeringHelpers.h"
#include "FlockingSteeringBehaviors.h"
#include "../SpacePartitioning/SpacePartitioning.h"

class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;


//#define USE_SPACE_PARTITIONING

class Flock
{
public:
	Flock(
		int flockSize = 100, 
		float worldSize = 100.f, 
		SteeringAgent* pAgentToEvade = nullptr, 
		bool trimWorld = false);

	~Flock();

	void Update(float deltaT);
	void UpdateAndRenderUI() ;
	void Render(float deltaT);

#ifdef USE_SPACE_PARTITIONING
	const vector<SteeringAgent*>& GetNeighbors() const { return m_pPartitionedSpace->GetNeighbors(); }
	int GetNrOfNeighbors() const { return m_pPartitionedSpace->GetNrOfNeighbors(); }

#else // No space partitioning
	void RegisterNeighbors(SteeringAgent* pAgent);
	int GetNrOfNeighbors() const { return m_NrOfNeighbors; }
	const vector<SteeringAgent*>& GetNeighbors() const { return m_Neighbors; }
#endif //USE_SPACE_PARTITIONING

	Elite::Vector2 GetAverageNeighborPos() const;
	Elite::Vector2 GetAverageNeighborVelocity() const;

	void SetSeekTarget(TargetData target);
	void SetEvadeTarget();
	void SetWorldTrimSize(float size) { m_WorldSize = size; }


private:
	//Datamembers
	int m_FlockSize = 0;
	vector<SteeringAgent*> m_Agents;

#ifdef  USE_SPACE_PARTITIONING
	CellSpace* m_pPartitionedSpace = nullptr;
	Cell* m_pCell = nullptr;
	int m_NrOfCellsX{ 25 };

	float m_spaceWidth{0};
	float m_spaceHeight{ 0 };

	int m_NrOfRows{0};
	int m_NrOfCols{0};

	float m_CellWidth{0};
	float m_CellHeight{0};

	float m_cellBottom{0};
	float m_cellLeft{ 0 };

	vector<Elite::Vector2> m_OldPositions = {};

#else // No space partitioning
	vector<SteeringAgent*> m_Neighbors;
#endif  //USE_SPACE_PARTITIONING

	bool m_TrimWorld = false;
	float m_WorldSize = 0.f;

	float m_NeighborhoodRadius = 10.f;
	int m_NrOfNeighbors = 0;

	SteeringAgent* m_pAgentToEvade = nullptr;
	
	//Steering Behaviors
	Separation* m_pSeparationBehavior = nullptr;
	Cohesion* m_pCohesionBehavior = nullptr;
	VelocityMatch* m_pVelMatchBehavior = nullptr;
	Seek* m_pSeekBehavior = nullptr;
	Wander* m_pWanderBehavior = nullptr;
	Evade* m_pEvadeBehavior = nullptr;

	BlendedSteering* m_pBlendedSteering = nullptr;
	PrioritySteering* m_pPrioritySteering = nullptr;

	float* GetWeight(ISteeringBehavior* pBehaviour);

	bool m_ShowNeigborhood = true;
	bool m_ShowSteering = false;
	bool m_SpacialPartitioning = true;

private:
	Flock(const Flock& other);
	Flock& operator=(const Flock& other);
};