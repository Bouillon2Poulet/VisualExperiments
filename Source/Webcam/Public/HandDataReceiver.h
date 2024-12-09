#pragma once

#include "CoreMinimal.h"
#include "IPAddress.h"
#include "HandDataReceiver.generated.h"

UCLASS()
class WEBCAM_API UHandDataReceiver : public UObject
{
	GENERATED_BODY()
    
public:
	void StartPythonScript();
	UFUNCTION()
	void StartReceiver();
	void TryToConnectToServer();
	void ReceiveData();
	
	FString ReceivedDataAsString;

private:
	FSocket* ReceiverSocket;
	FTimerHandle TimerHandleReceiver;
	double LastReceivedTimer = 0;

	TSharedPtr<FInternetAddr> Addr;
	const FString& IP = "127.0.0.1";
	const int32 Port = 9999;
	bool Connected = false;

	FString AccumulatedData;
};
