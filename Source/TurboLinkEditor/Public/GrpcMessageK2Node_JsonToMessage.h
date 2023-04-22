// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "K2Node.h"
#include "GrpcMessageK2Node_JsonToMessage.generated.h"

UCLASS()
class TURBOLINKEDITOR_API UJsonToGrpcMessageNode : public UK2Node
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
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin);
	void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins);
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;

	// Get the then output pin
	UEdGraphPin* GetThenPin() const;
	// Get the input pin
	UEdGraphPin* GetInputPin(const FName& PinName, const TArray<UEdGraphPin*>* InPinsToSearch=nullptr) const;	
	// Get return message pin
	UEdGraphPin* GetReturnMessagePin(const TArray<UEdGraphPin*>* InPinsToSearch=nullptr) const;
	// Get the result output pin
	UEdGraphPin* GetResultPin() const;

protected:
	// Refresh pins when input message was changed
	void OnMessageTypeChanged();
	// Get message script struct
	UScriptStruct* GetMessageScriptStruct(const TArray<UEdGraphPin*>* InPinsToSearch=nullptr) const;
};
