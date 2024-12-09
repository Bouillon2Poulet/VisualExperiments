// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HandDatas.h"
#include "GameFramework/Actor.h"
#include "Hand3D.generated.h"

USTRUCT()
struct FHand3DPoint
{
	GENERATED_BODY()
	FString Name;
	FVector3f Coordinates;
	UPROPERTY()
	AActor* Point;
};

UCLASS()
class WEBCAM_API AHand3D : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHand3D();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<FHand3DPoint> HandPoints;

	UPROPERTY()
	FVector HandAverageLocation;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> PointClass;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Update(TArray<FVector> Landmarks);
	
	UPROPERTY()
	UHandDatas* HandDataAsset;
};
