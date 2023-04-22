// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "KismetPins/SGraphPinStructInstance.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "SGrpcMessageTagWidget.h"

/** Pin that represents a single gameplay tag, overrides the generic struct widget because tags have their own system for saving changes */
class SGrpcMessageTagGraphPin : public SGraphPinStructInstance
{
public:
	SLATE_BEGIN_ARGS(SGrpcMessageTagGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

protected:
	//~ Begin SGraphPinStructInstance Interface
	virtual void ParseDefaultValueData() override;
	virtual void SaveDefaultValueData() override;
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	virtual TSharedRef<SWidget> GetEditContent() override;
	virtual TSharedRef<SWidget> GetDescriptionContent() override;
	//~ End SGraphPin Interface

	void OnGrpcMessageWidgetOpenStateChanged(bool bIsOpened);
	void OnMessageChecked(FName _SelectedMessageTag);

	// Holds the full tag name of urrent checked message 
	FName SelectedMessageTagName;

	// The text block used to display the tag name of selected grpc message
	TSharedPtr<STextBlock> TagTextBlock;
	// Current message tag widget
	TWeakPtr<class SGrpcMessageTagWidget> CurrentMessageTagWidget;
};
