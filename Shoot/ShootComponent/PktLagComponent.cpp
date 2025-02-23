// Fill out your copyright notice in the Description page of Project Settings.


#include "PktLagComponent.h"

UPktLagComponent::UPktLagComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	
}

void UPktLagComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UPktLagComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

