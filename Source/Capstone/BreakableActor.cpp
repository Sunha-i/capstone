// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollectionComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	CalculateBoneCenters();

	 TArray<int32> ClusteredIndex = { 3, 0, 0, 1, 2, 4, 3, 4, 3, 2, 3, 2 };		// sample idx for GC_10_12. start from bone idx 1
}

void ABreakableActor::CalculateBoneCenters()
{
	if (!GeometryCollectionComponent || !GeometryCollectionComponent->GetRestCollection())
	{
		UE_LOG(LogTemp, Warning, TEXT("GeometryCollectionComponent or RestCollection is invalid"));
		return;
	}

	const UGeometryCollection* RestCollection = GeometryCollectionComponent->GetRestCollection();
	const FGeometryCollection* GeometryCollection = RestCollection->GetGeometryCollection().Get();
	if (!GeometryCollection)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get GeometryCollection"));
		return;
	}

	// Get vertices group
	const TManagedArray<FVector3f>& VertexArray = GeometryCollection->GetAttribute<FVector3f>("Vertex", FGeometryCollection::VerticesGroup);
	const TManagedArray<int32>& BoneMapArray = GeometryCollection->GetAttribute<int32>("BoneMap", FGeometryCollection::VerticesGroup);
	if (VertexArray.Num() == 0 || BoneMapArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("VertexArray or BoneMapArray is empty"));
		return;
	}

	const int32 NumOfBones = GeometryCollection->NumElements(FGeometryCollection::TransformGroup);
	UE_LOG(LogTemp, Warning, TEXT("Number of unique bones: %d"), NumOfBones - 1);

	// store the sum & count of vertices per bone
	TArray<FVector3f> BoneVertexSums;
	TArray<int32> BoneVertexCounts;
	BoneVertexSums.SetNumZeroed(NumOfBones);
	BoneVertexCounts.SetNumZeroed(NumOfBones);

	for (int32 i = 0; i < VertexArray.Num(); ++i)
	{
		FVector3f Vertex = VertexArray[i];
		int32 BoneIndex = BoneMapArray[i];

		BoneVertexSums[BoneIndex] += Vertex;
		BoneVertexCounts[BoneIndex]++;

		// UE_LOG(LogTemp, Warning, TEXT("Vertex %d: Position = (%f, %f, %f), BoneIndex = %d"), i, Vertex.X, Vertex.Y, Vertex.Z, BoneIndex);
	}

	// Calculate center of mass for each bone
	for (int32 BoneIdx = 1; BoneIdx < NumOfBones; ++BoneIdx)	// 0th bone is not a piece. root of pieces
	{
		FVector3f SumOfVertices = BoneVertexSums[BoneIdx];
		int32 VertexCount = BoneVertexCounts[BoneIdx];
		if (VertexCount)
		{
			FVector3f CenterOfMass = SumOfVertices / VertexCount;
			PieceLocArr.Add(FVector(CenterOfMass.X, CenterOfMass.Y, CenterOfMass.Z));
			UE_LOG(LogTemp, Log, TEXT("Bone %d: Center of Mass = (%f, %f, %f)"), BoneIdx, CenterOfMass.X, CenterOfMass.Y, CenterOfMass.Z);

			DrawDebugSphere(
				GetWorld(),
				FVector(CenterOfMass.X, CenterOfMass.Y, CenterOfMass.Z + 76.4f),
				2.0f,		 // Sphere radius
				12,          // Number of segments
				FColor::Red, // Sphere color
				false,       // Persistent (false = only for a single frame)
				50.0f        // Lifetime (1 second)
			);
		}
	}
}

void ABreakableActor::DebugSocketInfo()
{
	// set pivot to each pieces
	auto pieceNameArr = GeometryCollectionComponent->GetAllSocketNames();

	uint16 numberOfPieces = pieceNameArr.Num() - 1;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Number of pieces: %d"), numberOfPieces));

	for (int i = 0; i < numberOfPieces; i++) {
		FVector CenterOfMass = GeometryCollectionComponent->GetSocketLocation(pieceNameArr[i]);
		// PieceLocArr.Add(CenterOfMass);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%f %f %f"), CenterOfMass.X, CenterOfMass.Y, CenterOfMass.Z));
	}
}

TArray<FVector> ABreakableActor::GetPieceLocArray() const
{
	return PieceLocArr;
}