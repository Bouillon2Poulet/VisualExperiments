﻿#include "HandDataReceiver.h"

#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Sockets.h"
#include "TimerManager.h"


void UHandDataReceiver::StartPythonScript()
{
	// // Chemin vers l'exécutable Python et le script à lancer
	// FString PythonExe = TEXT("C:/Users/Rom1/AppData/Local/Programs/Python/Python312/python.exe");
	// FString ScriptPath = FPaths::ConvertRelativePathToFull(
	// 	TEXT("C:/Users/Rom1/Documents/Unreal Projects/VisualExperiments/Content/Python/HandRecognition.py"));
	//
	// // Créer le processus
	// FProcHandle ProcHandle = FPlatformProcess::CreateProc(*PythonExe, *ScriptPath, true, false, false, nullptr, 0,
	//                                                       nullptr, nullptr);
	
	// FString Command = TEXT("C:/Windows/System32/WindowsPowerShell/v1.0/powershell.exe");
	// FString Params = TEXT("& C:/Users/Rom1/AppData/Local/Programs/Python/Python312/python.exe 'c:/Users/Rom1/Documents/Unreal Projects/VisualExperiments/Content/Python/HandRecognition.py'");
	
	// FString Command = TEXT("C:/Users/Rom1/AppData/Local/Programs/Python/Python312/python.exe");
	// FString Params = TEXT("C:/Users/Rom1/Documents/Unreal Projects/VisualExperiments/Content/Python/HandRecognition.py");
	//
	// FString FullCommand = FString::Printf(TEXT("\"%s\" \"%s\""), *Command, *Params);
	// UE_LOG(LogTemp, Warning, TEXT("Attempting to execute command: %s"), *FullCommand);
	//
	// // Ouverture du terminal et exécution du script Python
	// // FProcHandle ProcHandle = FPlatformProcess::CreateProc(*Command, *Params, true, false, false, nullptr, 0, nullptr, nullptr);
	// FPlatformProcess::ExecProcess(*Command, *Params, nullptr, nullptr, nullptr);
	//
	// // Vérifier si le processus a démarré correctement
	// // if (ProcHandle.IsValid())
	// // {
	// //     UE_LOG(LogTemp, Log, TEXT("Python script is running. Waiting..."));
	// //     FPlatformProcess::WaitForProc(ProcHandle); // Attend la fin du script
	// // }
}

void UHandDataReceiver::StartReceiver()
{
	// Création du socket
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	FIPv4Address IPAddress;
	FIPv4Address::Parse(IP, IPAddress);

	// Configuration de l'adresse de connexion
	TSharedRef<FInternetAddr> AddrRef = SocketSubsystem->CreateInternetAddr();
	AddrRef->SetIp(IPAddress.Value);
	AddrRef->SetPort(Port);
	Addr = AddrRef.ToSharedPtr();
	ReceiverSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("HandDataReceiver"), false);

	// Lancer le timer pour recevoir les données
	FTimerHandle TimerHandleReceiver;
	if(GetOuter()->GetWorld())
	{
		GetOuter()->GetWorld()->GetTimerManager().SetTimer(TimerHandleReceiver, this, &UHandDataReceiver::ReceiveData, 1.f, true);
	}
}

void UHandDataReceiver::ReceiveData()
{
	if (!Connected)
	{
		// Création du socket
		Connected = ReceiverSocket->Connect(*Addr);
		if(Connected)
		{
			UE_LOG(LogTemp, Warning, TEXT("Connexion établie !"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to connect :("));
			return;
		}
	}

	ReceivedDataAsString.Empty();
	
	TArray<uint8> ReceivedData;
	uint32 Size;
	while (ReceiverSocket->HasPendingData(Size))
	{
		ReceivedData.SetNumUninitialized(Size);
		int32 Read = 0;
		ReceiverSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

		// Conversion des données reçues
		ReceivedDataAsString = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));

		// Traitement des données
		// UE_LOG(LogTemp, Warning, TEXT("Coordonnées reçues : %s"), *ReceivedDataAsString);
	}
}