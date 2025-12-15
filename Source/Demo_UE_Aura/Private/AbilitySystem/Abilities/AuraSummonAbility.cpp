


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	check(NumMinions > 1)	// not supported NumMinions==1 yet

	TArray<FVector> SpawnLocations;

	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const FVector Forward = AvatarActor->GetActorForwardVector();
	const FVector Location = AvatarActor->GetActorLocation();
	const float DeltaSpread = NumMinions == 1 ? 0.f : SpawnSpread / (NumMinions - 1);

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);

	if (bDrawDebugLine)
		UKismetSystemLibrary::DrawDebugCoordinateSystem(AvatarActor, Location, AvatarActor->GetActorRotation(), 100.f, 3.f, 5.f);


	for (int32 i = 0; i < NumMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit,
			ChosenSpawnLocation + FVector(0.f, 0.f, 200.f),
			ChosenSpawnLocation - FVector(0.f, 0.f, 200.f),
			ECC_Visibility
		);
		if (Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;
		}

		SpawnLocations.Add(ChosenSpawnLocation);


		if (bDrawDebugLine)
		{
			// Area to query
			UKismetSystemLibrary::DrawDebugLine(AvatarActor, Location + Direction * MinSpawnDistance, Location + Direction * MaxSpawnDistance, FLinearColor::White, 3.f);

			// Location for Spawn(will be rearranged in BP)
			//UKismetSystemLibrary::DrawDebugSphere(AvatarActor, ChosenSpawnLocation, 5.f, 3.f, FLinearColor::Green, 3.f);
		}
	}

	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	check(MinionClasses.Num() > 0);

	const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[Selection];
}
