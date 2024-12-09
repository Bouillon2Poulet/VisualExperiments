// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseGenerator.h"
#include "Math/UnrealMathUtility.h" // Pour les fonctions mathématiques comme FMath::FloorToInt, FMath::Frac, etc.
#include "Engine/Texture2D.h"
#include "Rendering/Texture2DResource.h"
#include "Kismet/KismetRenderingLibrary.h"

// Fonction hash3 pour générer des valeurs pseudo-aléatoires
FVector Hash3(const FVector2D& p)
{
	FVector q(FMath::Frac(FMath::Sin(FVector2D::DotProduct(p, FVector2D(127.1f, 311.7f))) * 43758.5453f),
			  FMath::Frac(FMath::Sin(FVector2D::DotProduct(p, FVector2D(269.5f, 183.3f))) * 43758.5453f),
			  FMath::Frac(FMath::Sin(FVector2D::DotProduct(p, FVector2D(113.5f, 271.9f))) * 43758.5453f));
	return q;
}

// Fonction principale pour le bruit
float Noise(const FVector2D& x, float u, float v)
{
	FVector2D p = FVector2D(FMath::Floor(x.X), FMath::Floor(x.Y)); // Floor(x)
	FVector2D f = FVector2D(FMath::Frac(x.X), FMath::Frac(x.Y));   // Fract(x)

	float k = 1.0f + 63.0f * FMath::Pow(1.0f - v, 4.0f); // Facteur de pondération
	float va = 0.0f; // Valeur accumulée
	float wt = 0.0f; // Poids total accumulé

	for (int j = -2; j <= 2; j++)
	{
		for (int i = -2; i <= 2; i++)
		{
			FVector2D g(i, j);
			FVector o = Hash3(p + g) * FVector(u, u, 1.0f);
			FVector2D r = g - f + FVector2D(o.X, o.Y);
			float d = FVector2D::DotProduct(r, r); // Distance au carré

			// Calcul du poids en fonction de la distance
			float w = FMath::Pow(1.0f - FMath::SmoothStep(0.0f, 1.414f, FMath::Sqrt(d)), k);

			// Accumuler uniquement si le poids est significatif
			if (w > 0.0f)
			{
				va += w * o.Z;
				wt += w;
			}
		}
	}

	// Si wt == 0, retourner une valeur par défaut pour éviter NaN
	if (wt == 0.0f)
	{
		return 1.f; // Retourner une valeur neutre
	}

	return va / wt;
}


// Génère une texture 2D basée sur la fonction Noise
void UNoiseGenerator::GenerateNoiseTexture(int32 InWidth, int32 InHeight, float InScale, float U, float V)
{
	Width = InWidth;
	Height = InHeight;
	Scale = InScale;
	
	Values.Empty();
	// Créer une nouvelle texture 2D
	UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	if (!Texture) return;

	// Verrouiller la texture pour la modification
	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	FByteBulkData* RawImageData = &Mip.BulkData;
	uint8* TextureData = static_cast<uint8*>(RawImageData->Lock(LOCK_READ_WRITE));

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			// Coordonnées normalisées (0 à 1)
			float FX = X / static_cast<float>(Width) * Scale;
			float FY = Y / static_cast<float>(Height) * Scale;

			// Calculer la valeur de bruit
			float NoiseValue = Noise(FVector2D(FX, FY), U, V);

			// Débug : Écrivez dans le log pour vérifier les valeurs
			// UE_LOG(LogTemp, Warning, TEXT("Noise: %f"), NoiseValue);

			// Convertir la valeur de bruit en couleur (0-255)
			uint8 Intensity = FMath::Clamp(static_cast<int32>(NoiseValue * 255.0f), 0, 255);
			
			// Débug : Écrivez dans le log pour vérifier les valeurs
			// UE_LOG(LogTemp, Warning, TEXT("Intensity: %d"), Intensity);

			// Calcul de l'index dans le tableau
			int32 Index = (Y * Width + X) * 4;

			// Remplir les canaux (R, G, B, A)
			TextureData[Index + 0] = Intensity; // R
			TextureData[Index + 1] = Intensity; // G
			TextureData[Index + 2] = Intensity; // B
			TextureData[Index + 3] = 255;       // A

			Values.Add(Intensity);
		}
	}

	// Déverrouiller la texture
	RawImageData->Unlock();

	// Mettre à jour la texture
	Texture->UpdateResource();

	NoiseTexture = Texture;
	OnTextureGenerated.Broadcast();
}

void UNoiseGenerator::UpdateNoiseTexture(float U, float V)
{
	Values.Empty();
	
	// Verrouiller la texture pour la modification
	FTexture2DMipMap& Mip = NoiseTexture->GetPlatformData()->Mips[0];
	FByteBulkData* RawImageData = &Mip.BulkData;
	uint8* TextureData = static_cast<uint8*>(RawImageData->Lock(LOCK_READ_WRITE));

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			// Coordonnées normalisées (0 à 1)
			float FX = X / static_cast<float>(Width) * Scale;
			float FY = Y / static_cast<float>(Height) * Scale;

			// Calculer la valeur de bruit
			float NoiseValue = Noise(FVector2D(FX, FY), U, V);

			// Débug : Écrivez dans le log pour vérifier les valeurs
			// UE_LOG(LogTemp, Warning, TEXT("Noise: %f"), NoiseValue);

			// Convertir la valeur de bruit en couleur (0-255)
			uint8 Intensity = FMath::Clamp(static_cast<int32>(NoiseValue * 255.0f), 0, 255);
			
			// Débug : Écrivez dans le log pour vérifier les valeurs
			// UE_LOG(LogTemp, Warning, TEXT("Intensity: %d"), Intensity);

			// Calcul de l'index dans le tableau
			int32 Index = (Y * Width + X) * 4;

			// Remplir les canaux (R, G, B, A)
			TextureData[Index + 0] = Intensity; // R
			TextureData[Index + 1] = Intensity; // G
			TextureData[Index + 2] = Intensity; // B
			TextureData[Index + 3] = 255;       // A

			Values.Add(Intensity);
		}
	}

	// Déverrouiller la texture
	RawImageData->Unlock();

	// Mettre à jour la texture
	NoiseTexture->UpdateResource();
	OnTextureUpdated.Broadcast();
}

UTexture2D* UNoiseGenerator::GetNoiseTexture()
{
	return NoiseTexture;
}

TArray<float> UNoiseGenerator::GetValues()
{
	return Values;
}
