#include "stdafx.h"
#include "TheFlock.h"

#include "../SteeringAgent.h"
#include "../Steering/SteeringBehaviors.h"
#include "../CombinedSteering/CombinedSteeringBehaviors.h"

//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/, 
	float worldSize /*= 100.f*/, 
	SteeringAgent* pAgentToEvade /*= nullptr*/, 
	bool trimWorld /*= false*/)

	: m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld { trimWorld }
	, m_pAgentToEvade{pAgentToEvade}
	, m_NeighborhoodRadius{ 10 }
	, m_NrOfNeighbors{0}
	/*, m_spaceWidth{worldSize}
	, m_spaceHeight{worldSize}
	, m_cellBottom{0}
	, m_cellLeft{0}*/
	, m_ShowNeigborhood{true}
	, m_ShowSteering{false}
	, m_SpacialPartitioning{true}
{
	m_Agents.resize(m_FlockSize);

	// DONE: initialize the flock and the memory pool

	m_pSeparationBehavior = new Separation(this);
	m_pCohesionBehavior = new Cohesion(this);
	m_pVelMatchBehavior = new VelocityMatch(this);
	m_pSeekBehavior = new Seek;
	m_pWanderBehavior = new Wander;
	m_pEvadeBehavior = new Evade;

	vector<BlendedSteering::WeightedBehavior> weightedSteeringBehaviors;
	weightedSteeringBehaviors.push_back({ m_pSeparationBehavior, 0.2f });
	weightedSteeringBehaviors.push_back({ m_pCohesionBehavior, 0.2f });
	weightedSteeringBehaviors.push_back({ m_pVelMatchBehavior, 0.2f });
	weightedSteeringBehaviors.push_back({ m_pSeekBehavior, 0.2f });
	weightedSteeringBehaviors.push_back({ m_pWanderBehavior, 0.2f });
	m_pBlendedSteering = new BlendedSteering(weightedSteeringBehaviors);
	m_pPrioritySteering = new PrioritySteering({ m_pEvadeBehavior,m_pBlendedSteering});

	for (int i = 0; i < m_FlockSize; i++)
	{
		m_Agents[i] = new SteeringAgent();
		m_Agents[i]->SetPosition(Elite::randomVector2(0, worldSize));
		m_Agents[i]->SetSteeringBehavior(m_pPrioritySteering);
		m_Agents[i]->SetMass(1.0f);
		m_Agents[i]->SetMaxAngularSpeed(25.f);
		m_Agents[i]->SetMaxLinearSpeed(55.f);
		m_Agents[i]->SetAutoOrient(true);
	}
	m_pAgentToEvade = new SteeringAgent();
	m_pAgentToEvade->SetAutoOrient(true);
	m_pAgentToEvade->SetSteeringBehavior(m_pWanderBehavior);
	m_pAgentToEvade->SetBodyColor({ 1,0,0 });
	m_pAgentToEvade->SetMaxLinearSpeed(55.f);
	m_pAgentToEvade->SetMaxAngularSpeed(25.f);
	m_pAgentToEvade->SetMass(1.0f);

	//Spacial Partitioning
	

	

	
}

Flock::~Flock()
{
	// TODO: clean up any additional data

	SAFE_DELETE(m_pSeparationBehavior);
	SAFE_DELETE(m_pCohesionBehavior);
	SAFE_DELETE(m_pVelMatchBehavior);
	SAFE_DELETE(m_pEvadeBehavior);
	SAFE_DELETE(m_pWanderBehavior);
	SAFE_DELETE(m_pSeekBehavior);

	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pPrioritySteering);

	for (auto pAgent : m_Agents)
	{
		SAFE_DELETE(pAgent);
	}
	m_Agents.clear();

	SAFE_DELETE(m_pAgentToEvade);

	//SAFE_DELETE(m_pPartitionedSpace);
	//SAFE_DELETE(m_pCell);
}

void Flock::Update(float deltaT)
{
	// DONE: update the flock
	// loop over all the agents
		// register its neighbors	(-> memory pool is filled with neighbors of the currently evaluated agent)
		// update it				(-> the behaviors can use the neighbors stored in the pool, next iteration they will be the next agent's neighbors)
		// trim it to the world

	

	for (int i = 0; i < m_FlockSize; i++)
	{
		m_Neighbors.clear();
		m_NrOfNeighbors = 0;
		for (int j = 0; j < m_FlockSize; j++)
		{
			if (m_Agents[i] == m_Agents[j])
			{
				continue;
			}
			if (Elite::DistanceSquared(m_Agents[i]->GetPosition(),m_Agents[j]->GetPosition()) <= m_NeighborhoodRadius*m_NeighborhoodRadius)
			{
				RegisterNeighbors(m_Agents[j]);
			}

		}
		m_Agents[i]->Update(deltaT);
		m_Agents[i]->TrimToWorld({ 0,0 }, { m_WorldSize,m_WorldSize });
	}

	m_pAgentToEvade->Update(deltaT);
	m_pAgentToEvade->TrimToWorld({ 0,0 }, { m_WorldSize,m_WorldSize });
}

