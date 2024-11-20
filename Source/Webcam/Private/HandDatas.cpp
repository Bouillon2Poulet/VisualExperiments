// Fill out your copyright notice in the Description page of Project Settings.


#include "HandDatas.h"

void UHandDatas::SetLabel(EHandLabel InLabel)
{
	Label = InLabel;
}

void UHandDatas::Update(FString LandmarksCoordinates)
{
	Landmarks.Empty();
	TArray<FString> CoordinatesAsString;
	LandmarksCoordinates.ParseIntoArray(CoordinatesAsString, TEXT("/"));
	int index = 0;
	for (int i = 0; i < 21; i++)
	{
		FVector3f CoordinatesAsVector;
		CoordinatesAsVector.InitFromString(CoordinatesAsString[i]);
		FString Name = EnumToString("EHandLandmark", EHandLandmark(index));
		Landmarks.Add(TPair<EHandLandmark, FVector3f>(EHandLandmark(index), CoordinatesAsVector));
		index++;
	}
	for (TPair<EHandLandmark, FVector3f> Pair : Landmarks)
	{
		// UE_LOG(LogTemp, Warning, TEXT("%hhd %s"), Pair.Key, *Pair.Value.ToString());
	}
}

TArray<FVector3f> UHandDatas::GetLandmarksCoordinates()
{
	TArray<FVector3f> LandmarksCoordinates;
	for (TPair<EHandLandmark, FVector3f> Pair : Landmarks)
	{
		LandmarksCoordinates.Add(Pair.Value);
	}
	return LandmarksCoordinates;
}
