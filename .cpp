#include "SimpleEnemy.h"
#include "NavigationSystem.h"

ASimpleEnemy::ASimpleEnemy()
{
	Eyes = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Eyes"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.0f;
	SightConfig->LoseSightRadius = 1550.0f;
	SightConfig->PeripheralVisionAngleDegrees = 75.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	Eyes->ConfigureSense(*SightConfig);
	Eyes->SetDominantSense(SightConfig->GetSenseImplementation());
	Eyes->OnTargetPerceptionUpdated.AddDynamic(this, &ASimpleEnemy::Sighted);
}
void ASimpleEnemy::Sighted(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		SightConfig->SightRadius = 4000.0f;
		Eyes->ConfigureSense(*SightConfig);
		UE_LOG(LogTemp, Warning, TEXT("Enemy: Hostile Sighted!"));
		if (Actor && Stimulus.WasSuccessfullySensed())
		{
			GetWorldTimerManager().ClearTimer(InvestigatingHandle);
			GetWorldTimerManager().ClearTimer(GawkingHandle);
			Searches = 0;
			MoveToActor(Actor);
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Lost visual contact. Checking last known position."));
		LastKnownPosition = Stimulus.StimulusLocation;
		MoveToLocation(LastKnownPosition);
		GetWorldTimerManager().SetTimer(InvestigatingHandle, this, &ASimpleEnemy::Investigating, 0.2f, true);
	}
}
void ASimpleEnemy::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (InPawn)
	{
		Post = InPawn->GetActorLocation();
		Investigation = Post;
	}
}
void ASimpleEnemy::Gawking()
{
	if (Searches == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reached Last Point of contact. Commencing Search."));
	}
	if (Searches < 2)
	{
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (NavSys)
		{
			FNavLocation NextSearchPoint;
			if (NavSys->GetRandomReachablePointInRadius(LastKnownPosition, 1000.0f, NextSearchPoint))
			{
				GetWorldTimerManager().SetTimer(GawkingHandle, this, &ASimpleEnemy::Gawking, 5.0f, true);
				Searches++;
				MoveToLocation(NextSearchPoint.Location);
			}
		}
	}
				
	else if(Searches >= 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to locate hostile. Returning to post."));
		MoveToLocation(Post);
		GetWorldTimerManager().ClearTimer(GawkingHandle);
	}
			
}
void ASimpleEnemy::Investigating()
{
	APawn* MyPawn = GetPawn();
	if (MyPawn)
	{
		float Distance = FVector::Dist(MyPawn->GetActorLocation(), LastKnownPosition);
		if (Distance <= 100.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy: Huh..? Where'd he go?"));
			GetWorldTimerManager().ClearTimer(InvestigatingHandle);
			GetWorldTimerManager().SetTimer(GawkingHandle, this, &ASimpleEnemy::Gawking, 2.0f, true);
		}

	}
}
//Copy Paste for output log --> UE_LOG(LogTemp, Warning, TEXT(""));
