// Copyright Daniel V. Sandholt 2017

#include "BuildingEscape.h"
#include "OpenDoor.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UOpenDoor::BeginPlay() {
	Super::BeginPlay();
	Owner = GetOwner();

	if( !PressurePlate ) UE_LOG(LogTemp, Error, TEXT("%s missing a PressurePlate"), *GetOwner()->GetName());
}

void UOpenDoor::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if( GetTotalMassOffActorsOnPlate() > TriggerMass ) {
		OnOpen.Broadcast();
	} else
		OnClose.Broadcast();
}

float UOpenDoor::GetTotalMassOffActorsOnPlate() {
	float TotalMass = 0.f;
	
	if( PressurePlate ) {
		TArray<AActor*> OverlappingActors;
		PressurePlate->GetOverlappingActors( OUT OverlappingActors );

		for( const auto* Actor : OverlappingActors ) {
			// TODO if( Owner->GetName().Contains( "Blue" ) ) if( Actor->GetName().Contains( "Blue" ) )
			UE_LOG( LogTemp, Warning, TEXT( "%s has %s" ), *Owner->GetName(), *Actor->GetName() );
				TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		}
	}

	return TotalMass;
}