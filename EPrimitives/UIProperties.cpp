#include  "pch.h"
#include "UIProperties.h"
#include "Primitives.h"
using namespace UI;
void UI::UIContainer::show() {
	ImGui::SetCurrentContext(Primitives::Material::privateCtx);
	if (ImGui::CollapsingHeader("UI Properties", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader)) {
		ImGui::DragFloat("Position X",&InsDat.UILocation.x,0.01f,-1,1);
		ImGui::DragFloat("Position Y",&InsDat.UILocation.y,0.01f,-1,1);
		ImGui::DragFloat("Scaling X", &InsDat.UIScaling.x, 0.01f, 0, 1);
		ImGui::DragFloat("Scaling Y", &InsDat.UIScaling.y, 0.01f, 0, 1);
	}

}

std::string UI::UIContainer::GetPropertyClassName() {
	return "UIContainer";
}
ObjectProperties* UI::UIContainer::GetPropertyRef() {
	return this;
}
const std::type_info& UI::UIContainer::GetPropertyType() {
	return typeid(UIContainer);
}
UIContainer::UIContainer(Objects* obj):ObjectProperties(obj) {

}
bool UIContainer::isContainerMoving() {
	if ((InsDat.UILocation.x != prevX) || (InsDat.UILocation.y != prevY)||((InsDat.UIScaling.y != prevSY)|| InsDat.UIScaling.x != prevSX)) {
		prevX = InsDat.UILocation.x;
		prevY = InsDat.UILocation.y;
		prevSX = InsDat.UIScaling.x;
		prevSY = InsDat.UIScaling.y;
		return true;
	}
	return false;
}