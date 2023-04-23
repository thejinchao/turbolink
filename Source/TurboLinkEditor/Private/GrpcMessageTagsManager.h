// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "GrpcMessageTagsManager.generated.h"

USTRUCT()
struct FGrpcMessageTag
{
	GENERATED_BODY()
	FGrpcMessageTag() {};
	
	// Simple constructor, passing redundant data for performance */
	FGrpcMessageTag(FName _SimpleTagName, FName _TagName, TSharedPtr<FGrpcMessageTag> _ParentNode, UScriptStruct* _MessageScriptStruct);

	// Get the children nodes of this node
	FORCEINLINE TArray< TSharedPtr<FGrpcMessageTag> >& GetChildTagNodes() { return ChildTags; }
	// Get the parent tag node of this node
	FORCEINLINE TSharedPtr<FGrpcMessageTag> GetParentTagNode() const { return ParentNode; }
	// Reset the node of all of its values
	void ResetNode();

	// Raw name for this tag at current rank in the tree
	FName SimpleTagName;

	// Full tag name
	UPROPERTY(VisibleAnywhere, SaveGame, Category = TurboLink)
	FName TagName;

	// Child gameplay tag nodes
	TArray<TSharedPtr<FGrpcMessageTag>> ChildTags;
	// Owner gameplay tag node, if any
	TSharedPtr<FGrpcMessageTag> ParentNode;
	// The ScriptStruct of grpc message
	UScriptStruct* MessageScriptStruct;

	// Operators
	FORCEINLINE bool operator==(FGrpcMessageTag const& Other) const
	{
		return TagName == Other.TagName;
	}

	FORCEINLINE bool operator!=(FGrpcMessageTag const& Other) const
	{
		return TagName != Other.TagName;
	}

	FORCEINLINE bool operator<(FGrpcMessageTag const& Other) const
	{
		return TagName.LexicalLess(Other.TagName);
	}

	friend void operator<<(FStructuredArchive::FSlot Slot, FGrpcMessageTag& GrpcMessageTag)
	{
		Slot << GrpcMessageTag.TagName;
	}
	// Sets from a ImportText string, used in asset registry
	void FromExportString(const FString& ExportString, int32 PortFlags = 0);
};

UCLASS()
class UGrpcMessageTagsManager : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	void AddGrpcMessageTag(const FName& OriginalTagName, UScriptStruct* MessageScriptStruct);
	// Gets a copy of the GrpcMessageRootTag array
	void GetGrpcMessageRootTags(TArray< TSharedPtr<FGrpcMessageTag> >& OutTagArray) const;
	// find grpc message tag from name
	TSharedPtr<FGrpcMessageTag> FindGrpcMessageTag(const FName& TagName, bool bWithScriptStructOnly);

private:
	// Roots of gameplay tag nodes
	TSharedPtr<FGrpcMessageTag> GrpcMessageRootTag;
	bool bIsConstructingTagTree = false;

	// Construct the grpc message tag tree
	void ConstructTagTree();
	// Destroy the grpc message tag tree
	void DestroyTagTree();
	/**
	 * Helper function to insert a tag into a tag node array
	 *
	 * @param Tag							Short name of tag to insert
	 * @param FullTag						Full tag, passed in for performance
	 * @param ParentNode					Parent node, if any, for the tag
	 * @param NodeArray						Node array to insert the new node into, if necessary (if the tag already exists, no insertion will occur)
	 * @param MessageScriptStruct			UScriptStruct for message struct
	 *
	 * @return Index of the node of the tag
	 */
	int32 InsertTagIntoNodeArray(FName Tag, FName FullTag,
		TSharedPtr<FGrpcMessageTag> ParentNode, TArray< TSharedPtr<FGrpcMessageTag> >& NodeArray, UScriptStruct* MessageScriptStruct);

	TSharedPtr<FGrpcMessageTag> FindGrpcMessageTagInArray(const TArray<TSharedPtr<FGrpcMessageTag>>& grpcMessageTags, 
		const FName& TagName, bool bWithScriptStructOnly);
public:
	~UGrpcMessageTagsManager();
	// The Manager singleton
	static UGrpcMessageTagsManager* Singleton;
	// Initializes the singleton manager
	static void InitializeManager();
	// Returns the global UGrpcMessageTagsManager manager
	static UGrpcMessageTagsManager& Get()
	{
		if (Singleton == nullptr)
		{
			InitializeManager();
		}
		return *Singleton;
	}
};
