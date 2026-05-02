#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AIController.h"
#include "SimpleEnemy.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT3_API ASimpleEnemy : public AAIController
{
	GENERATED_BODY()
	
public:
	ASimpleEnemy();
	//keep components below
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Posted Location")
	FVector Post;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Posted Location")
	FVector PatrolPoint;
	FVector LastKnownPosition;
	FVector Investigation;

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	//logic parts below here
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Senses", meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent* Eyes;
	class UAISenseConfig_Sight* SightConfig;


	UFUNCTION() 
	void Sighted(AActor* Actor, FAIStimulus Stimulus);
	void Investigating();
	void Gawking();
	FTimerHandle LostSightHandle;
	FTimerHandle InvestigatingHandle;
	FTimerHandle GawkingHandle;
	int32 Searches;
};
