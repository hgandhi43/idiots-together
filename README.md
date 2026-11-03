# Idiots Together

Online co-op chaos party game for 2–6 players. **Plan perfectly. Execute terribly.**

- Design brief: [docs/GAME_DESIGN.md](docs/GAME_DESIGN.md)
- Roadmap & task list: [docs/ROADMAP.md](docs/ROADMAP.md)
- Rules for contributors and Claude Code: [CLAUDE.md](CLAUDE.md)

## Requirements
- Unreal Engine **5.8**
- Visual Studio 2022+ with the components in [Game/.vsconfig](Game/.vsconfig) (Game development with C++, MSVC 14.50, Windows 11 SDK 22621)
- Git LFS (`git lfs install` once per machine)

## Getting started
```
git lfs install
git clone <repo-url>
cd idiots-together
git lfs pull
```
Right-click `Game/IdiotsTogether.uproject` → **Generate Visual Studio project files**, then open the generated solution and build `IdiotsTogetherEditor | Development Editor | Win64`. You can also run this from the repo root:

```
"<UE_ROOT>/Engine/Build/BatchFiles/Build.bat" IdiotsTogetherEditor Win64 Development -Project="%CD%/Game/IdiotsTogether.uproject" -WaitMutex
```

## Layout
```
Game/                    Unreal project (uproject, Config, Content, Source)
  Source/IdiotsTogether/ C++ module (Core, Interaction, UI, ...)
docs/                    Design brief, roadmap
```
