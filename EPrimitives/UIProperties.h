#pragma once
#include "Global.h"
#include "EPrimitives.h"
namespace UI {
	struct InstanceData {
		struct {
			float x = 0.0f;
			float y = 0.0f;
		}UILocation;
		struct {
			float x = 1.0f;
			float y = 1.0f;
		}UIScaling;
	};
	class UIContainer :public ObjectProperties {
	private:
		float prevX = 0;
		float prevY = 0;
		float prevSX = 1;
		float prevSY = 1;
	public:
		PRIMITVE_API UIContainer(Objects* obj);
		InstanceData InsDat;
		PRIMITVE_API bool isContainerMoving();
		PRIMITVE_API const std::type_info& GetPropertyType();
		PRIMITVE_API ObjectProperties* GetPropertyRef();
		PRIMITVE_API std::string GetPropertyClassName();
		PRIMITVE_API void show();
	};
}