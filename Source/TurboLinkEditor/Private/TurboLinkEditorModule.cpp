// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#include "TurboLinkEditorModule.h"
#include "GrpcMessageTagsManager.h"
#include "EdGraphUtilities.h"
#include "SGraphPin.h"
#include "SGrpcMessageTagGraphPin.h"

class FGrpcMessageTagGraphPinFactory : public FGraphPanelPinFactory
{
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override
	{
		if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
		{
			if (UScriptStruct* PinStructType = Cast<UScriptStruct>(InPin->PinType.PinSubCategoryObject.Get()))
			{
				if (PinStructType->IsChildOf(FGrpcMessageTag::StaticStruct()))
				{
					return SNew(SGrpcMessageTagGraphPin, InPin);
				}
			}
		}

		return nullptr;
	}
};

void FTurboLinkEditorModule::StartupModule()
{
	// This will force initialization GrpcMessageTagsManager
	UGrpcMessageTagsManager::Get();

	FCoreDelegates::OnPostEngineInit.AddLambda([]() {
		// Register GrpcMessageTagGraphPinFactory
		TSharedPtr<FGrpcMessageTagGraphPinFactory> grpcMessageTagGraphPinFactory = MakeShareable(new FGrpcMessageTagGraphPinFactory());
		FEdGraphUtilities::RegisterVisualPinFactory(grpcMessageTagGraphPinFactory);
	});
}

void FTurboLinkEditorModule::ShutdownModule()
{
	UGrpcMessageTagsManager::Singleton = nullptr;
}

IMPLEMENT_MODULE(FTurboLinkEditorModule, TurboLinkEditor)