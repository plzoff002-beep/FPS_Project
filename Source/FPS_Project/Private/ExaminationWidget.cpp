#include "ExaminationWidget.h"
#include "ExamineStudio.h"
#include "ItemTypes.h"

void UExaminationWidget::SetContextItem(UObject* NewContext) {
	if (!NewContext || !DescriptionText || !ItemNameText) return;

	if (AExamineStudio* ExamineStudio = Cast<AExamineStudio>(NewContext)) {
		if (ExamineStudio->GetCurrentData()) {
			ItemNameText->SetText(ExamineStudio->GetCurrentData()->Name);
			DescriptionText->SetText(ExamineStudio->GetCurrentData()->Description);
		}
	}
}