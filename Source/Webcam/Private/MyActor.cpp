#include "MyActor.h"
#include "Hand3D.h"
#include "HandDataReceiver.h"
#include "JsonObjectConverter.h"

// Sets default values
AMyActor::AMyActor()
{
    PrimaryActorTick.bCanEverTick = true;

    DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
    RootComponent = DisplayMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
    if (PlaneMesh.Succeeded())
    {
        DisplayMesh->SetStaticMesh(PlaneMesh.Object);
    }
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
    Super::BeginPlay();

    if (GetWorld())
    {
        RightHand = GetWorld()->SpawnActor<AHand3D>(Hand3DClass);
        LeftHand = GetWorld()->SpawnActor<AHand3D>(Hand3DClass);
    }

    HandDataReceiver = NewObject<UHandDataReceiver>(this);
    HandDataReceiver->StartReceiver();
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    HandDataReceiver->ReceiveData();
    FString ReceivedData = HandDataReceiver->ReceivedDataAsString;
    if (ReceivedData.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("No data received."));
        return;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ReceivedData);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        const TArray<TSharedPtr<FJsonValue>>* HandsArray;
        if (JsonObject->TryGetArrayField(TEXT("hands"), HandsArray))
        {
            for (const TSharedPtr<FJsonValue>& HandValue : *HandsArray)
            {
                TSharedPtr<FJsonObject> HandObject = HandValue->AsObject();
                if (HandObject.IsValid())
                {
                    FString HandType = HandObject->GetStringField(TEXT("type"));

                    const TArray<TSharedPtr<FJsonValue>>* LandmarksArray;
                    if (HandObject->TryGetArrayField(TEXT("landmarks"), LandmarksArray))
                    {
                        TArray<FVector> Landmarks;
                        for (const TSharedPtr<FJsonValue>& LandmarkValue : *LandmarksArray)
                        {
                            TSharedPtr<FJsonObject> LandmarkObject = LandmarkValue->AsObject();
                            if (LandmarkObject.IsValid())
                            {
                                float X = LandmarkObject->GetNumberField(TEXT("x"));
                                float Y = LandmarkObject->GetNumberField(TEXT("y"));
                                float Z = LandmarkObject->GetNumberField(TEXT("z"));
                                Landmarks.Add(FVector(X, Y, Z));
                            }
                        }

                        if (HandType == "Right")
                        {
                            RightHand->Update(Landmarks);
                        }
                        else if (HandType == "Left")
                        {
                            LeftHand->Update(Landmarks);
                        }
                    }
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON data: %s"), *ReceivedData);
    }
}
