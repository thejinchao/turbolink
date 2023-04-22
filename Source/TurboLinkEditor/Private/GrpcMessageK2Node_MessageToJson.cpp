// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#include "GrpcMessageK2Node_MessageToJson.h"
#include "EdGraphSchema_K2.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "K2Node_MakeStruct.h"
#include "TurboLinkGrpcMessage.h"
#include "TurboLinkGrpcUtilities.h"

#define LOCTEXT_NAMESPACE "K2Node_TurboLinkGrpcMessageToJsonNode"

struct FK2Node_GrpcMessageToJsonHelper
{
	static FName MessagePinName;
	static FName PrettyModeName;
};

FName FK2Node_GrpcMessageToJsonHelper::MessagePinName(TEXT("GrpcMessage"));
FName FK2Node_GrpcMessageToJsonHelper::PrettyModeName(TEXT("PrettyMode"));

void UGrpcMessageToJsonNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UGrpcMessageToJsonNode::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "TurboLink");
}

FText UGrpcMessageToJsonNode::GetNodeTitle(ENodeTitleType::Type Title) const
{
	bool bLineked;
	UScriptStruct* structType = GetLinkedStruct(bLineked);
	if (structType == nullptr)
	{
		return LOCTEXT("DefaultTitle", "GrpcMessage To Json String");
	}

	if (CachedNodeTitle.IsOutOfDate(this))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("Message"), FText::FromName(structType->GetFName()));
		CachedNodeTitle.SetCachedText(FText::Format(LOCTEXT("NodeTitle", "{Message} To Json String"), Args), this);
	}

	return CachedNodeTitle;
}

FText UGrpcMessageToJsonNode::GetTooltipText() const
{
	return FText::FromString(TEXT("Convert GrpcMessage to Json String"));
}

void UGrpcMessageToJsonNode::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	if (Pin.PinName == FK2Node_GrpcMessageToJsonHelper::MessagePinName)
	{
		//set friend message name
		bool bConnected = false;
		UScriptStruct* scriptStruct = GetLinkedStruct(bConnected);
		if (scriptStruct != nullptr && bConnected)
		{
			HoverTextOut = FString::Printf(TEXT("Grpc Message\n%s"), *(scriptStruct->GetFName().ToString()));
			return;
		}
	}
	Super::GetPinHoverText(Pin, HoverTextOut);
}

void UGrpcMessageToJsonNode::AllocateDefaultPins()
{
	// Add execution pins
	UEdGraphPin* execPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* thenPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Add Message pin
	UEdGraphPin* messagePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, FK2Node_GrpcMessageToJsonHelper::MessagePinName);
	// Add Pretty Mode pin
	UEdGraphPin* prettyModePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, FK2Node_GrpcMessageToJsonHelper::PrettyModeName);

	// return value pin
	UEdGraphPin* returnValuePin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_String, UEdGraphSchema_K2::PN_ReturnValue);

	Super::AllocateDefaultPins();
}

