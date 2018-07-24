#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	//By checking header file, I see that I have an entity count variable, I need to set that to 0 at the beginning
	m_uEntityCount = 0;
	
	//I also see an entity list. That needs to be cleared at the beginning to make sure that it is empty when we try to populate it later.
	m_entityList.clear();
}
void MyEntityManager::Release(void)
{
	//Since MyEntity is a pointer, we will need to delete them once the program is released. But when the program is exited, the pointers will be in the entity list.
	//To fix this, we need to fill the entity list with a new MyEntity and then delete that MyEntity. 
	//use a for loop to go through the entity list, pretty simple.
	for (int i = 0; i < m_uEntityCount; ++i)
	{
		MyEntity* fillerEntity;
		fillerEntity = m_entityList[i];
		SafeDelete(fillerEntity);
	}
	//Now we need to reset the entity count and clear the entity list in order to make sure that it is empty
	m_entityList.clear();
	m_uEntityCount = 0;
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	//need to release m_pInstance. Easy, check if it is not a null ptr, if it is not a null ptr, then delete it and make it one
	if (m_pInstance != nullptr)
	{
		delete m_pInstance; //tried to use SafeDelete here at first, but then I remembered that SafeDelete is for pointers;
		m_pInstance = nullptr;
	}

}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	//should be simple. Just need to loop through the entity list, use GetUniqueID, and compare what it returns to a_sUniqueID
	//In fact the actual for loop itself should like the one I made in Release, just with a comparison instead of a deletion.
	for (int i = 0; i < m_uEntityCount; ++i)
	{
		if (a_sUniqueID == m_entityList[i]->GetUniqueID() )
		{
			return i;
		}
	}
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	//easy, just call GetModel at the specified point in entity list
	//we need checks in case a_uIndex is greater that the actual entity list size or if the entity list is empty
	if (m_entityList.size() == 0)
	{
		return nullptr;
	}

	if (a_uIndex >= m_entityList.size())
	{
		a_uIndex = m_entityList.size() - 1;
	}

	return m_entityList[a_uIndex]->GetModel();
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	//Should be simple if we store the entity in another temporary entity
	MyEntity* tempEntity = MyEntity::GetEntity(a_sUniqueID);
	if (tempEntity)
	{
		return tempEntity->GetModel();
	}
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	//easy, same thing as the first GetModel, but with GetRigidBody
	if (m_entityList.size() == 0)
	{
		return nullptr;
	}

	if (a_uIndex >= m_entityList.size())
	{
		a_uIndex = m_entityList.size() - 1;
	}

	return m_entityList[a_uIndex]->GetRigidBody();

}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	//same as second GetModel, but with GetRigidBody
	MyEntity* tempEntity = MyEntity::GetEntity(a_sUniqueID);
	if (tempEntity)
	{
		return tempEntity->GetRigidBody();
	}
	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	//again very similar, but we need to make sure that the matrix returned is the identity matrix if entity list is empty
	if (m_entityList.size() == 0)
	{
		return IDENTITY_M4;
	}

	if (a_uIndex >= m_entityList.size())
	{
		a_uIndex = m_entityList.size() - 1;
	}

	return m_entityList[a_uIndex]->GetModelMatrix();
	
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	//yet again, very similar
	MyEntity* tempEntity = MyEntity::GetEntity(a_sUniqueID);
	if (tempEntity)
	{
		return tempEntity->GetModelMatrix();
	}
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	//This should be easy as well because we will do the exact same thing as the other ones except we are actually setting the matrix to a_m4ToWorld
	MyEntity* tempEntity = MyEntity::GetEntity(a_sUniqueID);
	if (tempEntity)
	{
		tempEntity->SetModelMatrix(a_m4ToWorld);
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	//Again, should be simple because it should be similar to the others
	if (m_entityList.size() == 0)
	{
		return;
	}

	if (a_uIndex >= m_entityList.size())
	{
		a_uIndex = m_entityList.size() - 1;
	}

	return m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	//should be simple. Just use a double for loop to go through entity list and check if they are colliding
	for (int i = 0; i < m_uEntityCount - 1; ++i)
	{
		for (int j = i + 1; j < m_uEntityCount; ++j)
		{
			m_entityList[i]->IsColliding(m_entityList[j]);
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	//Easy, create a temporary entity to store the new entity, check if it initialized, and then push it back onto the entity list.
	MyEntity* tempEntity = new MyEntity(a_sFileName, a_sUniqueID);

	if (tempEntity->IsInitialized())
	{
		m_entityList.push_back(tempEntity);
		m_uEntityCount = m_entityList.size();
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	//This one is a bit more involved because it involves the same checks as before, but it involves an extra check, deleting the extra entity, and removing 1 from the entity count 
	if (m_entityList.size() == 0)
	{
		return;
	}

	if (a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	if (a_uIndex != m_uEntityCount - 1)
	{
		std::swap(m_entityList[a_uIndex], m_entityList[m_uEntityCount - 1]);
	}

	MyEntity* tempEntity = m_entityList[m_uEntityCount - 1];

	SafeDelete(tempEntity);

	m_entityList.pop_back();

	--m_uEntityCount;

	return;
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	//simple, store the index returned from GetEntityIndex and use that to remove it.
	int tempIndex = GetEntityIndex(a_sUniqueID);

	RemoveEntity((uint)tempIndex);
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	//again, same as GetModel but with a dfferent return type
	if (m_entityList.size() == 0)
	{
		return "";
	}

	if (a_uIndex >= m_entityList.size())
	{
		a_uIndex = m_entityList.size() - 1;
	}

	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	//same as GetModel, except we return entity list at a_uIndex
	if (m_entityList.size() == 0)
	{
		return nullptr;
	}

	if (a_uIndex >= m_entityList.size())
	{
		a_uIndex = m_entityList.size() - 1;
	}

	return m_entityList[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	//easy. Add all of the entities to the render list, but need to make sure to check if a_uIndex is bigger than entity count and give them rigidbodys	
	if (a_uIndex >= m_uEntityCount)
	{
		for (a_uIndex = 0; a_uIndex < m_uEntityCount; ++a_uIndex)
		{
			m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
		}
	}
	else
	{
		m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	MyEntity* tempEntity = MyEntity::GetEntity(a_sUniqueID);
	if (tempEntity)
	{
		tempEntity->AddToRenderList(a_bRigidBody);
	}
}