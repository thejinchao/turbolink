// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SlateFwd.h"
#include "UObject/Object.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"
#include "GrpcMessageTagsManager.h"

// Widget allowing user to tag assets with gameplay tags
class SGrpcMessageTagWidget : public SCompoundWidget
{
public:

	// Called when a tag status is changed
	DECLARE_DELEGATE_OneParam(FOnTagChanged, FName)

	SLATE_BEGIN_ARGS(SGrpcMessageTagWidget)
		: _SelectedMessageTagName(NAME_None)
		, _MaxHeight(260.0f)
	{}
		SLATE_ARGUMENT(FName, SelectedMessageTagName) // Name of selected message
		SLATE_EVENT(FOnTagChanged, OnTagChanged) // Called when a tag status changes
		SLATE_ARGUMENT(float, MaxHeight)	// caps the height of the gameplay tag tree
	SLATE_END_ARGS()

	// Construct the actual widget.
	void Construct(const FArguments& InArgs);

	// Ensures that this widget will always account for the MaxHeight if it's specified
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

	// Updates the tag list when the filter text changes
	void OnFilterTextChanged(const FText& InFilterText);

	// Returns true if this TagNode has any children that match the current filter
	bool FilterChildrenCheck(TSharedPtr<FGrpcMessageTag>);

	// Gets the widget to focus once the menu opens.
	TSharedPtr<SWidget> GetWidgetToFocusOnOpen();

private:
	// Verify the selected tags are all valid
	void VerifySelectedTagValidity();

	// Filters the tree view based on the current filter text.
	void FilterTagTree();

	void SelectMessage(const FName& MessageTagName);
	
	// Holds the full tag name of urrent checked message 
	FName SelectedMessageTagName;

	// The maximum height of the gameplay tag tree. If 0, the height is unbound.
	float MaxHeight;

	// Array of tags to be displayed in the TreeView
	TArray< TSharedPtr<FGrpcMessageTag> > TagItems;

	// Container widget holding the tag tree
	TSharedPtr<SBorder> TagTreeContainerWidget;

	// Tree widget showing the gameplay tag library
	TSharedPtr< STreeView< TSharedPtr<FGrpcMessageTag> > > TagTreeWidget;

	// Allows for the user to find a specific gameplay tag in the tree
	TSharedPtr<SSearchBox> SearchTagBox;

	// Filter string used during search box
	FString FilterString;

	// Called when the Tag list changes
	FOnTagChanged OnTagChanged;

	/**
	 * Generate a row widget for the specified item node and table
	 *
	 * @param InItem		Tag node to generate a row widget for
	 * @param OwnerTable	Table that owns the row
	 *
	 * @return Generated row widget for the item node
	 */
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FGrpcMessageTag> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	/**
	 * Get children nodes of the specified node
	 *
	 * @param InItem		Node to get children of
	 * @param OutChildren	[OUT] Array of children nodes, if any
	 */
	void OnGetChildren(TSharedPtr<FGrpcMessageTag> InItem, TArray< TSharedPtr<FGrpcMessageTag> >& OutChildren);

	/**
	 * Called via delegate when the status of a check box in a row changes
	 *
	 * @param NewCheckState	New check box state
	 * @param NodeChanged	Node that was checked/unchecked
	 */
	void OnTagCheckStatusChanged(ECheckBoxState NewCheckState, TSharedPtr<FGrpcMessageTag> NodeChanged);

	/**
	 * Called via delegate to determine the checkbox state of the specified node
	 *
	 * @param Node	Node to find the checkbox state of
	 *
	 * @return Checkbox state of the specified node
	 */
	ECheckBoxState IsTagChecked(TSharedPtr<FGrpcMessageTag> Node) const;

	// is any child node checked
	bool IsChildChecked(TSharedPtr<FGrpcMessageTag> Node) const;

	// Called when the user clicks the "Clear Select" button;
	FReply OnClearSelectClicked();

	// Called when the user clicks the "Expand All" button; Expands the entire tag tree
	FReply OnExpandAllClicked();

	// Called when the user clicks the "Collapse All" button; Collapses the entire tag tree 
	FReply OnCollapseAllClicked();

	/**
	 * Helper function to set the expansion state of the tree widget
	 *
	 * @param bExpand If true, expand the entire tree; Otherwise, collapse the entire tree
	 */
	void SetTagTreeItemExpansion(bool bExpand);

	/**
	 * Helper function to set the expansion state of a specific node
	 *
	 * @param Node		Node to set the expansion state of
	 * @param bExapnd	If true, expand the node; Otherwise, collapse the node
	 */
	void SetTagNodeItemExpansion(TSharedPtr<FGrpcMessageTag> Node, bool bExpand);

	// Recursive function to go through all tags in the tree and set the expansion to default
	void SetDefaultTagNodeItemExpansion(TSharedPtr<FGrpcMessageTag> Node);
};
