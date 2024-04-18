// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#include "GrpcMessageTagsManager.h"
#include "Misc/OutputDeviceNull.h"
#include "TurboLinkGrpcModule.h"

UGrpcMessageTagsManager* UGrpcMessageTagsManager::Singleton = nullptr;

FGrpcMessageTag::FGrpcMessageTag(FName _SimpleTagName, FName _TagName, TSharedPtr<FGrpcMessageTag> _ParentNode, UScriptStruct* _MessageScriptStruct)
	: SimpleTagName(_SimpleTagName)
	, TagName(_TagName)
	, ParentNode(_ParentNode)
	, MessageScriptStruct(_MessageScriptStruct)
{
}

void FGrpcMessageTag::ResetNode()
{
	SimpleTagName = NAME_None;
	TagName = NAME_None;
	for (int32 ChildIdx = 0; ChildIdx < ChildTags.Num(); ++ChildIdx)
	{
		ChildTags[ChildIdx]->ResetNode();
	}

	ChildTags.Empty();
	ParentNode.Reset();
}

void FGrpcMessageTag::FromExportString(const FString& ExportString, int32 PortFlags)
{
	ResetNode();

	FOutputDeviceNull NullOut;
	FGrpcMessageTag::StaticStruct()->ImportText(*ExportString, this, nullptr, PortFlags, &NullOut, TEXT("FGrpcMessageTag"), true);

	//Split tagname to get simple name
	TArray<FString> SubTags;
	TagName.ToString().ParseIntoArray(SubTags, TEXT("."), true);
	if (SubTags.Num() > 0)
	{
		SimpleTagName = FName(*SubTags.Last());
	}
}

UGrpcMessageTagsManager::UGrpcMessageTagsManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UGrpcMessageTagsManager::~UGrpcMessageTagsManager()
{
	DestroyTagTree();
	Singleton = nullptr;
}

void UGrpcMessageTagsManager::InitializeManager()
{
	check(!Singleton);

	Singleton = NewObject<UGrpcMessageTagsManager>(GetTransientPackage(), NAME_None);
	Singleton->AddToRoot();

	//construct tag tree
	Singleton->ConstructTagTree();
}

void UGrpcMessageTagsManager::ConstructTagTree()
{
	TGuardValue<bool> GuardRebuilding(bIsConstructingTagTree, true);
	if (GrpcMessageRootTag.IsValid()) return;

	GrpcMessageRootTag = MakeShareable(new FGrpcMessageTag());

	//Scan all grpc messages...
	FTurboLinkGrpcModule* turboLinkModule = FModuleManager::GetModulePtr<FTurboLinkGrpcModule>("TurboLinkGrpc");
	if (turboLinkModule == nullptr)
	{
		turboLinkModule = &(FModuleManager::LoadModuleChecked<FTurboLinkGrpcModule>("TurboLinkGrpc"));
	}
	if (turboLinkModule)
	{
		const TMap<FName, UScriptStruct*>& messageStructMap = turboLinkModule->GetMessageStructMap();
		for (const auto& elem : messageStructMap)
		{
			AddGrpcMessageTag(elem.Key, elem.Value);
		}
	}
}

void UGrpcMessageTagsManager::DestroyTagTree()
{
	if (GrpcMessageRootTag.IsValid())
	{
		GrpcMessageRootTag->ResetNode();
		GrpcMessageRootTag.Reset();
	}
}

void UGrpcMessageTagsManager::AddGrpcMessageTag(const FName& OriginalTagName, UScriptStruct* MessageScriptStruct)
{
	TSharedPtr<FGrpcMessageTag> CurNode = GrpcMessageRootTag;

	// Split the tag text on the "." delimiter to establish tag depth and then insert each tag into the tag tree
	TArray<FString> SubTags;
	OriginalTagName.ToString().ParseIntoArray(SubTags, TEXT("."), true);

	int32 NumSubTags = SubTags.Num();
	bool bHasSeenConflict = false;
	FString FullTagString;

	for (int32 SubTagIdx = 0; SubTagIdx < NumSubTags; ++SubTagIdx)
	{
		bool bIsExplicitTag = (SubTagIdx == (NumSubTags - 1));
		FName ShortTagName = *SubTags[SubTagIdx];
		FName FullTagName;

		if (bIsExplicitTag)
		{
			// We already know the final name
			FullTagName = OriginalTagName;
		}
		else if (SubTagIdx == 0)
		{
			// Full tag is the same as short tag, and start building full tag string
			FullTagName = ShortTagName;
			FullTagString = SubTags[SubTagIdx];
		}
		else
		{
			// Add .Tag and use that as full tag
			FullTagString += TEXT(".");
			FullTagString += SubTags[SubTagIdx];

			FullTagName = FName(*FullTagString);
		}

		TArray< TSharedPtr<FGrpcMessageTag> >& ChildTags = CurNode.Get()->GetChildTagNodes();
		int32 InsertionIdx = InsertTagIntoNodeArray(ShortTagName, FullTagName, CurNode, ChildTags, bIsExplicitTag? MessageScriptStruct: nullptr);

		CurNode = ChildTags[InsertionIdx];
	}
}

