#include "MyMesh.h"
#define _USE_MATH_DEFINES
#include <math.h>
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	std::vector<vector3> tempVert;
	for (float i = 0; i <= a_nSubdivisions; ++i)
	{
		//do math for the points in a circle
		double angle = (2 * M_PI) * i / a_nSubdivisions;
		double s = sin(angle);
		double c = cos(angle);

		//calculates points in a circle and pushes back to tempVert
		tempVert.push_back(glm::vec3((a_fRadius * c), 0.0f, (a_fRadius * s)));

	}
	//adds tris to create sides of cone
	for (float i = 0; i < a_nSubdivisions; ++i)
	{
		//second variable in AddTri is a vec3 that adds point for all of the other points to connect to.
		AddTri(tempVert[i], glm::vec3(0.0f, a_fHeight, 0.0f), tempVert[i + 1]);
	}

	//creates bottom
	for (float i = 0; i < a_nSubdivisions; ++i)
	{
		AddTri(tempVert[i], tempVert[i + 1], glm::vec3(0, 0, 0));
	}


	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	std::vector<vector3> tempVert;
	std::vector<vector3> tempVert2;
	for (float i = 0; i <= a_nSubdivisions; ++i)
	{
		//do math for the points in a circle
		double angle = (2.0 * M_PI) * i / a_nSubdivisions;
		double s = sin(angle);
		double c = cos(angle);

		//these pushbacks add the points for the top and bottom to their respective vec3's
		tempVert.push_back(glm::vec3((a_fRadius * c), 0.0f, (a_fRadius * s))); 
		tempVert2.push_back(glm::vec3((a_fRadius * c), a_fHeight, (a_fRadius * s)));
	}

	//creates bottom
	for (float i = 0; i < a_nSubdivisions; ++i)
	{
		AddTri(tempVert[i], tempVert[i + 1], glm::vec3(0, 0, 0));
		AddTri(tempVert[i], tempVert2[i], tempVert2[i + 1]);
		AddTri(tempVert2[i + 1], tempVert[i + 1], tempVert[i]);
	}

	//creates top
	for (float i = 0; i < a_nSubdivisions; ++i)
	{
		AddTri(tempVert2[i], glm::vec3(0, a_fHeight, 0), tempVert2[i + 1]);
	}


	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	std::vector<vector3> tempVertOuterTop;
	std::vector<vector3> tempVertOuterBottom;
	std::vector<vector3> tempVertInnerTop;
	std::vector<vector3> tempVertInnerBottom;
	for (float i = 0; i <= a_nSubdivisions; ++i)
	{
		//do math for the points in a circle
		float angle = (2 * M_PI) * i / a_nSubdivisions;
		float s = sin(angle);
		float c = cos(angle);

		tempVertOuterBottom.push_back(glm::vec3((a_fOuterRadius * c), 0.0f, (a_fOuterRadius * s)));
		tempVertOuterTop.push_back(glm::vec3((a_fOuterRadius * c), a_fHeight, (a_fOuterRadius * s)));
		tempVertInnerBottom.push_back(glm::vec3((a_fInnerRadius * c), 0.0f, (a_fInnerRadius * s)));
		tempVertInnerTop.push_back(glm::vec3((a_fInnerRadius * c), a_fHeight, (a_fInnerRadius * s)));
	}

	//creates bottom and inside quads
	for (float i = 0; i < a_nSubdivisions; ++i)
	{
		AddQuad(tempVertOuterBottom[i], tempVertOuterBottom[i + 1], tempVertInnerBottom[i], tempVertInnerBottom[i + 1]);
		AddQuad(tempVertInnerBottom[i], tempVertInnerBottom[i + 1], tempVertInnerTop[i], tempVertInnerTop[i + 1]); //inside quads
	}

	//creates top and creates outside quads
	for (float i = 0; i < a_nSubdivisions; ++i)
	{
		AddQuad(tempVertOuterTop[i + 1], tempVertOuterTop[i], tempVertInnerTop[i + 1], tempVertInnerTop[i]);
		AddQuad(tempVertOuterBottom[i + 1], tempVertOuterBottom[i], tempVertOuterTop[i + 1], tempVertOuterTop[i]); //outside quads
	}


	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	
	std::vector<vector3> tempVertRing1;
	float radiusSubtraction = a_fOuterRadius - a_fInnerRadius;
	float radiusCenter = a_fInnerRadius + radiusSubtraction;

	//loop through the first set of subdivisions
	for (int i = 0; i <= a_nSubdivisionsA; i++)
	{
		float circ = glm::radians(360.0f / a_nSubdivisionsA) * i; //calculate all of the points in a circle for the number of subdivisions 
		float cx = sin(circ);
		float cy = 0;
		float cz = cos(circ);
		glm::vec3 centerPoint = glm::vec3((radiusCenter * cx), 0.0f, (radiusCenter * cz)); //calculate the center point using radiusCenter

		//loop through second set of subdivisions
		for (int j = 0; j <= a_nSubdivisionsB; j++)
		{
			//calculate the xyz points
			float don = glm::radians(360.0f / a_nSubdivisionsB) * j;
			float x = radiusCenter*cx + radiusSubtraction*sin(don)*cx;
			float y = radiusCenter*cy + radiusSubtraction*cos(don);
			float z = radiusCenter*cz + radiusSubtraction*sin(don)*cz;
			tempVertRing1.push_back(glm::vec3(x, y, z));
		}
	}

	//loop through the subdivisions again so that the points can be added to Tris
	for (int i = 0; i <= a_nSubdivisionsA; i++)
	{
		for (int j = i * a_nSubdivisionsB; j <= (i * a_nSubdivisionsB) + a_nSubdivisionsB; j++)
		{
			//adding all of the tris
			AddTri(tempVertRing1[j], tempVertRing1[(j + 1) % tempVertRing1.size()], tempVertRing1[(j + (a_nSubdivisionsB + 1)) % tempVertRing1.size()]);
			AddTri(tempVertRing1[j], tempVertRing1[(j + (a_nSubdivisionsB + 1)) % tempVertRing1.size()], tempVertRing1[(j + a_nSubdivisionsB) % tempVertRing1.size()]);
		}
	}
	
	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	std::vector<vector3> tempVert; //stores all of the vertices temporarily

	for (int i = 0; i <= a_nSubdivisions; ++i)
	{
		for (int j = 0; j <= a_nSubdivisions; j++)
		{
			float const y = sin(-M_PI_2 + M_PI * i / a_nSubdivisions);
			float const x = cos(2 * M_PI * j / a_nSubdivisions) * sin(M_PI * i / a_nSubdivisions);
			float const z = sin(2 * M_PI * j / a_nSubdivisions) * sin(M_PI * i / a_nSubdivisions);
			//finds all points for a sphere
			tempVert.push_back(glm::vec3(x * a_fRadius, y * a_fRadius, z * a_fRadius));
		}
	}

	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		tempVert.push_back(glm::vec3(0.0f, -a_fRadius, 0.0f));
	}
	
	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		for (int j = 0; j <= a_nSubdivisions; j++)
		{
			//adding the actual quads to the points
			AddQuad(tempVert[(i + 1) * a_nSubdivisions + j], tempVert[(i + 1) * a_nSubdivisions + (j + 1)], tempVert[i * a_nSubdivisions + j], tempVert[i * a_nSubdivisions + (j + 1)]);
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}