#include "UIData.h"
#include "UITypes.h"

FWidgetData* UUIData::FindWidget(EUIType TargetType) {
	for (FWidgetData& WC : WidgetClasses) {
		if (WC.WidgetClass && WC.UIType == TargetType) return &WC;
	}

	return nullptr;
}
