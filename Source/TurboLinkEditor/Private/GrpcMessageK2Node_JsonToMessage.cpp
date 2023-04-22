// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#include "GrpcMessageK2Node_JsonToMessage.h"
#include "EdGraphSchema_K2.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "TurboLinkGrpcMessage.h"
#include "TurboLinkGrpcUtilities.h"
#include "GrpcMessageTagsManager.h"

#define LOCTEXT_NAMESPACE "K2Node_JsonToGrpcMessageNode"

struct FK2Node_JsonToGrpcMessageHelper
{
	static FName MessageTypePinName;
	static FName JsonStringPinName;
	static FName ReturnMessagePinName;
};

FName FK2Node_JsonToGrpcMessageHelper::MessageTypePinName(TEXT("MessageType"));
FName FK2Node_JsonToGrpcMessageHelper::JsonStringPinName(TEXT("JsonString"));
FName FK2Node_JsonToGrpcMessageHelper::ReturnMessagePinName(TEXT("ReturnMessage"));

void UJsonToGrpcMessageNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UJsonToGrpcMessageNode::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "TurboLink");
}

FText UJsonToGrpcMessageNode::GetNodeTitle(ENodeTitleType::Type Title) const
{
	return LOCTEXT("DefaultTitle", "Json String To GrpcMessage");
}

FText UJsonToGrpcMessageNode::GetTooltipText() const
{
	return FText::FromString(TEXT("Convert Json String to GrpcMessage"));
}

void UJsonToGrpcMessageNode::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	if (Pin.PinName == FK2Node_JsonToGrpcMessageHelper::ReturnMessagePinName)
	{
		//set friend message name
		UScriptStruct* scriptStruct = GetMessageScriptStruct();
		if (scriptStruct != nullptr)
		{
			HoverTextOut = FString::Printf(TEXT("Return Message\n%s"), *(scriptStruct->GetFName().ToString()));
			return;
		}
	}
	Super::GetPinHoverText(Pin, HoverTextOut);
}

void UJsonToGrpcMessageNode::AllocateDefaultPins()
{
	// Add execution pins
	UEdGraphPin* execPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* thenPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Add Message type pin
	UEdGraphPin* messageTypePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FGrpcMessageTag::StaticStruct(), FK2Node_JsonToGrpcMessageHelper::MessageTypePinName);
	messageTypePin->bNotConnectable = 1;

	// Add String pin
	UEdGraphPin* jsonStringPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, FK2Node_JsonToGrpcMessageHelper::JsonStringPinName);

	// Add return message pin
	UEdGraphPin* returnMessagePin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FGrpcMessage::StaticStruct(), FK2Node_JsonToGrpcMessageHelper::ReturnMessagePinName);
	
	// Add result pin
	UEdGraphPin* returnValuePin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, UEdGraphSchema_K2::PN_ReturnValue);
	returnValuePin->PinFriendlyName = FText::FromString(TEXT("IsSuccess"));

	Super::AllocateDefaultPins();
}

void UJsonToGrpcMessageNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin && (Pin->PinName == FK2Node_JsonToGrpcMessageHelper::MessageTypePinName))
	{
		OnMessageTypeChanged();
	}
}

void UJsonToGrpcMessageNode::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	if (ChangedPin && (ChangedPin->PinName == FK2Node_JsonToGrpcMessageHelper::MessageTypePinName))
	{
		OnMessageTypeChanged();
	}
}

void UJsonToGrpcMessageNode::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	AllocateDefaultPins();

	// Change class of output pin
	UEdGraphPin* returnValuePin = GetReturnMessagePin();
	returnValuePin->PinType.PinSubCategoryObject = GetMessageScriptStruct(&OldPins);

	RestoreSplitPins(OldPins);
}

