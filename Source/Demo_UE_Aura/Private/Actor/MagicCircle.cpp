


#include "Actor/MagicCircle.h"

#include "Components/DecalComponent.h"

AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;


	MagicCircleDecalComp = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecalComp");
	SetRootComponent(MagicCircleDecalComp);
	//MagicCircleDecalComp->SetupAttachment(GetRootComponent());
}

void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
	
}

