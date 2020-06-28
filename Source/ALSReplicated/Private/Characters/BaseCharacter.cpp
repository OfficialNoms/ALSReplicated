#include "Characters/BaseCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SetReplicates(true);
    ACharacter::SetReplicateMovement(true);
    NetUpdateFrequency = 30.f;
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ABaseCharacter, ControlRotation, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(ABaseCharacter, Acceleration, COND_SimulatedOnly);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    SetEssentialValues();
    CacheValue();
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ABaseCharacter::GetSpeed() const
{
    return GetCharacterMovement()->Velocity.Size2D();
}

FVector ABaseCharacter::GetCharacterMovementCurrentAcceleration() const
{
    const FVector CurrentAcceleration = GetCharacterMovement()->GetCurrentAcceleration();
    return IsLocallyOrHasAuthority()
               ? CurrentAcceleration
               : CurrentAcceleration * GetCharacterMovement()->MaxAcceleration;
}

void ABaseCharacter::SetEssentialValues()
{
    bIsMoving = GetSpeed() > 1.f;
    if (bIsMoving)
    {
        if (IsLocallyOrHasAuthority())
        {
            LastVelocityRotation = GetCharacterMovement()->Velocity.ToOrientationRotator();
        }

        if (IsLocallyControlled())
        {
            Acceleration = CalculateAcceleration();
            Server_SetAcceleration(Acceleration);
        }
    }

    CharacterMovementCurrentAcceleration = GetCharacterMovementCurrentAcceleration();
    MovementInputAmount = CharacterMovementCurrentAcceleration.Size() / GetCharacterMovement()->MaxAcceleration;
    bHasMovementInput = MovementInputAmount > 0.f;
    if (bHasMovementInput && IsLocallyOrHasAuthority())
    {
        LastMovementInputRotation = CharacterMovementCurrentAcceleration.ToOrientationRotator();
    }

    if (IsLocallyOrHasAuthority())
    {
        const FRotator Rotation = GetControlRotation();
        if (UKismetMathLibrary::EqualEqual_RotatorRotator(Rotation, ControlRotation, 0.01f))
        {
            ControlRotation = Rotation;
        }
    }


    AimYawRate = UKismetMathLibrary::Round(UKismetMathLibrary::Abs((ControlRotation.Yaw - PreviousAimYaw) /
        UGameplayStatics::GetWorldDeltaSeconds(this)));
}

void ABaseCharacter::CacheValue()
{
    if (IsLocallyControlled())
    {
        PreviousVelocity = GetVelocity();
    }

    PreviousAimYaw = ControlRotation.Yaw;
}

void ABaseCharacter::SmoothCharacterRotation(const FRotator& Target, const float TargetInterpSpeed,
                                             const float ActorInterpSpeed)
{
    const float WorldDelta = UGameplayStatics::GetWorldDeltaSeconds(this);
    TargetRotation = UKismetMathLibrary::RInterpTo_Constant(TargetRotation, Target, WorldDelta, TargetInterpSpeed);

    const FRotator ActorRotation = UKismetMathLibrary::RInterpTo(CharacterRotation, TargetRotation, WorldDelta,
                                                                 ActorInterpSpeed);

    SetActorRotation(ActorRotation);
    CharacterRotation = GetActorRotation();
}

bool ABaseCharacter::Server_SetAcceleration_Validate(const FVector_NetQuantize10& NewAcceleration)
{
    return NewAcceleration != Acceleration;
}

void ABaseCharacter::Server_SetAcceleration_Implementation(const FVector_NetQuantize10& NewAcceleration)
{
    Acceleration = NewAcceleration;
}
