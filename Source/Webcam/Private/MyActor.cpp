#include "MyActor.h"

#include "HandDatas.h"
#include "HandDataReceiver.h"
#include "Misc/AssertionMacros.h"

// Sets default values
AMyActor::AMyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	RootComponent = DisplayMesh;

	// Assigner un maillage par défaut (ex: un plan)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
	if (PlaneMesh.Succeeded())
	{
		DisplayMesh->SetStaticMesh(PlaneMesh.Object);
	}
}

void AMyActor::TestOpenCV()
{
	UE_LOG(LogTemp, Log, TEXT("Testing OpenCV..."));
	int testDim[3] = {2, 3, 4};
	cv::Mat testMat(3, testDim, CV_32FC1);
	UE_LOG(
		LogTemp, Log,
		TEXT("dimension = %d, %d, %d"),
		testMat.size[0], testMat.size[1], testMat.size[2]);
	UE_LOG(LogTemp, Log, TEXT("Testing Done!"));
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
	HandDataReceiver->StartPythonScript();
	HandDataReceiver->StartReceiver();
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// CaptureFrame();
	//
	if (HandDataReceiver->ReceivedDataAsString.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("No data received"));
		return;
	}

	TArray<FString> DataParsed;
	HandDataReceiver->ReceivedDataAsString.ParseIntoArray(DataParsed, TEXT(":"));
	UE_LOG(LogTemp, Warning, TEXT("Type de main %s"), *DataParsed[0]);
	if (DataParsed.Contains("Right"))
	{
		RightHand->Update(DataParsed[1]);
	}
	else if (DataParsed.Contains("Left"))
	{
		LeftHand->Update(DataParsed[1]);
	}
}

void AMyActor::CaptureFrame()
{
	if (!Capture->isOpened()) return;

	cv::Mat Frame;
	*Capture >> Frame;

	if (Frame.empty()) return;

	// Convertir le format BGR de OpenCV en BGRA pour Unreal
	cv::cvtColor(Frame, Frame, cv::COLOR_BGR2GRAY);
	cv::blur(Frame, Frame, cv::Size(BlurLevel, BlurLevel));
	cv::threshold(Frame, Frame, 127, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

	////Fingers
	// // Trouver les contours
	// std::vector<std::vector<cv::Point>> contours;
	// std::vector<cv::Vec4i> hierarchy;
	// cv::findContours(Frame, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
	//
	// // Trouver le plus grand contour en fonction de l'aire
	// auto max_contour = std::max_element(contours.begin(), contours.end(), [](const std::vector<cv::Point>& c1, const std::vector<cv::Point>& c2) {
	// 	return cv::contourArea(c1) < cv::contourArea(c2);
	// });
	//
	// if (max_contour == contours.end()) {
	// 	UE_LOG(LogTemp, Log, TEXT("No contours found!"));
	// }
	//
	// // Calculer le rectangle englobant
	// cv::Rect bounding_box = cv::boundingRect(*max_contour);
	// cv::rectangle(Frame, bounding_box, cv::Scalar(0, 0, 255), 1);
	//
	// // Calculer la coque convexe du plus grand contour
	// std::vector<cv::Point> hull;
	// cv::convexHull(*max_contour, hull);
	//
	// // Dessiner le contour et la coque convexe sur une image vide
	// cv::Mat drawing = cv::Mat::zeros(Frame.size(), CV_8UC3);
	// cv::drawContours(drawing, contours, std::distance(contours.begin(), max_contour), cv::Scalar(0, 255, 0), 1);
	// cv::drawContours(drawing, std::vector<std::vector<cv::Point>>{hull}, 0, cv::Scalar(0, 0, 255), 1);
	//
	// Frame += drawing;


	//Format final pour UE
	cv::cvtColor(Frame, Frame, cv::COLOR_BGR2BGRA);


	// Mette à jour la texture dynamique avec le buffer d'OpenCV
	UpdateTexture(Frame);
}

void AMyActor::UpdateTexture(const cv::Mat& Frame)
{
	if (!DynamicTexture || Frame.empty() || Frame.channels() != 4) return;

	// Créer une région de mise à jour pour la texture
	FUpdateTextureRegion2D UpdateRegion(0, 0, 0, 0, Frame.cols, Frame.rows);

	// Vérifier la taille du buffer et aligner la copie des données
	uint8* MipData = static_cast<uint8*>(DynamicTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	// Copier les données du Frame dans MipData, ligne par ligne
	for (int y = 0; y < Frame.rows; y++)
	{
		FMemory::Memcpy(MipData + y * Frame.cols * 4, Frame.ptr(y), Frame.cols * 4);
	}

	DynamicTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	DynamicTexture->UpdateResource();
}


void AMyActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (Capture)
	{
		Capture->release();
	}
}
