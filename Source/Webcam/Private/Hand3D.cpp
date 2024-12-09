// Fill out your copyright notice in the Description page of Project Settings.


#include "Hand3D.h"

#include "HandDatas.h"


// Sets default values
AHand3D::AHand3D()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	HandDataAsset = CreateDefaultSubobject<UHandDatas>(FName(this->GetName().Append("_HandDatas")));
}

// Called when the game starts or when spawned
void AHand3D::BeginPlay()
{
	Super::BeginPlay();
	for(int i=0; i<21; i++)
	{
		FString Name = EnumToString("EHandLandmark", EHandLandmark(i));
		FHand3DPoint Point = FHand3DPoint(Name, FVector3f(0), GetWorld()->SpawnActor<AActor>(PointClass));
		HandPoints.Add(Point);
	}
}

// Called every frame
void AHand3D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHand3D::Update(TArray<FVector> Landmarks)
{
	// HandDataAsset->Update(DataParsed);
	int index = 0;
	FVector Sum = FVector(0);
	for(FVector Coordinate : Landmarks)
	{
		FVector Coordinates = FVector(Coordinate.Z, Coordinate.X, -Coordinate.Y);
		HandPoints[index].Point->SetActorLocation(FVector(Coordinates*1000.f));
		Sum += Coordinate;
		index++;
	}
	HandAverageLocation = Sum/index;
}

