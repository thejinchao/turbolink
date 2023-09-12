// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#include "SGrpcMessageTagWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SSearchBox.h"

#define LOCTEXT_NAMESPACE "GrpcMessageTagWidget"

void SGrpcMessageTagWidget::Construct(const FArguments& InArgs)
{
	OnTagChanged = InArgs._OnTagChanged;
	MaxHeight = InArgs._MaxHeight;
	SelectedMessageTagName = InArgs._SelectedMessageTagName;

	UGrpcMessageTagsManager& tagsManager = UGrpcMessageTagsManager::Get();
	tagsManager.GetGrpcMessageRootTags(TagItems);

	//is selected tag validity
	VerifySelectedTagValidity();

	ChildSlot
	[
		SNew(SBorder)
#if ENGINE_MAJOR_VERSION>=5
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
#else
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
#endif
		[
			SNew(SVerticalBox)

			// Message Tag Tree controls
			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Top)
			[
				SNew(SHorizontalBox)

				// Expand All nodes
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.OnClicked(this, &SGrpcMessageTagWidget::OnExpandAllClicked)
					.Text(LOCTEXT("GrpcMessageTagWidget_ExpandAll", "Expand All"))
				]

				// Collapse All nodes
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.OnClicked(this, &SGrpcMessageTagWidget::OnCollapseAllClicked)
					.Text(LOCTEXT("GrpcMessageTagWidget_CollapseAll", "Collapse All"))
				]

				// Clear selections
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.OnClicked(this, &SGrpcMessageTagWidget::OnClearSelectClicked)
					.Text(LOCTEXT("GrpcMessageTagWidget_ClearSelect", "Clear Select"))
				]

				// Search
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.FillWidth(1.f)
				.Padding(5, 1, 5, 1)
				[
					SAssignNew(SearchTagBox, SSearchBox)
					.HintText(LOCTEXT("GrpcMessageTagWidget_SearchBoxHint", "Search GrpcMessage Tags"))
					.OnTextChanged(this, &SGrpcMessageTagWidget::OnFilterTextChanged)
				]
			]

			// Message Tags tree
			+ SVerticalBox::Slot()
			.MaxHeight(MaxHeight)
			[
				SAssignNew(TagTreeContainerWidget, SBorder)
				.Padding(FMargin(4.f))
				[
					SAssignNew(TagTreeWidget, STreeView< TSharedPtr<FGrpcMessageTag> >)
					.TreeItemsSource(&TagItems)
					.OnGenerateRow(this, &SGrpcMessageTagWidget::OnGenerateRow)
					.OnGetChildren(this, &SGrpcMessageTagWidget::OnGetChildren)
					.SelectionMode(ESelectionMode::SingleToggle)
				]
			]
		]
	];

	FilterTagTree();
}

FVector2D SGrpcMessageTagWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	FVector2D WidgetSize = SCompoundWidget::ComputeDesiredSize(LayoutScaleMultiplier);

	FVector2D TagTreeContainerSize = TagTreeContainerWidget->GetDesiredSize();

	if (TagTreeContainerSize.Y < MaxHeight)
	{
		WidgetSize.Y += MaxHeight - TagTreeContainerSize.Y;
	}

	return WidgetSize;
}

void SGrpcMessageTagWidget::OnFilterTextChanged(const FText& InFilterText)
{
	FilterString = InFilterText.ToString();

	FilterTagTree();
}

void SGrpcMessageTagWidget::FilterTagTree()
{
	TagTreeWidget->SetTreeItemsSource(&TagItems);
	for (int32 iItem = 0; iItem < TagItems.Num(); ++iItem)
	{
		SetDefaultTagNodeItemExpansion(TagItems[iItem]);
	}

	TagTreeWidget->RequestTreeRefresh();
}

void SGrpcMessageTagWidget::SelectMessage(const FName& MessageTagName)
{
	SelectedMessageTagName = MessageTagName;
	OnTagChanged.ExecuteIfBound(SelectedMessageTagName);
}

bool SGrpcMessageTagWidget::FilterChildrenCheck(TSharedPtr<FGrpcMessageTag> InItem)
{
	if (!InItem.IsValid())
	{
		return false;
	}

	auto FilterChildrenCheck_r = ([this, InItem]()
		{
			TArray< TSharedPtr<FGrpcMessageTag> > Children = InItem->GetChildTagNodes();
			for (int32 iChild = 0; iChild < Children.Num(); ++iChild)
			{
				if (FilterChildrenCheck(Children[iChild]))
				{
					return true;
				}
			}
			return false;
		});

	if (InItem->TagName.ToString().Contains(FilterString) || FilterString.IsEmpty())
	{
		return true;
	}

	return FilterChildrenCheck_r();
}

TSharedPtr<SWidget> SGrpcMessageTagWidget::GetWidgetToFocusOnOpen() 
{ 
	return SearchTagBox; 
}

