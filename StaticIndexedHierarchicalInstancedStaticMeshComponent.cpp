// MIT

#include "StaticIndexedHierarchicalInstancedStaticMeshComponent.h"


int32 UStaticIndexedHierarchicalInstancedStaticMeshComponent::AddInstance(const FTransform& InstanceTransform)
{
	auto index = UHierarchicalInstancedStaticMeshComponent::AddInstance(InstanceTransform);
	if (index != INDEX_NONE)
	{
		mStaticIndexes.SetNum(index + 1);
		if (mFreeIndexes.Num() == 0)
		{
			mFreeIndexes.Add(mDynamicIndexes.Num());
			mDynamicIndexes.SetNum(mDynamicIndexes.Num() + 1);
		}
		auto StaticIndex = mFreeIndexes.Last();
		mFreeIndexes.SetNum(mFreeIndexes.Num() - 1);
		mStaticIndexes[index] = StaticIndex;
		mDynamicIndexes[StaticIndex] = index;
		return StaticIndex;
	}
	return INDEX_NONE;
}

bool UStaticIndexedHierarchicalInstancedStaticMeshComponent::RemoveInstance(int32 InstanceIndex)
{
	if (!mDynamicIndexes.IsValidIndex(InstanceIndex))
		return false;
	
	auto index = mDynamicIndexes[InstanceIndex];
	if (UHierarchicalInstancedStaticMeshComponent::RemoveInstance(index))
	{
		mStaticIndexes[index] = mStaticIndexes.Last();
		mDynamicIndexes[mStaticIndexes[index]] = index;
		mStaticIndexes.RemoveAtSwap(index);
		mFreeIndexes.Add(InstanceIndex);
		return true;
	}
	return false;
}

void UStaticIndexedHierarchicalInstancedStaticMeshComponent::ClearInstances()
{
	mLastStaticIndex = 0;
	mStaticIndexes.SetNum(0);
	mDynamicIndexes.SetNum(0);
	mFreeIndexes.SetNum(0);
	UHierarchicalInstancedStaticMeshComponent::ClearInstances();
}

bool UStaticIndexedHierarchicalInstancedStaticMeshComponent::RemoveInstances(const TArray<int32>& InstancesToRemove)
{
	TArray<int32> indexes;
	indexes.SetNum(InstancesToRemove.Num());
	for (int32 i = 0; i < InstancesToRemove.Num(); ++i)
	{
		if (!mDynamicIndexes.IsValidIndex(InstancesToRemove[i]))
			return false;
		indexes[i] = mDynamicIndexes[InstancesToRemove[i]];
	}
	indexes.Sort(TGreater<int32>());

	if (UHierarchicalInstancedStaticMeshComponent::RemoveInstances(indexes))
	{
		for (auto index : indexes)
		{
			mStaticIndexes[index] = mStaticIndexes.Last();
			mDynamicIndexes[mStaticIndexes[index]] = index;
			mStaticIndexes.RemoveAtSwap(index);
		}
		for (auto index : InstancesToRemove)
			mFreeIndexes.Add(index);
	
		return true;
	}

	return false;
}

bool UStaticIndexedHierarchicalInstancedStaticMeshComponent::UpdateInstanceTransform(int32 InstanceIndex, const FTransform& NewInstanceTransform, bool bWorldSpace, bool bMarkRenderStateDirty /*= false*/, bool bTeleport /*= false*/)
{
	if (!mDynamicIndexes.IsValidIndex(InstanceIndex))
		return false;

	return UHierarchicalInstancedStaticMeshComponent::UpdateInstanceTransform(mDynamicIndexes[InstanceIndex], NewInstanceTransform, bWorldSpace, bMarkRenderStateDirty, bTeleport);
}

TArray<int32> UStaticIndexedHierarchicalInstancedStaticMeshComponent::GetInstancesOverlappingSphere(const FVector& Center, float Radius, bool bSphereInWorldSpace /*= true*/) const
{
	auto InstanceIndexes = UHierarchicalInstancedStaticMeshComponent::GetInstancesOverlappingSphere(Center, Radius, bSphereInWorldSpace);
	TArray<int32> indexes;
	indexes.SetNum(InstanceIndexes.Num());
	for (int32 i = 0; i < InstanceIndexes.Num(); ++i)
		indexes[i] = mStaticIndexes[InstanceIndexes[i]];
	return indexes;
}

TArray<int32> UStaticIndexedHierarchicalInstancedStaticMeshComponent::GetInstancesOverlappingBox(const FBox& Box, bool bBoxInWorldSpace /*= true*/) const
{
	auto InstanceIndexes = UHierarchicalInstancedStaticMeshComponent::GetInstancesOverlappingBox(Box, bBoxInWorldSpace);
	TArray<int32> indexes;
	indexes.SetNum(InstanceIndexes.Num());
	for (int32 i = 0; i < InstanceIndexes.Num(); ++i)
		indexes[i] = mStaticIndexes[InstanceIndexes[i]];
	return indexes;
}
