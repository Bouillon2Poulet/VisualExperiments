// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NoiseGenerator.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTextureUpdated);

UCLASS(Blueprintable)
class WEBCAM_API UNoiseGenerator : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	UTexture2D* NoiseTexture;
	
	UPROPERTY()
	TArray<float> Values;

	int Width;
	int Height;
	float Scale;

public:
	UFUNCTION(BlueprintCallable)
	void GenerateNoiseTexture(int32 Width, int32 Height, float Scale, float U, float V);
	UFUNCTION(BlueprintCallable)
	void UpdateNoiseTexture(float U, float V);
	
	UPROPERTY(BlueprintAssignable)
	FOnTextureUpdated OnTextureGenerated;
	UPROPERTY(BlueprintAssignable)
	FOnTextureUpdated OnTextureUpdated;
	UFUNCTION(BlueprintCallable)
	UTexture2D* GetNoiseTexture();
	UFUNCTION(BlueprintCallable)
	TArray<float> GetValues();
};
