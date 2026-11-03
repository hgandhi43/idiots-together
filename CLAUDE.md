# CLAUDE.md — Idiots Together

Put this file at the repo root. Claude Code reads it at the start of every session.

## What this project is
Idiots Together is an online co-op chaos party game for 2–6 players. Players get a mission briefing, plan for about 5 minutes, then execute for about 3 minutes. Physics, time pressure and human mistakes make the execution go wrong. Core philosophy: **Plan perfectly. Execute terribly.** The full design brief is in `docs/GAME_DESIGN.md`. Read it before any design-level change.

- Engine: Unreal Engine 5.8 (project file: `Game/IdiotsTogether.uproject`)
- Language: C++ first. Blueprints are used only for content and tuning.
- Networking: native Unreal listen server. **Server-authoritative for everything.**
- First platform: Steam (PC). Do not hard-wire Steam into gameplay code, because a PlayStation port should stay possible.
- Target hardware: GTX 1650 4 GB / 16 GB RAM. No Lumen, no Nanite (see DefaultEngine.ini).

## Architecture principle (non-negotiable)
**Build systems once. Build scenarios from configuration.**
New scenarios must not need new C++ unless a missing *reusable* mechanic is found. When one is found, build it as a generic component, not a scenario-specific hack.

Module layout under `Game/Source/IdiotsTogether/`:
- `Core/`: GameMode, GameState, PlayerState, PlayerController, Character, types, scenario definition
- `Interaction/`: interactable interface, interactables, carry system
- `Objectives/`, `Triggers/`, `Events/`, `Chaos/`, `Randomization/`: add these as the roadmap reaches them
- `UI/`: C++ widget base classes (UMG layouts live in Content)
- `Online/`: session, lobby and voice abstraction. This is the **only** place platform SDK calls may appear.

## Coding rules
- Class prefix is `IdiotsTogether` (for example `AIdiotsTogetherDoor`). Keep it consistent.
- Every gameplay state change happens on the server. Clients send `Server` RPCs that state their *intent* and receive replicated state back. Never trust client-supplied targets or values. Re-check range, phase and ownership on the server.
- Prefer replicated properties with `OnRep_` over multicast RPCs for persistent state. Use unreliable multicasts only for cosmetic one-offs (sounds, VFX).
- Scenario and objective IDs: move to `FName` or Gameplay Tags (planned in Phase 0). Do not add new `FString` IDs.
- Dev or cheat functionality must be wrapped in `#if !UE_BUILD_SHIPPING` and must never be reachable in Shipping builds.
- Logging: use the `LogIdiotsTogether` category, or a sub-category per system. Remove milestone-tagged spam logs (`[M3D ...]`) when touching a file. Use `Verbose` for noise and `Warning` only for real warnings.
- No new engine plugins without stating why in the PR or commit message.

## Binary assets: important limitation
`.uasset` and `.umap` files are binary and stored in Git LFS. **You (Claude Code) cannot meaningfully edit them.** Therefore:
1. Put logic in C++ and expose tunables as `UPROPERTY(EditDefaultsOnly)` so a human only has to set values.
2. When editor work is needed (creating a Blueprint subclass, placing actors, building a level, creating a Data Asset), end your task with a section titled **`EDITOR STEPS FOR HUMAN`**: a numbered list a non-programmer can follow exactly, including asset paths and names.
3. Where it saves real time, you may write Unreal Editor Python scripts (in `Game/Scripts/`) that create or configure assets. A human runs them in the editor.

## Build & test
- Compile after every change. On Windows, from repo root:
  `"<UE_ROOT>/Engine/Build/BatchFiles/Build.bat" IdiotsTogetherEditor Win64 Development -Project="%CD%/Game/IdiotsTogether.uproject" -WaitMutex`
  (Set `UE_ROOT` in your local env. Do not commit machine paths.)
- Automation tests live in `Source/IdiotsTogether/Tests/` and use the `IdiotsTogether.*` prefix. Run headless:
  `UnrealEditor-Cmd.exe <uproject> -ExecCmds="Automation RunTests IdiotsTogether; Quit" -unattended -nullrhi -nosplash`
- A task is **not done** until: it compiles with zero new warnings, the tests pass, and you have listed the multiplayer manual test (PIE, Play As Listen Server, 2–4 clients) that a human should run.

## Workflow per task
1. Restate the task and its acceptance criteria. Ask if anything is ambiguous **before** coding.
2. Read the existing code you will touch. Don't duplicate systems that already exist.
3. Implement in small commits with message format `P<phase>.<task>: <summary>`.
4. Finish with: what changed, files touched, how to test (single-player plus multiplayer), EDITOR STEPS FOR HUMAN, and known limitations.

## Scope guardrails (from the design brief)
The game is NOT: open world, RPG, shooter, survival/crafting, class-based, story-heavy, or a job simulator. Characters (Boss, Nerd, Muscle, Intern) are **cosmetic identities with identical abilities.** If a request drifts toward any of these, flag it.
Test for every feature: *Does this make the multiplayer experience more entertaining without unnecessarily increasing development complexity?*
