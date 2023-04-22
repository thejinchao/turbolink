// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "K2Node.h"
#include "GrpcMessageK2Node_MessageToJson.generated.h"

UCLASS()
class TURBOLINKEDITOR_API UGrpcMessageToJsonNode : public UK2Node
{
	GENERATED_BODY()

public:
	// Override UK2Node Interface
	virtual bool IsNodeSafeToIgnore() const override { return true; }

	// Override UEdGraphNode Interface.
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type Title) const override;
	virtual void AllocateDefaultPins() override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin);
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;

	// Get the then output pin
	UEdGraphPin* GetThenPin() const;
	// Get the input pin
	UEdGraphPin* GetInputPin(const FName& PinName) const;
	// Get the result output pin
	UEdGraphPin* GetResultPin() const;
	// Get linked struct script
	UScriptStruct* GetLinkedStruct(bool& bLinked) const;

protected:
	// Refresh pins when input message was changed
	void OnMessagePinChanged();

protected:
	// Constructing FText strings can be costly, so we cache the node's title
	FNodeTextCache CachedNodeTitle;
};