void UGrpcMessageToJsonNode::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) 
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UEdGraphPin* execPin = GetExecPin();
	UEdGraphPin* messagePin = GetInputPin(FK2Node_GrpcMessageToJsonHelper::MessagePinName);
	UEdGraphPin* prettyModePin = GetInputPin(FK2Node_GrpcMessageToJsonHelper::PrettyModeName);
	UEdGraphPin* returnValuePin = GetResultPin();
	UEdGraphPin* thenPin = GetThenPin();

	//is linked to a valid make grpc message struct node?
	bool bLineked;
	UScriptStruct* structType = GetLinkedStruct(bLineked);
	if (bLineked && structType == nullptr)
	{
		//compile error
		CompilerContext.MessageLog.Error(TEXT("Only accept struct inherited from the FGrpcMessage. @@"), this);
		BreakAllNodeLinks();
		return;
	}
	
	// not working yet!
	if (execPin == nullptr || thenPin == nullptr)
	{
		BreakAllNodeLinks();
		return;
	}

	// here we adapt/bind our pins to the static function pins that we are calling.
	UK2Node_CallFunction* callFunctionNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	static FName InternalFunctionName = GET_FUNCTION_NAME_CHECKED(UGrpcMessageToJsonFunctionLibrary, GrpcMessageToJsonInternal);
	callFunctionNode->FunctionReference.SetExternalMember(InternalFunctionName, UGrpcMessageToJsonFunctionLibrary::StaticClass());
	callFunctionNode->AllocateDefaultPins();

	// get pins from call function node
	UEdGraphPin* callFunction_ExecPin = callFunctionNode->GetExecPin();
	UEdGraphPin* callFunction_MessagePin = callFunctionNode->FindPinChecked(FK2Node_GrpcMessageToJsonHelper::MessagePinName);
	UEdGraphPin* callFunction_PrettyModePin = callFunctionNode->FindPinChecked(FK2Node_GrpcMessageToJsonHelper::PrettyModeName);
	UEdGraphPin* callFunction_ReturnValuePin = callFunctionNode->GetReturnValuePin();
	UEdGraphPin* callFunction_ThenPin = callFunctionNode->GetThenPin();

	//grpc message in
	if (messagePin && messagePin->LinkedTo.Num()>0)
	{
		//assign input struct type to the wildcard input node type of call function node
		UEdGraphPin* linkedMessageStruct = messagePin->LinkedTo[0];
		CompilerContext.MovePinLinksToIntermediate(*messagePin, *callFunction_MessagePin);
		callFunction_MessagePin->PinType = linkedMessageStruct->PinType;
	}
	// pretty mode pin
	CompilerContext.MovePinLinksToIntermediate(*prettyModePin, *callFunction_PrettyModePin);
	// return value pin
	CompilerContext.MovePinLinksToIntermediate(*returnValuePin, *callFunction_ReturnValuePin);
	//exec pin 
	CompilerContext.MovePinLinksToIntermediate(*execPin, *callFunction_ExecPin);
	//then pin
	CompilerContext.MovePinLinksToIntermediate(*thenPin, *callFunction_ThenPin);
	// break any links to the expanded node
	BreakAllNodeLinks();
}

UEdGraphPin* UGrpcMessageToJsonNode::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UGrpcMessageToJsonNode::GetInputPin(const FName& PinName) const
{
	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : Pins)
	{
		if (TestPin && TestPin->PinName == PinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UGrpcMessageToJsonNode::GetResultPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UScriptStruct* UGrpcMessageToJsonNode::GetLinkedStruct(bool& bLinked) const
{
	UEdGraphPin* messagePin = GetInputPin(FK2Node_GrpcMessageToJsonHelper::MessagePinName);

	//is linked to a valid make grpc message struct node?
	if (messagePin==nullptr || messagePin->LinkedTo.Num() == 0)
	{
		bLinked = false;
		return nullptr;
	}

	bLinked = true;
	UEdGraphPin* messagePinLink = messagePin->LinkedTo[0];
	if (messagePinLink && messagePinLink->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
	{
		UScriptStruct* structType = Cast<UScriptStruct>(messagePinLink->PinType.PinSubCategoryObject.Get());
		if (structType && structType->IsChildOf(FGrpcMessage::StaticStruct()))
		{
			return structType;
		}
	}

	return nullptr;
}

void UGrpcMessageToJsonNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin && (Pin->PinName == FK2Node_GrpcMessageToJsonHelper::MessagePinName))
	{
		OnMessagePinChanged();
	}
}

void UGrpcMessageToJsonNode::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	if (ChangedPin && (ChangedPin->PinName == FK2Node_GrpcMessageToJsonHelper::MessagePinName))
	{
		OnMessagePinChanged();
	}
}

void UGrpcMessageToJsonNode::OnMessagePinChanged()
{
	UEdGraphPin* MessagePin = GetInputPin(FK2Node_GrpcMessageToJsonHelper::MessagePinName);

	// Mark dirty
	CachedNodeTitle.MarkDirty();
	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

#undef LOCTEXT_NAMESPACE
