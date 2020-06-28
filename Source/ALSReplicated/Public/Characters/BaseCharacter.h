

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include "BaseCharacter.generated.h"

UCLASS()
class ALSREPLICATED_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
	bool bIsMoving = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
	bool bHasMovementInput = false;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
    FRotator ControlRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
	FRotator LastVelocityRotation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
	FRotator LastMovementInputRotation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
	FRotator TargetRotation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
	FRotator CharacterRotation;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
	FVector_NetQuantize10 Acceleration;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
	FVector CharacterMovementCurrentAcceleration;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
	FVector PreviousVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
	float MovementInputAmount = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
	float AimYawRate = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Essential|Information")
	float PreviousAimYaw = 0.f;
	
protected:
	UFUNCTION(BlueprintPure, Category = "MovementSystem")
	FORCEINLINE float GetSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Network")
	FORCEINLINE bool IsLocallyOrHasAuthority() const
	{
		return HasAuthority() || IsLocallyControlled();
	}

	UFUNCTION(BlueprintPure, Category = "MovementSystem")
	FVector GetCharacterMovementCurrentAcceleration() const;

	UFUNCTION(BlueprintPure, Category = "MovementSystem")
	FORCEINLINE FVector CalculateAcceleration() const
	{
		return (GetVelocity() - PreviousVelocity) / UGameplayStatics::GetWorldDeltaSeconds(this);	
	}
	
	
protected:
	UFUNCTION(BlueprintCallable, Category = "Essential|Information")
	virtual void SetEssentialValues();

	UFUNCTION(BlueprintCallable, Category = "Essential|Information")
	virtual void CacheValue();

	UFUNCTION(BlueprintCallable, Category = "Network|MovementSystem", Unreliable, Server, WithValidation)
	void Server_SetAcceleration(const FVector_NetQuantize10& NewAcceleration);
	
	UFUNCTION(BlueprintCallable, Category = "RotationSystem")
	virtual void SmoothCharacterRotation(const FRotator& Target, const float TargetInterpSpeed, const float ActorInterpSpeed);
};
