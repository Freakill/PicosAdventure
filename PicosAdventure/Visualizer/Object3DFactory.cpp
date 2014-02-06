#include "Object3DFactory.h"

Object3DFactory Object3DFactory::object3DFactory_;

Object3DFactory::Object3DFactory()
{
	Register("StaticObject3D", &StaticObject3D::Create);
	Register("AnimatedObject3D", &AnimatedObject3D::Create);
}

Object3DFactory::Object3DFactory(const Object3DFactory &)
{

}

Object3DFactory::~Object3DFactory()
{

}

Object3DFactory* Object3DFactory::Instance()
{
	return (&object3DFactory_);
}

void Object3DFactory::Register(const std::string &object3DType, CreateObject3DFn pfnCreate)
{
	factoryMap_[object3DType] = pfnCreate;
}

Object3D* Object3DFactory::CreateObject3D(const std::string &object3DType, GraphicsManager* graphicsManager, const std::string &modelName)
{
	std::map<std::string, CreateObject3DFn>::iterator it = factoryMap_.find(object3DType);
	if( it != factoryMap_.end() )
		return it->second(graphicsManager, modelName);
	return NULL;
}