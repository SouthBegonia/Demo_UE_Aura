// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEMO_UE_AURA_API ISaveInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/* If return true, Actor will set transform using the data in SaveGame */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ShouldLoadTransform();

	/* Call after setup necessary info with SaveGame data, now you can use it freely (note: this happened before Actor::BeginPlay() */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnActorLoaded();
};
