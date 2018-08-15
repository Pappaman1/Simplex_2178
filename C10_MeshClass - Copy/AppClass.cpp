#include "AppClass.h"
#include "Heap.h"
int startValue = 99;
int endValue = 98;
float iValueStart;
float jValueStart;
float iValueEnd;
float jValueEnd;
Heap<int, int> a;
std::vector<int> backwards;
int currentPosition;
void Application::InitVariables(void)
{
	//Make MyMesh object
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(1.0f, C_BROWN);

	//Make MyMesh object
	m_pMesh1 = new MyMesh();
	m_pMesh1->GenerateCube(1.0f, C_GREEN);

	m_pMesh2 = new MyMesh();
	m_pMesh2->GenerateCube(1.0f, C_RED);

	m_pMesh3 = new MyMesh();
	m_pMesh3->GenerateCube(1.0f, C_YELLOW);

	int count = 0;
	int data[25];
	int goal;
	int start;

	//Read map
	std::fstream file("map.txt", std::ios_base::in);

	//check to make sure the file is closed, if it is, return.
	if (!file.is_open())
	{
		return;
	} 

	for (int i = 0; i < 5; i++)	//rownum
	{
		for (int j = 0; j < 5; j++)//colnum
		{
			int index = i * 5 + j;	//row times 5 plus j 
			file >> data[index];

			if (data[index] == 0) 
			{
				count++;
				posVec.push_back(glm::vec3((float)j, (float)i, 0.0f)); //push back the position of the cubes into posVec
			}
			
			if (data[index] == startValue)
			{
				//store values for later use
				start = index; 
				iValueStart = i;
				jValueStart = j;
			}
		

			if (data[index] == endValue)
			{
				//store values for later use
				iValueEnd = i;
				jValueEnd = j;
				goal = index;
			}
		}
	}
	file.close();

	//initialize search data table
	bool visited[25] = { false };
	unsigned int g[25] = {}; //distance
	int trace[25];
	for (int i = 0; i < 25; i++)
	{
		g[i] = -1;

	}

	//init h
	unsigned int h[25] = {};
	for (int i = 0; i < 25; i++)
	{
		int thisCol, goalCol, thisRow, goalRow;
		thisCol = i % 5;
		goalCol = jValueEnd;
		thisRow = i / 5;
		goalRow = iValueEnd;
		// manhattan heuristic
		h[i] = labs(thisCol - goalCol) + labs(thisRow - goalRow);//
	}

	g[start] = 0;
	trace[start] = -1;
	a.push(h[start] + g[start], start);

	while (a.getsize() != 0)
	{
		int nodeIndex = a.pop();
		if (visited[nodeIndex]) continue;
		visited[nodeIndex] = true;

	
		std::cout << std::endl;
		// update all neighbors
		int neighborIndex1 = nodeIndex + 5;
		int neighborIndex2 = nodeIndex - 5;
		int neighborIndex3 = nodeIndex + 1;
		int neighborIndex4 = nodeIndex - 1;

		if (neighborIndex1 >= 25)
		{
			neighborIndex1 -= 5;

		}
		if (neighborIndex2 < 0)
		{
			neighborIndex2 += 5;

		}
		if (neighborIndex3 < 0)
		{
			neighborIndex3 += 1;

		}
		if (neighborIndex4 >= 25)
		{
			neighborIndex4 -= 1;
		}
		//new distances between nodes
		int newDist1 = data[neighborIndex1] + g[nodeIndex];
		int newDist2 = data[neighborIndex2] + g[nodeIndex];
		int newDist3 = data[neighborIndex3] + g[nodeIndex];
		int newDist4 = data[neighborIndex4] + g[nodeIndex];

		//nodes check for walls and lowest weight path.
		if (newDist1 < g[neighborIndex1] && data[neighborIndex1] != 0)
		{
			g[neighborIndex1] = newDist1;
			trace[neighborIndex1] = nodeIndex;
			a.push(g[neighborIndex1] + h[neighborIndex1], neighborIndex1);
		}
		if (newDist2 < g[neighborIndex2] && data[neighborIndex2] != 0)
		{
			g[neighborIndex2] = newDist2;
			trace[neighborIndex2] = nodeIndex;
			a.push(g[neighborIndex2] + h[neighborIndex2], neighborIndex2);
		}
		if (newDist3 < g[neighborIndex3] && data[neighborIndex3] != 0)
		{
			g[neighborIndex3] = newDist3;
			trace[neighborIndex3] = nodeIndex;
			a.push(g[neighborIndex3] + h[neighborIndex3], neighborIndex3);
		}
		if (newDist4 < g[neighborIndex4] && data[neighborIndex4] != 0)
		{
			g[neighborIndex4] = newDist4;
			trace[neighborIndex4] = nodeIndex;
			a.push(g[neighborIndex4] + h[neighborIndex4], neighborIndex4);
		}

		if (nodeIndex == goal ) {
		
			bool done = false;
			int currentPos = goal;

			while (!done)
			{
				//keep track of path travelled
				backwards.push_back(currentPos);
				currentPos = trace[currentPos];

				if (currentPos == start)
				{
					done = true;
					backwards.push_back(currentPos);
					currentPosition = backwards.size() - 1;
				}
			}
			break;
		}
	}



}

void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	
	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();

	//Used to make the yellow block travel along the path every second.
	static DWORD startTime = GetTickCount(); //start timer
	DWORD currentTime = GetTickCount(); //current time
	float maxTime = 1.0f;
	float timeSinceStart = (currentTime - startTime) / 1000.0f; //self explanatory
	float mapValue = MapValue(timeSinceStart, 0.0f, maxTime, 0.0f, 1.0f);

	if (mapValue >= 0.99f)
	{
		startTime = GetTickCount();
		if (currentPosition > 0)
		{
			currentPosition -= 1;
		}
	}

	if (timeSinceStart > maxTime)
	{
		startTime = GetTickCount();
	}

	//m_pMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), ToMatrix4(m_qArcBall));
	m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3( jValueStart, iValueStart, 0.0f)));
	m_pMesh2->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(jValueEnd, iValueEnd, 0.0f)));
	int currentPosX = backwards[currentPosition] % 5; //convert the XY values into just an X value
	int currentPosY = backwards[currentPosition] / 5; //convert the XY values into just an Y value

	m_pMesh3->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(currentPosX, currentPosY, 0.0f)));


	matrix4 transformationMatrix; //name self explanatory 

	for (int i = 0; i < posVec.size(); i++)
	{
		//scale cubes
		transformationMatrix = glm::scale(matrix4(), glm::vec3(1, 1, 1));
		
		//translate every cube in posVec to the location at i
		transformationMatrix = glm::translate(transformationMatrix, posVec[i]);
		
		//render the cubes
		m_pMesh->Render(m4Projection, m4View, transformationMatrix);

	}


	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}