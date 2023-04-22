// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#include "SGrpcMessageTagGraphPin.h"
#include "SGrpcMessageTagWidget.h"

#define LOCTEXT_NAMESPACE "GrpcMessageTagGraphPin"

void SGrpcMessageTagGraphPin::Construct( const FArguments& InArgs, UEdGraphPin* InGraphPinObj )
{
	SGraphPin::Construct( SGraphPin::FArguments(), InGraphPinObj );
}

void SGrpcMessageTagGraphPin::ParseDefaultValueData()
{
	// Read using import text, but with serialize flag set so it doesn't always throw away invalid ones
	FGrpcMessageTag grpcMessageTag;
	grpcMessageTag.FromExportString(GraphPinObj->GetDefaultAsString(), PPF_SerializedAsImportText);

	SelectedMessageTagName = grpcMessageTag.TagName;
}


TSharedRef<SWidget>	SGrpcMessageTagGraphPin::GetDefaultValueWidget()
{
	if (GraphPinObj == nullptr)
	{
		return SNullWidget::NullWidget;
	}

	ParseDefaultValueData();

	//Create widget
	return SNew(SVerticalBox)
		.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(ComboButton, SComboButton)
			.OnGetMenuContent(this, &SGrpcMessageTagGraphPin::GetEditContent)
			.OnMenuOpenChanged(this, &SGrpcMessageTagGraphPin::OnGrpcMessageWidgetOpenStateChanged)
			.ContentPadding(FMargin(2.0f, 2.0f))
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SelectText", "Select Message Type"))
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			GetDescriptionContent()
		];
}

TSharedRef<SWidget> SGrpcMessageTagGraphPin::GetEditContent()
{
	TSharedRef<SGrpcMessageTagWidget> messageTagWidget =
		SNew(SGrpcMessageTagWidget)
		.OnTagChanged(this, &SGrpcMessageTagGraphPin::OnMessageChecked)
		.SelectedMessageTagName(SelectedMessageTagName)
		.Visibility(this, &SGraphPin::GetDefaultValueVisibility);

	CurrentMessageTagWidget = messageTagWidget;

	return SNew( SVerticalBox )
		+SVerticalBox::Slot()
		.AutoHeight()
		.MaxHeight( 400 )
		[
			messageTagWidget
		];

}

TSharedRef<SWidget> SGrpcMessageTagGraphPin::GetDescriptionContent()
{
	SAssignNew(TagTextBlock, STextBlock)
		.Text(FText::FromName(SelectedMessageTagName))
		.Visibility(EVisibility::Visible);

	return TagTextBlock->AsShared();
}

void SGrpcMessageTagGraphPin::OnGrpcMessageWidgetOpenStateChanged(bool bIsOpened)
{
	if (bIsOpened)
	{
		//set edit focus to search box
		TSharedPtr<SGrpcMessageTagWidget> currentMessageTagWidget = CurrentMessageTagWidget.Pin();
		if (currentMessageTagWidget.IsValid())
		{
			ComboButton->SetMenuContentWidgetToFocus(currentMessageTagWidget->GetWidgetToFocusOnOpen());
		}
	}
}

void SGrpcMessageTagGraphPin::OnMessageChecked(FName _SelectedMessageTag)
{
	SelectedMessageTagName = _SelectedMessageTag;
	SaveDefaultValueData();
}

void SGrpcMessageTagGraphPin::SaveDefaultValueData()
{	
	// Refresh selected message tag name
	if (TagTextBlock.IsValid())
	{
		TagTextBlock->SetText(FText::FromName(SelectedMessageTagName));
	}

	// Set serialize string to pin default value
	FString SerializeString = FString::Printf(TEXT("(TagName=\"%s\")"), *SelectedMessageTagName.ToString());
	FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();
	if (CurrentDefaultValue.IsEmpty() || CurrentDefaultValue == TEXT("(TagName=\"\")"))
	{
		CurrentDefaultValue = FString(TEXT(""));
	}
	if (!CurrentDefaultValue.Equals(SerializeString))
	{
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, SerializeString);
	}
}

#undef LOCTEXT_NAMESPACE
