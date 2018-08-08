#include "MyOctantExample.h"
Simplex::uint Simplex::MyOctant::m_uMaxLevel;
Simplex::uint Simplex::MyOctant::m_uIdealEntityCount;
int octantCount = 0;
bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	if (m_uChildren < a_nEntities)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Simplex::MyOctant::KillBranches(void)
{

}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{

}

void Simplex::MyOctant::AssignIDtoEntity(void)
{

}

Simplex::uint Simplex::MyOctant::GetOctantCount(void)
{
	return octantCount;
}

void Simplex::MyOctant::Release(void)
{

}

void Simplex::MyOctant::Init(void)
{

}

void Simplex::MyOctant::ConstructList(void)
{

}

Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	m_pMeshMngr = MeshManager::GetInstance();
	//loop through all objects and find min and max positions
	m_pEntityMngr = MyEntityManager::GetInstance();
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		if (m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMinGlobal().x < m_v3Min.x)
		{
			m_v3Min.x = m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMinGlobal().x;
		}
		if (m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMinGlobal().y < m_v3Min.y)
		{
			m_v3Min.y = m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMinGlobal().y;
		}
		if (m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMinGlobal().z < m_v3Min.z)
		{
			m_v3Min.z = m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMinGlobal().z;
		}
		if (m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMaxGlobal().x > m_v3Max.x)
		{
			m_v3Max.x = m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMaxGlobal().x;
		}
		if (m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMaxGlobal().y > m_v3Max.y)
		{
			m_v3Max.y = m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMaxGlobal().y;
		}
		if (m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMaxGlobal().z > m_v3Max.z)
		{
			m_v3Max.z = m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMaxGlobal().z;
		}
		m_EntityList.push_back(i);

	}
	    m_v3Center.x = (m_v3Min.x + m_v3Max.x) / 2;
		m_v3Center.y = (m_v3Min.y + m_v3Max.y) / 2;
		m_v3Center.z = (m_v3Min.z + m_v3Max.z) / 2;

		m_fSize = (m_v3Max.x - m_v3Min.x);
		if (m_fSize < (m_v3Max.y - m_v3Min.y))
		{
			m_fSize = (m_v3Max.y - m_v3Min.y);
		}
		if (m_fSize < (m_v3Max.z - m_v3Min.z))
		{
			m_fSize = (m_v3Max.z - m_v3Min.z);
		}
		
		m_uIdealEntityCount = a_nIdealEntityCount;
		m_uMaxLevel = a_nMaxLevel;

		if (m_EntityList.size() > m_uIdealEntityCount && m_uLevel < m_uMaxLevel)
		{
			Subdivide();
		}
		m_uID = 0;
}


int currentLevel = 0;
Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize, int level)
{
	m_uID = ++octantCount;
	m_uLevel = level;
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;
	m_pMeshMngr = MeshManager::GetInstance();
	//loop through all objects and find min and max positions
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_v3Min.x = m_v3Center.x - m_fSize / 2;
	m_v3Min.y = m_v3Center.y - m_fSize / 2;
	m_v3Min.z = m_v3Center.z - m_fSize / 2;
	m_v3Max.x = m_v3Center.x + m_fSize / 2;
	m_v3Max.y = m_v3Center.y + m_fSize / 2;
	m_v3Max.z = m_v3Center.z + m_fSize / 2;

	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		if (m_v3Min.x <= m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetCenterGlobal().x &&
			m_v3Max.x >= m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetCenterGlobal().x &&
			m_v3Min.y <= m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetCenterGlobal().y &&
			m_v3Max.y >= m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetCenterGlobal().y &&
			m_v3Min.z <= m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetCenterGlobal().z &&
			m_v3Max.z >= m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetCenterGlobal().z)
		{
			m_EntityList.push_back(i);
			m_pEntityMngr->GetEntity(i)->ClearDimensionSet();
			m_pEntityMngr->GetEntity(i)->AddDimension(m_uID);
		}
	}
	
	if (m_EntityList.size() > m_uIdealEntityCount && m_uLevel < m_uMaxLevel)
	{
		Subdivide();
	}
	
}

