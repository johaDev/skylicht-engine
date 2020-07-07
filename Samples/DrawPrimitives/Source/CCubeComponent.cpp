#include "pch.h"
#include "SkylichtEngine.h"
#include "CCubeComponent.h"
#include "CCubeData.h"

CCubeComponent::CCubeComponent()
{

}

CCubeComponent::~CCubeComponent()
{

}

void CCubeComponent::initComponent()
{
	CEntity *entity = m_gameObject->getEntity();

	// add cube mesh data to entity
	// see more code init cube mesh from CCubeData
	CCubeData *cube = entity->addData<CCubeData>();
	cube->initCube(1.0f);

	// add culling
	CCullingData *culling = entity->addData<CCullingData>();
	culling->Type = CCullingData::BoundingBox;
}

void CCubeComponent::updateComponent()
{

}