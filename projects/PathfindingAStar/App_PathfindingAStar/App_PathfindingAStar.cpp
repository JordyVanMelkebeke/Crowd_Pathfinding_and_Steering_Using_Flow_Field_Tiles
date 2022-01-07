//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_PathfindingAStar.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAstar.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EBFS.h"

using namespace Elite;

//Destructor
App_PathfindingAStar::~App_PathfindingAStar()
{
	SAFE_DELETE(m_pGridGraph);
}

//Functions
void App_PathfindingAStar::Start()
{
	//Set Camera
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(39.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(73.0f, 35.0f));

	//Create Graph
	MakeGridGraph();

	//Setup default start path
	startPathIdx = 44;
	endPathIdx = 88;
	CalculatePath();
}

void App_PathfindingAStar::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	//INPUT
	bool const middleMousePressed = INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eMiddle);
	if (middleMousePressed)
	{
		MouseData mouseData = { INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eMiddle) };
		Elite::Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseData.X, (float)mouseData.Y });

		//Find closest node to click pos
		int closestNode = m_pGridGraph->GetNodeIdxAtWorldPos(mousePos);
		if (m_StartSelected)
		{
			startPathIdx = closestNode;
			CalculatePath();
		}
		else
		{
			endPathIdx = closestNode;
			CalculatePath();
		}
	}
	//IMGUI
	UpdateImGui();

	//UPDATE/CHECK GRID HAS CHANGED
	if (m_GraphEditor.UpdateGraph(m_pGridGraph))
	{
		CalculatePath();
	}
}

void App_PathfindingAStar::Render(float deltaTime) const
{
	UNREFERENCED_PARAMETER(deltaTime);
	//Render grid
	m_GraphRenderer.RenderGraph(
		m_pGridGraph,
		m_bDrawGrid,
		m_bDrawNodeNumbers,
		m_bDrawConnections,
		m_bDrawConnectionsCosts
	);

	//Render end node on top if applicable
	if (endPathIdx != invalid_node_index)
	{
		m_GraphRenderer.HighlightNodes(m_pGridGraph, { m_pGridGraph->GetNode(endPathIdx) }, END_NODE_COLOR);
	}
}

void App_PathfindingAStar::MakeGridGraph()
{
	m_pGridGraph = new GridGraph<GridTerrainNode, GraphConnection>(COLUMNS, ROWS, m_SizeCell, false, false, 1.f, 1.5f);

	//Setup default terrain
	m_pGridGraph->GetNode(86)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(66)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(67)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(47)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(86);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(66);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(67);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(47);

	m_pGridGraph->GetNode(187)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(188)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(189)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(190)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(191)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(187);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(188);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(189);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(190);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(191);

	m_pGridGraph->GetNode(162)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(163)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(183)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(203)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(223)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(162);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(163);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(183);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(203);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(223);

	m_pGridGraph->GetNode(293)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(273)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(253)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(233)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(213)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(193)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(293);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(273);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(253);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(233);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(213);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(193);

	m_pGridGraph->GetNode(53)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(73)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(74)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(75)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(76)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(53);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(73);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(74);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(75);
	m_pGridGraph->RemoveConnectionsToAdjacentNodes(76);
}

void App_PathfindingAStar::UpdateImGui()
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		int menuWidth = 115;
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
		ImGui::Text("MMB: target");

		ImGui::Unindent();

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Text("Pathfinding and steering using flow field tiles");
		ImGui::Spacing();

		std::string buttonText{ "" };


		ImGui::Checkbox("NodeNumbers", &m_bDrawNodeNumbers);
		ImGui::Checkbox("Connections", &m_bDrawConnections);
		ImGui::Checkbox("Connections Costs", &m_bDrawConnectionsCosts);

		ImGui::Spacing();

		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif
}

void App_PathfindingAStar::CalculatePath()
{

}