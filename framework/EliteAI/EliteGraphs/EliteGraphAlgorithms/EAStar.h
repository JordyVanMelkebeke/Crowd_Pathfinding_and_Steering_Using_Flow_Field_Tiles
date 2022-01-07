#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
		vector<T_NodeType*> path;
		vector<NodeRecord> openList;
		vector<NodeRecord> closedList;
		NodeRecord currentRecord;


		NodeRecord startRecord{};
		startRecord.pNode = pStartNode;
		startRecord.pConnection = nullptr;
		startRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);
		openList.push_back(startRecord);

		while (!openList.empty())
		{

			//A Get connection with lowest F score
			currentRecord = *std::min_element(openList.begin(), openList.end(), [](NodeRecord A, NodeRecord B) {return A < B; });

			//B Check if that connection leads to the end node
			if (currentRecord.pConnection)
			{
				if (currentRecord.pConnection->GetTo() == pGoalNode->GetIndex())
					break;
			}
			//C Else, we get all the connections of the NodeRecord’s node
			/*else
			{*/
			std::list<T_ConnectionType*> connections{ m_pGraph->GetNodeConnections(currentRecord.pNode->GetIndex()) };

			for (auto connection : connections)
			{

				float totalGCost = connection->GetCost() + currentRecord.costSoFar;

				auto nodeInClosedList{ std::find_if(closedList.begin(), closedList.end(), [&connection](NodeRecord A) {return A.pNode->GetIndex() == connection->GetTo(); }) };
				auto nodeInOpenList{ std::find_if(openList.begin(), openList.end(), [&connection](NodeRecord A) {return A.pNode->GetIndex() == connection->GetTo(); }) };

				if (nodeInClosedList != closedList.end())
				{
					if (nodeInClosedList->costSoFar <= totalGCost)
						continue;
					else
						closedList.erase(std::remove(closedList.begin(), closedList.end(), *nodeInClosedList), closedList.end());
				}
				else if (nodeInOpenList != openList.end())
				{
					if (nodeInOpenList->costSoFar <= totalGCost)
						continue;
					else
						openList.erase(std::remove(openList.begin(), openList.end(), *nodeInOpenList), openList.end());
				}

				NodeRecord newRecord{};
				newRecord.pConnection = connection;
				newRecord.pNode = m_pGraph->GetNode(connection->GetTo());
				newRecord.costSoFar = totalGCost;
				newRecord.estimatedTotalCost = GetHeuristicCost(newRecord.pNode, pGoalNode) + totalGCost;
				openList.push_back(newRecord);

			}

			//G Remove NodeRecord from the openList and add it to the closedList
			openList.erase(std::remove(openList.begin(), openList.end(), currentRecord), openList.end());
			closedList.push_back(currentRecord);
		}

		//}

		//Reconstruct path from last connection to start node

		while (currentRecord.pNode != pStartNode)
		{
			path.push_back(currentRecord.pNode);
			currentRecord = *std::find_if(closedList.begin(), closedList.end(), [&currentRecord](NodeRecord A)
				{ return A.pNode->GetIndex() == currentRecord.pConnection->GetFrom(); });
		}
		path.push_back(pStartNode);
		std::reverse(path.begin(), path.end());
		return path;
	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}