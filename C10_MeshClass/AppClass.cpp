#include "AppClass.h"
void Application::InitVariables(void)
{
	//Make MyMesh object
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(2.0f, C_BROWN);

	//Make MyMesh object
	m_pMesh1 = new MyMesh();
	m_pMesh1->GenerateCube(1.0f, C_WHITE);

	//first row of antenna
	posVec.push_back(glm::vec3(-6, 6, 0));
	posVec.push_back(glm::vec3(6, 6, 0));
	//end of first row with antenna

	//2nd row of antenna
	posVec.push_back(glm::vec3(-4, 4, 0));
	posVec.push_back(glm::vec3(4, 4, 0));
	//end of 2nd row with antenna

	//row above eyes
	posVec.push_back(glm::vec3(0, 2, 0));
	posVec.push_back(glm::vec3(-2, 2, 0));
	posVec.push_back(glm::vec3(-4, 2, 0));
	posVec.push_back(glm::vec3(-6, 2, 0));
	posVec.push_back(glm::vec3(2, 2, 0));
	posVec.push_back(glm::vec3(4, 2, 0));
	posVec.push_back(glm::vec3(6, 2, 0));
	//end of row above eyes

	//row with eyes
	posVec.push_back(glm::vec3(0, 0, 0));
	posVec.push_back(glm::vec3(-2, 0, 0));
	posVec.push_back(glm::vec3(-6, 0, 0));
	posVec.push_back(glm::vec3(-8, 0, 0));
	posVec.push_back(glm::vec3(2, 0, 0));
	posVec.push_back(glm::vec3(6, 0, 0));
	posVec.push_back(glm::vec3(8, 0, 0));
	//end of row with eyes

	//row under eyes
	posVec.push_back(glm::vec3(0, -2, 0));
	posVec.push_back(glm::vec3(-2, -2, 0));
	posVec.push_back(glm::vec3(-4, -2, 0));
	posVec.push_back(glm::vec3(-6, -2, 0));
	posVec.push_back(glm::vec3(-8, -2, 0));
	posVec.push_back(glm::vec3(-10, -2, 0));
	posVec.push_back(glm::vec3(2, -2, 0));
	posVec.push_back(glm::vec3(4, -2, 0));
	posVec.push_back(glm::vec3(6, -2, 0));
	posVec.push_back(glm::vec3(8, -2, 0));
	posVec.push_back(glm::vec3(10, -2, 0));
	//end of row under eyes

	//2nd row below eyes
	posVec.push_back(glm::vec3(0, -4, 0));
	posVec.push_back(glm::vec3(-2, -4, 0));
	posVec.push_back(glm::vec3(-4, -4, 0));
	posVec.push_back(glm::vec3(-6, -4, 0));
	posVec.push_back(glm::vec3(-10, -4, 0));
	posVec.push_back(glm::vec3(2, -4, 0));
	posVec.push_back(glm::vec3(4, -4, 0));
	posVec.push_back(glm::vec3(6, -4, 0));
	posVec.push_back(glm::vec3(10, -4, 0));
	//end of 2nd row below eyes

	//2nd to last row
	posVec.push_back(glm::vec3(-6, -6, 0));
	posVec.push_back(glm::vec3(-10, -6, 0));
	posVec.push_back(glm::vec3(6, -6, 0));
	posVec.push_back(glm::vec3(10, -6, 0));
	//end of 2nd to last row 

	//last row
	posVec.push_back(glm::vec3(-2, -8, 0));
	posVec.push_back(glm::vec3(-4, -8, 0));
	posVec.push_back(glm::vec3(2, -8, 0));
	posVec.push_back(glm::vec3(4, -8, 0));
	//end of last row

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

	//m_pMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), ToMatrix4(m_qArcBall));
	//m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3( 3.0f, 0.0f, 0.0f)));
		
	matrix4 transformationMatrix; //name self explanatory 

	for (int i = 0; i < posVec.size(); i++)
	{
		//scale cubes
		transformationMatrix = glm::scale(matrix4(), glm::vec3(0.5, 0.5, 1));
		
		//translate every cube in posVec to the location at i
		transformationMatrix = glm::translate(transformationMatrix, posVec[i]);

		//render the cubes
		m_pMesh->Render(m4Projection, m4View, transformationMatrix);

		//translate them
		posVec[i] = posVec[i] + glm::vec3(0.1f, 0, 0);
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