// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSideRewindComponent.h"


UServerSideRewindComponent::UServerSideRewindComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UServerSideRewindComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}


// Called every frame
void UServerSideRewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

