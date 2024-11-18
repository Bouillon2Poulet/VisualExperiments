// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HandDatas.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EHandLandmark : uint8
{
	WRIST UMETA(DisplayName = "Wrist"),
	THUMB_CMC UMETA(DisplayName = "Thumb CMC"),
	THUMB_MCP UMETA(DisplayName = "Thumb MCP"),
	THUMB_IP UMETA(DisplayName = "Thumb IP"),
	THUMB_TIP UMETA(DisplayName = "Thumb Tip"),
	INDEX_FINGER_MCP UMETA(DisplayName = "Index Finger MCP"),
	INDEX_FINGER_PIP UMETA(DisplayName = "Index Finger PIP"),
	INDEX_FINGER_DIP UMETA(DisplayName = "Index Finger DIP"),
	INDEX_FINGER_TIP UMETA(DisplayName = "Index Finger Tip"),
	MIDDLE_FINGER_MCP UMETA(DisplayName = "Middle Finger MCP"),
	MIDDLE_FINGER_PIP UMETA(DisplayName = "Middle Finger PIP"),
	MIDDLE_FINGER_DIP UMETA(DisplayName = "Middle Finger DIP"),
	MIDDLE_FINGER_TIP UMETA(DisplayName = "Middle Finger Tip"),
	RING_FINGER_MCP UMETA(DisplayName = "Ring Finger MCP"),
	RING_FINGER_PIP UMETA(DisplayName = "Ring Finger PIP"),
	RING_FINGER_DIP UMETA(DisplayName = "Ring Finger DIP"),
	RING_FINGER_TIP UMETA(DisplayName = "Ring Finger Tip"),
	PINKY_MCP UMETA(DisplayName = "Pinky MCP"),
	PINKY_PIP UMETA(DisplayName = "Pinky PIP"),
	PINKY_DIP UMETA(DisplayName = "Pinky DIP"),
	PINKY_TIP UMETA(DisplayName = "Pinky Tip")
};

template<typename T>
static FString EnumToString(const FString& enumName, const T value)
{
	UEnum* pEnum = FindObject<UEnum>(ANY_PACKAGE, *enumName);
	return *(pEnum ? pEnum->GetNameStringByIndex(static_cast<uint8>(value)) : "null");
}

UENUM(BlueprintType)
enum class EHandLabel : uint8
{
	LEFT UMETA(DisplayName = "Left"),
	RIGHT UMETA(DisplayName = "Right")
};

UCLASS()
class WEBCAM_API UHandDatas : public UObject
{
	GENERATED_BODY()
protected:
	EHandLabel Label;
	TArray<TPair<EHandLandmark,FVector3f>> Landmarks;

public:
	void SetLabel(EHandLabel Label);
	void Update(FString LandmarksCoordinates);
	TArray<FVector3f> GetLandmarksCoordinates();
};