Simplex::MyOctant::MyOctant(MyOctant const & other)
{

}

//Simplex::MyOctant & Simplex::MyOctant::operator=(MyOctant const & other)
//{
//	// TODO: insert return statement he
//}

Simplex::MyOctant::~MyOctant(void)
{
	if (m_uChildren == 8)
	{
		for (int i = 0; i < 8; i++)
		{
			SafeDelete(m_pChild[i]);
		}
	}
}

void Simplex::MyOctant::Swap(MyOctant & other)
{

}

float Simplex::MyOctant::GetSize(void)
{
	return m_fSize;
}

Simplex::vector3 Simplex::MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

Simplex::vector3 Simplex::MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

Simplex::vector3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	return false;
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (m_uID == a_nIndex)
	{
		Display(a_v3Color);
	}
	else
	{
		if (m_uChildren == 8)
		{
			for (auto& i : m_pChild)
			{
				i->Display(a_nIndex, a_v3Color);
			}

		}
	}
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	a_v3Color = C_YELLOW;
	Simplex::matrix4 tempMat4;
	tempMat4 = glm::translate(tempMat4, m_v3Center);
	if (m_uMeshID == -1)
	{
		m_uMeshID = m_pMeshMngr->GenerateCube(m_fSize, a_v3Color);
	}
	//m_pMeshMngr->AddCubeToRenderList(m_fSize,1);
	m_pMeshMngr->AddMeshToRenderList(m_uMeshID, tempMat4, 2);
//	std::cout << m_pMeshMngr->GetMeshCount() << std::endl;
	if (m_uChildren != 0)
	{
		for (auto& i:m_pChild)
		{
			i->Display(a_v3Color);
		}
	}

}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{

}

void Simplex::MyOctant::ClearEntityList(void)
{

}

void Simplex::MyOctant::Subdivide(void)
{
	m_uChildren = 8;
	m_pChild[0] = new MyOctant(vector3(m_v3Center.x + m_fSize / 4.0, m_v3Center.y + m_fSize / 4.0, m_v3Center.z + m_fSize / 4.0), m_fSize / 2.0, m_uLevel + 1);
	m_pChild[1] = new MyOctant(vector3(m_v3Center.x + m_fSize / 4.0, m_v3Center.y + m_fSize / 4.0, m_v3Center.z - m_fSize / 4.0), m_fSize / 2.0, m_uLevel + 1);
	m_pChild[2] = new MyOctant(vector3(m_v3Center.x + m_fSize / 4.0, m_v3Center.y - m_fSize / 4.0, m_v3Center.z + m_fSize / 4.0), m_fSize / 2.0, m_uLevel + 1);
	m_pChild[3] = new MyOctant(vector3(m_v3Center.x + m_fSize / 4.0, m_v3Center.y - m_fSize / 4.0, m_v3Center.z - m_fSize / 4.0), m_fSize / 2.0, m_uLevel + 1);
	m_pChild[4] = new MyOctant(vector3(m_v3Center.x - m_fSize / 4.0, m_v3Center.y + m_fSize / 4.0, m_v3Center.z + m_fSize / 4.0), m_fSize / 2.0, m_uLevel + 1);
	m_pChild[5] = new MyOctant(vector3(m_v3Center.x - m_fSize / 4.0, m_v3Center.y + m_fSize / 4.0, m_v3Center.z - m_fSize / 4.0), m_fSize / 2.0, m_uLevel + 1);
	m_pChild[6] = new MyOctant(vector3(m_v3Center.x - m_fSize / 4.0, m_v3Center.y - m_fSize / 4.0, m_v3Center.z + m_fSize / 4.0), m_fSize / 2.0, m_uLevel + 1);
	m_pChild[7] = new MyOctant(vector3(m_v3Center.x - m_fSize / 4.0, m_v3Center.y - m_fSize / 4.0, m_v3Center.z - m_fSize / 4.0), m_fSize / 2.0, m_uLevel+1);
	
}

Simplex::MyOctant * Simplex::MyOctant::GetChild(uint a_nChild)
{
	return m_pChild[a_nChild];
}

Simplex::MyOctant * Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	return false;
}