void UGrpcMessageTagsManager::GetGrpcMessageRootTags(TArray< TSharedPtr<FGrpcMessageTag> >& OutTagArray) const
{
	TArray<TSharedPtr<FGrpcMessageTag>>& grpcMessageRootTags = GrpcMessageRootTag->GetChildTagNodes();
	OutTagArray = grpcMessageRootTags;
}

TSharedPtr<FGrpcMessageTag> UGrpcMessageTagsManager::FindGrpcMessageTag(const FName& TagName, bool bWithScriptStructOnly)
{
	const TArray<TSharedPtr<FGrpcMessageTag>>& tagsArray = GrpcMessageRootTag->GetChildTagNodes();

	return FindGrpcMessageTagInArray(tagsArray, TagName, bWithScriptStructOnly);
}

TSharedPtr<FGrpcMessageTag> UGrpcMessageTagsManager::FindGrpcMessageTagInArray(const TArray<TSharedPtr<FGrpcMessageTag>>& grpcMessageTags, 
	const FName& TagName, bool bWithScriptStructOnly)
{
	for (int32 iItem = 0; iItem < grpcMessageTags.Num(); ++iItem)
	{
		if (grpcMessageTags[iItem]->TagName == TagName && 
			(!bWithScriptStructOnly || (bWithScriptStructOnly && grpcMessageTags[iItem]->MessageScriptStruct != nullptr)))
		{
			return grpcMessageTags[iItem];
		}

		if (grpcMessageTags[iItem]->TagName.GetStringLength() < TagName.GetStringLength())
		{
			TSharedPtr<FGrpcMessageTag> messageTag = FindGrpcMessageTagInArray(grpcMessageTags[iItem]->GetChildTagNodes(), 
				TagName, bWithScriptStructOnly);
			if (messageTag.IsValid()) return messageTag;
		}
	}
	return TSharedPtr<FGrpcMessageTag>();
}

int32 UGrpcMessageTagsManager::InsertTagIntoNodeArray(FName Tag, FName FullTag,
	TSharedPtr<FGrpcMessageTag> ParentNode, TArray< TSharedPtr<FGrpcMessageTag> >& NodeArray, UScriptStruct* MessageScriptStruct)
{
	int32 FoundNodeIdx = INDEX_NONE;
	int32 WhereToInsert = INDEX_NONE;

	// See if the tag is already in the array
	for (int32 CurIdx = 0; CurIdx < NodeArray.Num(); ++CurIdx)
	{
		FGrpcMessageTag* CurrNode = NodeArray[CurIdx].Get();
		if (CurrNode == nullptr) continue;
		
		FName SimpleTagName = CurrNode->SimpleTagName;
		if (SimpleTagName == Tag)
		{
			FoundNodeIdx = CurIdx;
			break;
		}
		else if (Tag.LexicalLess(SimpleTagName) && WhereToInsert == INDEX_NONE)
		{
			// Insert new node before this
			WhereToInsert = CurIdx;
		}
	}

	if (MessageScriptStruct != nullptr && FoundNodeIdx != INDEX_NONE)
	{
		FGrpcMessageTag* CurrNode = NodeArray[FoundNodeIdx].Get();
		CurrNode->MessageScriptStruct = MessageScriptStruct;
	}

	if (FoundNodeIdx == INDEX_NONE)
	{
		if (WhereToInsert == INDEX_NONE)
		{
			// Insert at end
			WhereToInsert = NodeArray.Num();
		}

		// Don't add the root node as parent
		TSharedPtr<FGrpcMessageTag> TagNode = 
			MakeShareable(new FGrpcMessageTag(Tag, FullTag, ParentNode == GrpcMessageRootTag ? nullptr : ParentNode, MessageScriptStruct));
		// Add at the sorted location
		FoundNodeIdx = NodeArray.Insert(TagNode, WhereToInsert);
	}
	return FoundNodeIdx;
}