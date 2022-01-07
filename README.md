# Crowd_Pathfinding_and_Steering_Using_Flow_Field_Tiles
**Introduction**

Crowd pathfinding and steering using flow field tiles is a technique that is used to solve the computational problemswhen moving a large amount of agents on a map (ex: Crowds in planet Coaster).
Through the use of these dynamic flow field tiles a more modern steering pipeline can be achieved.
This can be achieved without a heavy load on the CPU because it does not have to compute every individual path per agent.
Agents wil also move more instantly despite the complexity of the path, giving the AI and user immediate feedback.
A paper made by ElijahEmerson goes deep into the fundamentals of this technique and i will use this as a base for implementation in a pathfinding project