void Flock::Render(float deltaT)
{
	//DONE: render the flock
	for (auto pAgent : m_Agents)
	{
		pAgent->Render(deltaT);
	}

	
	
	if (m_ShowNeigborhood)
	{
		DEBUGRENDERER2D->DrawCircle(m_Agents.back()->GetPosition(), m_NeighborhoodRadius, Elite::Color(1, 1, 1), 0.4f);
		for (auto* pNeighbors : m_Neighbors)
		{
			DEBUGRENDERER2D->DrawSolidCircle(pNeighbors->GetPosition(), 1, {}, Elite::Color(0, 1, 0), 0.3f);
		}

		m_Agents.back()->SetRenderBehavior(true);
	}
	else
	{
		m_Agents.back()->SetRenderBehavior(false);
	}

	if (m_ShowSteering)
	{
		for (int i = 0; i < m_FlockSize-1; i++)
		{
			m_Agents[i]->SetRenderBehavior(true);
		}
	}
	else
	{
		for (int i = 0; i < m_FlockSize-1; i++)
		{
			m_Agents[i]->SetRenderBehavior(false);
		}
	}

	m_pAgentToEvade->Render(deltaT);

	if (m_SpacialPartitioning)
	{
		//m_pPartitionedSpace->RenderCells();
	}

	
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Flocking");
	ImGui::Spacing();

	// TODO: Implement checkboxes for debug rendering and weight sliders here
	
	ImGui::Checkbox("Debug render steering", &m_ShowSteering);
	ImGui::Spacing();
	ImGui::Checkbox("Debug render neighborhood", &m_ShowNeigborhood);
	ImGui::Spacing();
	ImGui::Checkbox("Debug render partitions", &m_SpacialPartitioning);
	ImGui::Spacing();
	ImGui::SliderFloat("Separation", &m_pBlendedSteering->GetWeightedBehaviorsRef()[0].weight, 0.f, 1.f, "%.2");
	ImGui::Spacing();
	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->GetWeightedBehaviorsRef()[1].weight, 0.f, 1.f, "%.2");
	ImGui::Spacing();
	ImGui::SliderFloat("Velocity Match", &m_pBlendedSteering->GetWeightedBehaviorsRef()[2].weight, 0.f, 1.f, "%.2");
	ImGui::Spacing();
	ImGui::SliderFloat("Seek", &m_pBlendedSteering->GetWeightedBehaviorsRef()[3].weight, 0.f, 1.f, "%.2");
	ImGui::Spacing();
	ImGui::SliderFloat("Wander", &m_pBlendedSteering->GetWeightedBehaviorsRef()[4].weight, 0.f, 1.f, "%.2");
	ImGui::Spacing();

	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
	
}

void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	m_Neighbors.push_back(pAgent);
	++m_NrOfNeighbors;

}

Elite::Vector2 Flock::GetAverageNeighborPos() const
{
	Elite::Vector2 avaragePos;

	for (int i = 0; i < m_NrOfNeighbors; i++)
	{
		avaragePos += m_Agents[i]->GetPosition();
	}
	avaragePos.x /= m_NrOfNeighbors;
	avaragePos.y /= m_NrOfNeighbors;
	return avaragePos;
}

Elite::Vector2 Flock::GetAverageNeighborVelocity() const
{
	Elite::Vector2 avarageVel;

	for (int i = 0; i < m_NrOfNeighbors; i++)
	{
		avarageVel += m_Agents[i]->GetLinearVelocity();
	}
	avarageVel.x /= m_NrOfNeighbors;
	avarageVel.y /= m_NrOfNeighbors;
	return avarageVel;
}

void Flock::SetSeekTarget(TargetData target)
{
	// DONE: set target for Seek behavior
	m_pSeekBehavior->SetTarget(target);
}

void Flock::SetEvadeTarget()
{
	m_pEvadeBehavior->SetTarget(m_pAgentToEvade->GetPosition());
}

float* Flock::GetWeight(ISteeringBehavior* pBehavior) 
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->GetWeightedBehaviorsRef();
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}
