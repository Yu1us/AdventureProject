# Repository Guidelines

## Project Structure & Module Organization

This is an Unreal Engine 5.7 C++ project. The project descriptor is `AdventureProject.uproject`, and the Visual Studio solution is `AdventureProject.sln`.

- `Source/AdventureProject/` contains the runtime module.
- `Source/AdventureProject/Public/` holds public headers; `Private/` holds implementation files.
- `Source/AdventureProject/Private/Data/` and `Public/Data/` contain item data types; `Tools/` contains tool implementations such as `DartLauncher`.
- `Config/` stores Unreal project configuration, including input, engine, editor, and game defaults.
- `Content/` stores Unreal assets, with current top-level groups such as `Characters`, `Input`, `FirstPerson`, `Weapons`, and `Variant_Shooter`.
- Do not edit or commit generated local output from `.vs/`, `Binaries/`, `Intermediate/`, `DerivedDataCache/`, or `Saved/`.

## Build, Test, and Development Commands

Use the Unreal Engine version associated with the project (`5.7`).

- Unreal Engine install path: `D:\Program\Epic\UE_5.7`
- Open locally: `D:\Program\Epic\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe AdventureProject.uproject`
- Build editor target:
  `& 'D:\Program\Epic\UE_5.7\Engine\Build\BatchFiles\Build.bat' AdventureProjectEditor Win64 Development -Project="D:\Project\FirstPD\AdventureProject\AdventureProject.uproject" -WaitMutex -FromMsBuild`
- Build game target:
  `& 'D:\Program\Epic\UE_5.7\Engine\Build\BatchFiles\Build.bat' AdventureProject Win64 Development -Project="D:\Project\FirstPD\AdventureProject\AdventureProject.uproject" -WaitMutex`
- Run automation tests, once tests exist:
  `& 'D:\Program\Epic\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe' "D:\Project\FirstPD\AdventureProject\AdventureProject.uproject" -ExecCmds="Automation RunTests AdventureProject; Quit" -unattended -nop4 -NullRHI`

## Coding Style & Naming Conventions

Follow Unreal C++ conventions already used in `Source/AdventureProject`: tabs for indentation, PascalCase for classes, methods, and reflected properties, and Unreal prefixes such as `A`, `U`, `F`, and `E`. Use `TObjectPtr` for UObject references where appropriate. Place reflected fields and callable methods behind `UPROPERTY` and `UFUNCTION` macros with clear `Category` values. Keep headers in `Public/`, implementations in `Private/`, and include the matching header first in each `.cpp`.

Use common asset prefixes in `Content/`, for example `BP_`, `ABP_`, `IA_`, `IMC_`, and `DA_`.

## Testing Guidelines

No automated tests are currently present. Add Unreal Automation tests for reusable gameplay logic under a dedicated `Tests` folder in the module, and name tests around behavior, not implementation details. Before submitting gameplay changes, verify an editor build and manually exercise the affected Blueprint/assets in Play-in-Editor.

## Commit & Pull Request Guidelines

Recent history uses conventional prefixes such as `feat:` with short Chinese summaries. Continue using concise prefixes like `feat:`, `fix:`, `docs:`, or `test:`.

Pull requests should include a short change summary, affected source/assets/config paths, build or test results, and screenshots or short captures for visible gameplay or asset changes. Link related issues when available.