TSharedRef<ITableRow> SGrpcMessageTagWidget::OnGenerateRow(TSharedPtr<FGrpcMessageTag> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	FText TooltipText;
	bool bWithScriptStruct = false;
	if (InItem.IsValid())
	{
		FString TooltipString = InItem->TagName.ToString();

		if (InItem.IsValid() && InItem->MessageScriptStruct != nullptr)
		{
			bWithScriptStruct = true;
		}
		TooltipText = FText::FromString(TooltipString);
	}

	return SNew(STableRow< TSharedPtr<FGrpcMessageTag> >, OwnerTable)
#if ENGINE_MAJOR_VERSION>=5
		.Style(FAppStyle::Get(), "GameplayTagTreeView")
#else
		.Style(FEditorStyle::Get(), "GameplayTagTreeView")
#endif
		[
			SNew(SHorizontalBox)

			// Tag Selection (selection mode only)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.HAlign(HAlign_Left)
			[
				SNew(SCheckBox)
				.OnCheckStateChanged(this, &SGrpcMessageTagWidget::OnTagCheckStatusChanged, InItem)
				.IsChecked(this, &SGrpcMessageTagWidget::IsTagChecked, InItem)
				.ToolTipText(TooltipText)
				.IsEnabled(true)
				.Visibility(bWithScriptStruct ? EVisibility::Visible : EVisibility::Collapsed)
				[
					SNew(STextBlock)
					.Text(FText::FromName(InItem->SimpleTagName))
				]
			]
			
			// Normal Tag Display (management mode only)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.ToolTip(FSlateApplication::Get().MakeToolTip(TooltipText))
				.Text(FText::FromName(InItem->SimpleTagName))
				.Visibility(!bWithScriptStruct ? EVisibility::Visible : EVisibility::Collapsed)
			]
		];
}

void SGrpcMessageTagWidget::OnGetChildren(TSharedPtr<FGrpcMessageTag> InItem, TArray< TSharedPtr<FGrpcMessageTag> >& OutChildren)
{
	TArray< TSharedPtr<FGrpcMessageTag> > FilteredChildren;
	TArray< TSharedPtr<FGrpcMessageTag> > Children = InItem->GetChildTagNodes();

	for (int32 iChild = 0; iChild < Children.Num(); ++iChild)
	{
		if (FilterChildrenCheck(Children[iChild]))
		{
			FilteredChildren.Add(Children[iChild]);
		}
	}
	OutChildren += FilteredChildren;
}

void SGrpcMessageTagWidget::OnTagCheckStatusChanged(ECheckBoxState NewCheckState, TSharedPtr<FGrpcMessageTag> NodeChanged)
{
	if (NewCheckState == ECheckBoxState::Checked)
	{
		SelectMessage(NodeChanged->TagName);
	}
}

ECheckBoxState SGrpcMessageTagWidget::IsTagChecked(TSharedPtr<FGrpcMessageTag> Node) const
{
	if (Node->TagName == SelectedMessageTagName)
	{
		return ECheckBoxState::Checked;
	}
	return ECheckBoxState::Unchecked;;
}

bool SGrpcMessageTagWidget::IsChildChecked(TSharedPtr<FGrpcMessageTag> Node) const
{
	FString selectedMessageTagName = SelectedMessageTagName.ToString().Left(Node->TagName.GetStringLength());
	if (selectedMessageTagName.Compare(Node->TagName.ToString()) == 0)
	{
		return true;
	}
	return false;
}

FReply SGrpcMessageTagWidget::OnClearSelectClicked()
{
	SelectMessage(NAME_None);
	return FReply::Handled();
}

FReply SGrpcMessageTagWidget::OnExpandAllClicked()
{
	SetTagTreeItemExpansion(true);
	return FReply::Handled();
}

FReply SGrpcMessageTagWidget::OnCollapseAllClicked()
{
	SetTagTreeItemExpansion(false);
	return FReply::Handled();
}

void SGrpcMessageTagWidget::SetTagTreeItemExpansion(bool bExpand)
{
	for (int32 TagIdx = 0; TagIdx < TagItems.Num(); ++TagIdx)
	{
		SetTagNodeItemExpansion(TagItems[TagIdx], bExpand);
	}
}

void SGrpcMessageTagWidget::SetTagNodeItemExpansion(TSharedPtr<FGrpcMessageTag> Node, bool bExpand)
{
	if (Node.IsValid() && TagTreeWidget.IsValid())
	{
		TagTreeWidget->SetItemExpansion(Node, bExpand);

		const TArray< TSharedPtr<FGrpcMessageTag> >& ChildTags = Node->GetChildTagNodes();
		for (int32 ChildIdx = 0; ChildIdx < ChildTags.Num(); ++ChildIdx)
		{
			SetTagNodeItemExpansion(ChildTags[ChildIdx], bExpand);
		}
	}
}

void SGrpcMessageTagWidget::SetDefaultTagNodeItemExpansion(TSharedPtr<FGrpcMessageTag> Node)
{
	if (Node.IsValid() && TagTreeWidget.IsValid())
	{
		bool bExpanded = false;
		if (!FilterString.IsEmpty())
		{
			bExpanded = true;
		}

		if (IsTagChecked(Node) == ECheckBoxState::Checked || IsChildChecked(Node))
		{
			bExpanded = true;
		}
		TagTreeWidget->SetItemExpansion(Node, bExpanded);

		const TArray< TSharedPtr<FGrpcMessageTag> >& ChildTags = Node->GetChildTagNodes();
		for (int32 ChildIdx = 0; ChildIdx < ChildTags.Num(); ++ChildIdx)
		{
			SetDefaultTagNodeItemExpansion(ChildTags[ChildIdx]);
		}
	}
}

void SGrpcMessageTagWidget::VerifySelectedTagValidity()
{
	if (SelectedMessageTagName == NAME_None) return;

	UGrpcMessageTagsManager& tagsManager = UGrpcMessageTagsManager::Get();
	TSharedPtr<FGrpcMessageTag> messageTag = tagsManager.FindGrpcMessageTag(SelectedMessageTagName, true);
	if (messageTag.IsValid()) return;

	//clean selected
	SelectMessage(NAME_None);
}

#undef LOCTEXT_NAMESPACE
