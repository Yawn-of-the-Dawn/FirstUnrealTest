// Copyright Daniel V. Sandholt 2017

#include "BuildingEscape.h"
#include "Grabber.h"

// This is a Macro that does noting, but highlights the word OUT in the editor
#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber(){
	/// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	/// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay(){
	Super::BeginPlay();
	PhysicsHandleComponent();
	SetupInputComponent();
}

void UGrabber::PhysicsHandleComponent(){
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(!PhysicsHandle){
		UE_LOG(LogTemp, Error, TEXT("No PhysicsHandle detected on %s"), *GetOwner()->GetName() );
	}
}

void UGrabber::SetupInputComponent(){
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if(InputComponent){
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}else{
		UE_LOG(LogTemp, Error, TEXT("No InputComponent detected on %s"), *GetOwner()->GetName() );
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach() {
	/// Setup query parameters
	FCollisionQueryParams TraceParameters( FName( TEXT( "" ) ), false, GetOwner() );

	/// Line-trace (Ray-cast) out to reach distance
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams( ECollisionChannel::ECC_PhysicsBody ),
		TraceParameters
	);

	return HitResult;
}

void UGrabber::Grab() {
	/// Line-trace (ray-cast) and  reach any actors with physicsHandle
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	if( ActorHit ) {
		if( !PhysicsHandle ) return;
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None, /// no bones needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			ComponentToGrab->GetOwner()->GetActorRotation()
		);
	}
}

void UGrabber::Release() {
	if( !PhysicsHandle ) return;
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if( !PhysicsHandle ) return;
	if( PhysicsHandle->GrabbedComponent ) {
		PhysicsHandle->SetTargetLocation( GetReachLineEnd() );
	}
}

FVector UGrabber::GetReachLineStart() {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint( OUT PlayerViewPointLocation, OUT PlayerViewPointRotation );

	return PlayerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd(){
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint( OUT PlayerViewPointLocation, OUT PlayerViewPointRotation );

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}