void UJsonToGrpcMessageNode::OnMessageTypeChanged()
{
	UEdGraphPin* returnMessagePin = GetReturnMessagePin();

	// Change class of output pin
	returnMessagePin->PinType.PinSubCategoryObject = GetMessageScriptStruct();

	// Cache all the pin connections to the ResultPin, we will attempt to recreate them
	TArray<UEdGraphPin*> resultPinConnectionList = returnMessagePin->LinkedTo;
	// Because the archetype has changed, we break the output link as the output pin type will change
	returnMessagePin->BreakAllPinLinks(true);

	// Recreate any pin links to the Result pin that are still valid
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	for (UEdGraphPin* connections : resultPinConnectionList)
	{
		K2Schema->TryCreateConnection(returnMessagePin, connections);
	}

	// Refresh the UI for the graph so the pin changes show up
	GetGraph()->NotifyGraphChanged();

	// Mark dirty
	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

UScriptStruct* UJsonToGrpcMessageNode::GetMessageScriptStruct(const TArray<UEdGraphPin*>* InPinsToSearch) const
{
	UScriptStruct* returnStruct = nullptr;
	UEdGraphPin* inputPin = GetInputPin(FK2Node_JsonToGrpcMessageHelper::MessageTypePinName, InPinsToSearch);
	if (inputPin == nullptr) return nullptr;

	//find message tag
	FGrpcMessageTag tempMessageTag;
	tempMessageTag.FromExportString(inputPin->DefaultValue, PPF_SerializedAsImportText);

	TSharedPtr<FGrpcMessageTag> grpcMessageTag = UGrpcMessageTagsManager::Get().FindGrpcMessageTag(tempMessageTag.TagName, true);
	if (grpcMessageTag.IsValid())
	{
		returnStruct = grpcMessageTag->MessageScriptStruct;
	}
	return returnStruct;
}

UEdGraphPin* UJsonToGrpcMessageNode::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UJsonToGrpcMessageNode::GetInputPin(const FName& PinName, const TArray<UEdGraphPin*>* InPinsToSearch) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : *PinsToSearch)
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

UEdGraphPin* UJsonToGrpcMessageNode::GetReturnMessagePin(const TArray<UEdGraphPin*>* InPinsToSearch) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : *PinsToSearch)
	{
		if (TestPin && TestPin->PinName == FK2Node_JsonToGrpcMessageHelper::ReturnMessagePinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UJsonToGrpcMessageNode::GetResultPin() const
{
	UEdGraphPin* Pin = FindPin(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output);
	return Pin;
}

void UJsonToGrpcMessageNode::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UEdGraphPin* execPin = GetExecPin();
	UEdGraphPin* jsonStringPin = GetInputPin(FK2Node_JsonToGrpcMessageHelper::JsonStringPinName);
	UEdGraphPin* returnMessagePin = GetReturnMessagePin();
	UEdGraphPin* returnValuePin = GetResultPin();
	UEdGraphPin* thenPin = GetThenPin();

	//valid message type assigned?
	UScriptStruct* structType = GetMessageScriptStruct();
	if (structType == nullptr)
	{
		//compile error
		CompilerContext.MessageLog.Error(TEXT("Must assign a valid grpc message type. @@"), this);
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
	static FName InternalFunctionName = GET_FUNCTION_NAME_CHECKED(UGrpcMessageToJsonFunctionLibrary, JsonToGrpcMessageInternal);
	callFunctionNode->FunctionReference.SetExternalMember(InternalFunctionName, UGrpcMessageToJsonFunctionLibrary::StaticClass());
	callFunctionNode->AllocateDefaultPins();

	// get pins from call function node
	UEdGraphPin* callFunction_ExecPin = callFunctionNode->GetExecPin();
	UEdGraphPin* callFunction_JsonStringPin = callFunctionNode->FindPinChecked(FK2Node_JsonToGrpcMessageHelper::JsonStringPinName);
	UEdGraphPin* callFunction_ReturnMessagePin = callFunctionNode->FindPinChecked(FK2Node_JsonToGrpcMessageHelper::ReturnMessagePinName);
	UEdGraphPin* callFunction_ReturnValuePin = callFunctionNode->GetReturnValuePin();
	UEdGraphPin* callFunction_ThenPin = callFunctionNode->GetThenPin();

	//json string pin
	CompilerContext.MovePinLinksToIntermediate(*jsonStringPin, *callFunction_JsonStringPin);
	//return message pin
	CompilerContext.MovePinLinksToIntermediate(*returnMessagePin, *callFunction_ReturnMessagePin);
	callFunction_ReturnMessagePin->PinType = returnMessagePin->PinType;
	//return value pin
	CompilerContext.MovePinLinksToIntermediate(*returnValuePin, *callFunction_ReturnValuePin);
	//exec pin 
	CompilerContext.MovePinLinksToIntermediate(*execPin, *callFunction_ExecPin);
	//then pin
	CompilerContext.MovePinLinksToIntermediate(*thenPin, *callFunction_ThenPin);
	// break any links to the expanded node
	BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE
