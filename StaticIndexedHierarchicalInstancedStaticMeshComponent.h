// MIT

#pragma once

#include "StaticIndexedHierarchicalInstancedStaticMeshComponent.generated.h"


UCLASS(ClassGroup = Rendering, meta = (BlueprintSpawnableComponent))
class MY_API UStaticIndexedHierarchicalInstancedStaticMeshComponent : public UHierarchicalInstancedStaticMeshComponent
{
	GENERATED_BODY()
public:
	// UInstancedStaticMesh interface
	virtual int32 AddInstance(const FTransform& InstanceTransform) override;
	virtual bool RemoveInstance(int32 InstanceIndex) override;
	virtual void ClearInstances() override;

	bool RemoveInstances(const TArray<int32>& InstancesToRemove);

	virtual bool UpdateInstanceTransform(int32 InstanceIndex, const FTransform& NewInstanceTransform, bool bWorldSpace, bool bMarkRenderStateDirty = false, bool bTeleport = false) override;
	virtual TArray<int32> GetInstancesOverlappingSphere(const FVector& Center, float Radius, bool bSphereInWorldSpace = true) const override;
	virtual TArray<int32> GetInstancesOverlappingBox(const FBox& Box, bool bBoxInWorldSpace = true) const override;

private:
	TArray<int32> mStaticIndexes;
	TArray<int32> mDynamicIndexes;
	TArray<int32> mFreeIndexes;
	int32 mLastStaticIndex = 0;
};