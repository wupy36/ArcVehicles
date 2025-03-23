# ArcVehicles

[![License](https://img.shields.io/badge/License-Custom-blue.svg)](LICENSE)

## Overview

ArcVehicles is a comprehensive, production-ready vehicle system for Unreal Engine projects. It provides a modular, network-ready framework for implementing multi-seat vehicles with advanced player interactions.

## Features

- **Advanced Seat System**: Support for driver and multiple passenger seats
- **Player Integration**: Seamless player entry, exit, and seat switching
- **Network Ready**: Full replication support for multiplayer games
- **Physics Integration**: Custom collision handling between players and vehicles
- **Modular Design**: Easily extendable for different vehicle types

## Requirements

- Unreal Engine 4.25 or newer
- C++ project (Blueprint-only projects need to add C++ support)

## Installation

1. Create a `Plugins` folder in your project's root directory if it doesn't exist already
2. Clone or download this repository into the `Plugins` folder
3. Rename the folder to `ArcVehicles`
4. Regenerate your project files (right-click your .uproject file and select "Generate Visual Studio Project Files")
5. Open your project, the plugin should be automatically enabled

Or add as a git submodule:
```bash
git submodule add https://github.com/your-username/ArcVehicles.git Plugins/ArcVehicles
```

## Quick Start

### 1. Create a Vehicle Blueprint

1. Create a new Blueprint based on `ArcBaseVehicle`
2. Add mesh components for your vehicle
3. Configure the driver seat in the "Vehicle Config" category
4. Add additional passenger seats as needed
5. Add exit points using `ArcVehicleExitPoint` components

### 2. Set Up Player Integration

1. Ensure your player character has the `ArcVehiclePlayerSeatComponent`
2. Implement input handling to detect vehicles and request entry/exit
3. Add seat switching functionality

### 3. Basic Usage Example

```cpp
// Requesting to enter a vehicle
void AMyCharacter::AttemptEnterVehicle()
{
    AArcBaseVehicle* NearbyVehicle = GetNearbyVehicle();
    if (NearbyVehicle)
    {
        NearbyVehicle->RequestEnterAnySeat(GetPlayerState());
    }
}

// Requesting to exit a vehicle
void AMyCharacter::AttemptExitVehicle()
{
    if (CurrentVehicle)
    {
        CurrentVehicle->RequestLeaveVehicle(GetPlayerState());
    }
}

// Switching to a specific seat
void AMyCharacter::SwitchToSeat(int32 SeatIndex)
{
    if (CurrentVehicle)
    {
        CurrentVehicle->RequestEnterSeat(GetPlayerState(), SeatIndex);
    }
}
```

## Core Components

### Vehicle Components

- **AArcBaseVehicle**: Base vehicle class that manages seats and player interactions
- **UArcVehicleSeatConfig**: Configuration for individual seats
- **AArcVehicleSeat**: Optional dedicated pawns for complex seat functionality
- **UArcVehicleExitPoint**: Component for defining where players exit the vehicle

### Player Components

- **UArcVehiclePlayerSeatComponent**: Component for managing player interactions with vehicles
- **UArcVehiclePlayerStateComponent**: Component for preserving player state during vehicle use

## Advanced Usage

### Custom Seat Types

Create custom seat configurations by extending the seat config classes:

```cpp
UCLASS()
class UMyGunnerSeatConfig : public UArcVehicleSeatConfig_SeatPawn
{
    GENERATED_BODY()
public:
    // Override seat behavior
    virtual void AttachPlayerToSeat(APlayerState* Player) override;
    
    // Add custom properties
    UPROPERTY(EditAnywhere, Category="Weapons")
    TSubclassOf<AWeapon> WeaponClass;
};
```

### Custom Vehicle Types

Create specialized vehicle types with custom functionality:

```cpp
UCLASS()
class AMyHelicopter : public AArcBaseVehicle
{
    GENERATED_BODY()
public:
    // Add helicopter-specific components
    UPROPERTY(VisibleAnywhere, Category="Components")
    UArcVehicleTurretMovementComp* RotorMovement;
    
    // Override input handling
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
};
```

## Networking Considerations

- Vehicle state changes are server-authoritative
- Player entry/exit requests are validated on the server
- State changes are replicated to clients
- Custom properties should be marked for replication

## Examples

The plugin includes several example blueprints demonstrating common use cases:

- `BP_BasicCar`: Simple four-seat vehicle
- `BP_Tank`: Multi-crew vehicle with specialized seats
- `BP_Character_Vehicle`: Character blueprint with vehicle interaction

## License

Copyright © 2017-2020 Puny Human, All Rights Reserved.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Support

For issues, feature requests, or questions, please open an issue on GitHub.

## Acknowledgments

- Built by Puny Human
- Special thanks to all contributors and testers
