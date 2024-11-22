#pragma once
        
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PreOpenCVHeaders.h"
#include "OpenCVHelper.h"
#include <ThirdParty/OpenCV/include/opencv2/imgproc.hpp>
#include <ThirdParty/OpenCV/include/opencv2/highgui/highgui.hpp>
#include <ThirdParty/OpenCV/include/opencv2/core.hpp>

#include "Hand3D.h"
#include "HandDataReceiver.h"
#include "PostOpenCVHeaders.h"
#include "MyActor.generated.h"

class UHandDatas;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRefreshUI, const TArray<FVector>&, Landmarks, FString, HandType);

UCLASS()
class WEBCAM_API AMyActor : public AActor
{
	GENERATED_BODY()

public: 
	// Sets default values for this actor's properties
	AMyActor();
	// void TestOpenCV();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;


	cv::VideoCapture* Capture;
	UPROPERTY()
	UTexture2D* DynamicTexture;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DisplayMesh;
	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	UPROPERTY()
	UHandDataReceiver* HandDataReceiver;

	// UPROPERTY()
	// AHand3D* RightHand;
	// UPROPERTY()
	// AHand3D* LeftHand;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHand3D> Hand3DClass;

public: 
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// void CaptureFrame();
	// void UpdateTexture(const cv::Mat& Frame);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRefreshUI OnRefreshUI;

	UPROPERTY(EditAnywhere)
	int BlurLevel = 35;
};