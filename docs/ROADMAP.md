# Idiots Together: Development Roadmap for Claude Code

Give Claude Code **one task at a time**, using the prompt template at the bottom of this file.

---

## Where the project stands (audit of `main` @ c7e44e5, "M3D")

About 2,800 lines of C++, 10 binary assets, 1 test map.

**Built and working (per commits M1 → M3D):**
- Third-person character with Enhanced Input (move, look, jump, interact) and a spring-arm camera
- Interaction interface (`GetInteractionPrompt`, `CanInteract`, `Interact`, focus begin/end), a camera line trace, a server RPC with a range check, and an interaction prompt widget
- Character selection on PlayerState with server-side uniqueness, plus "team random"; a dev selection widget
- Round lifecycle state machine in GameMode: Waiting → Briefing → Planning → Execution → Success/Failure → Results → NextRound, with replicated phase, timer and round index
- `UIdiotsTogetherScenarioDefinition` data asset (ID, name, description, phase durations, round count, objectives)
- Replicated objective states, `CompleteObjective()` with early success, an objective trigger volume, an objective component, and a phase HUD
- Dev RPCs: advance phase, force success/failure, complete objective

**Gaps and issues found (fixed in Phase 0):**
1. **Design conflict:** 2–6 players, but only 4 unique characters and "no duplicates." Team Random wraps around and gives duplicates to players 5 and 6. *Decision needed* (see Phase 0).
2. Dev cheat RPCs are callable by any client and are not stripped from Shipping builds.
3. `ServerInteract_Validate` returns true unconditionally. The server range check is loose (+200 units) and measures actor origins rather than the actual interaction point.
4. No world reset between rounds. NextRound loops the same level state, so moved or broken objects stay that way. Also, the round counter silently wraps back to 1.
5. The scenario lifecycle auto-starts in `BeginPlay`. There is no lobby, no ready-up and no gating on player count.
6. String-based IDs (`FString ObjectiveID`) are error-prone at 50-scenario scale.
7. Characters are a static mesh plus a text label. There is no skeletal mesh, animation or physics reaction yet.
8. Leftover template config (an Android File Server block, ProjectID placeholder `A1B2C3D4...`) and the folder name `GroupProject`.
9. Packaging is set to Development / not ForDistribution (fine for now, needed for Phase 10).
10. No automation tests, no online subsystem, no sessions, no voice.

---

## Scope verdict on the concept

The concept is strong and well-scoped *as a framework*. It has a clear loop, cosmetic-only characters, and a "systems once, scenarios as data" approach. The proven genre (Overcooked, Human: Fall Flat, Moving Out, Totally Reliable Delivery Service) shows the appetite is real.

The risks, in order:
1. **Networked physics.** Carrying, pushing and co-op lifting of physics objects over a listen server is the hardest technical problem in this game, and it is the core of the fun. Prototype it early (Phase 3), not late.
2. **50 scenarios.** Not a launch target. Launch (or Early Access) with **6–8 scenarios that each have a distinct chaos mechanic**, then add more in updates. Each distinct mechanic (vehicles, carrying, noise/stealth, rising water) is a real system to build. Re-skins of the same mechanic are cheap.
3. **Art and animation.** Claude Code cannot produce low-poly characters, animations or environments. Budget for a 3D artist, or for licensed low-poly packs plus retargeted animations. Comedy depends heavily on animation.
4. **Fun-finding is human work.** Claude Code can build systems. Only playtests with real friend groups can tell you whether a scenario is funny. Phase 6 is a hard go/no-go gate.
5. **6 players vs. 4 characters** (decision in Phase 0).
6. **Voice and lobbies.** Solvable with Steam or EOS services, but test them early on real networks, not only in PIE.

Rough effort, for a team of 2 using Claude Code heavily, plus outsourced art: about **2–3 months** to a playable The Con vertical slice (end of Phase 6), and about **6–10 months** to a Steam Early Access build with 6–8 scenarios. Treat these as planning ranges, not commitments.

---

## Phase overview

| Phase | Goal | Gate to move on |
|---|---|---|
| 0 | Hardening & foundations | Clean build, tests run, issues 1–8 resolved |
| 1 | Session flow: menu → lobby → scenario → back | 4 players complete a full loop over LAN |
| 2 | Scenario framework v2 | Scenario fully driven by its data asset; clean round reset |
| 3 | Interaction & physics carrying | Co-op carry feels OK with 4 clients at 150 ms simulated lag |
| 4 | Gameplay building blocks | A test scenario built with **zero** new C++ |
| 5 | Controlled randomization | 1,000 seeds auto-validated as solvable |
| 6 | Scenario #001 The Con (vertical slice) | **Go/no-go playtest:** groups laugh and want to replay |
| 7 | Characters, animation, audio, juice | Characters look funny standing still |
| 8 | Steam online & voice | Friends join through Steam invites over the internet |
| 9 | Scenario production (launch set) | 6–8 polished scenarios |
| 10 | Release preparation | Steam review passed, store page live |

---

## Phase 0: Hardening & Foundations

**Human decisions before starting:**
- **6 players vs. 4 characters.** Pick one: (a) allow duplicates of an identity with a distinct colour or outfit per player (the cheapest option, and it fits the comedy); (b) add 2 more identities (for example "Driver" and "Grandpa"); (c) cap at 4 players. Recommendation: **(a)** for launch.
- Whether to rename the `GroupProject/` folder now. Renaming later is more painful.

**Decisions taken (2026-11-03):**
- Characters: **option (a)**. Duplicate identities are allowed, and each player gets a unique colour (`PlayerColorIndex`), so 2–6 players always resolve.
- Folder: **renamed `GroupProject/` → `Game/`** in P0.1. The project file is now `Game/IdiotsTogether.uproject`.

**Tasks:**
- **P0.1 Repo hygiene.** Add `CLAUDE.md` and `docs/GAME_DESIGN.md` (the brief). Remove the Android File Server config. Generate a real ProjectID. Optionally rename the folder. Confirm the LFS rules cover every binary type in use.
- **P0.1b Fix doubled class prefixes.** The earlier rename produced `UIIdiotsTogetherInteractableInterface` / `IIIdiotsTogetherInteractableInterface` and `UIIdiotsTogetherInteractionWidget`. Rename them to `UIdiotsTogetherInteractableInterface` / `IIdiotsTogetherInteractableInterface` / `UIdiotsTogetherInteractionWidget`, add `ClassRedirects` from the old names, then open and re-save `WBP_IdiotsTogetherInteractionPrompt` (re-parent it if needed).
- **P0.2 Character identity rule.** Implement the chosen option from the decision above. The Random assignment must respect it for 2–6 players. Add per-player colour (`PlayerColorIndex` already exists but is unused).
- **P0.3 Security pass.** Wrap all dev RPCs, console commands and the dev widget in `#if !UE_BUILD_SHIPPING`, and restrict them to the host even in dev builds. Harden `ServerInteract`: validate the phase, re-trace from the server's view of the pawn, check the distance to the hit point, and rate-limit.
- **P0.4 IDs to Gameplay Tags.** Enable the GameplayTags module. Migrate objective and scenario IDs to `FGameplayTag` (or `FName`). Add a tag table under `Config/Tags/`.
- **P0.5 Logging cleanup.** Create per-system log categories, downgrade diagnostic spam to Verbose, and remove milestone tags from log strings.
- **P0.6 Test harness.** Create the `Tests/` folder with automation tests for the phase state machine (transition order, early success, timeout failure) and the character selection rules. Add a `Scripts/build.bat` and `Scripts/test.bat`.

**Done when:** a clean compile, the tests pass headless, and a 3-client PIE run shows correct and unique character assignment.

---

## Phase 1: Session Flow & Front End

Goal: the real flow, Main Menu → Host/Join → Lobby → Scenario → Results → Lobby, first over LAN or the Null subsystem.

- **P1.1 Online abstraction layer.** `UIdiotsTogetherSessionSubsystem` (a GameInstance subsystem) wraps create, find, join and destroy session. Use the Null/LAN backend now. All later Steam code goes behind this same interface (Phase 8).
- **P1.2 Maps & modes.** A `MainMenu` map (with a menu GameMode) and a `Lobby` map with `AIdiotsTogetherLobbyGameMode`. The scenario GameMode no longer auto-starts in BeginPlay. It starts when the host launches the scenario and everyone has loaded.
- **P1.3 Lobby.** Player list, ready-up, character select (moved here from the dev widget), and host-only scenario select from the registry. The Start button is enabled only when all players are ready and the count is within the scenario's min/max.
- **P1.4 Travel.** Use seamless `ServerTravel` from lobby to scenario map. Carry character choice and colour across travel via `PlayerState::CopyProperties`. Wait for all clients to load before Briefing begins.
- **P1.5 Connection edge cases.** Block joins during a running scenario (or queue them for the next round). Handle a client disconnect mid-round without breaking objectives. If the host leaves, return everyone to the main menu with a message. There is no host migration.
- **P1.6 Front-end UI (C++ bases).** Main menu, lobby, a minimal settings stub, and a pause menu. Provide functional greybox UMG only.

**Done when:** 4 standalone game instances on LAN can host, join, ready up, play a round and return to the lobby, twice in a row.

---

## Phase 2: Scenario Framework v2

- **P2.1 Scenario registry.** Make the scenario definition a Primary Data Asset discovered through the Asset Manager. Add fields: map (soft reference), category, min/max players, thumbnail, briefing text, difficulty, failure conditions, and randomization profile (used in Phase 5).
- **P2.2 Round reset.** Each attempt runs on a fresh world state. Recommended approach: reload the scenario map with `ServerTravel` between attempts (the simplest and most reliable option). Keep the round count and stats in GameInstance/PlayerState. Remove the silent wrap to round 1. When the run ends, go to the Results screen and then back to the Lobby.
- **P2.3 Phase behaviour.** In *Briefing*, show the scenario info UI and lock input. In *Planning*, players are free to walk the map, or sit in a planning room with a map overview (make this configurable per scenario). Objects cannot be touched, and voice is open. In *Execution*, everything is live. Each phase should fire hooks (`OnPhaseEntered` as a BlueprintImplementableEvent and a delegate) that any actor can use.
- **P2.4 Failure conditions.** Add a generic `UIdiotsTogetherFailureCondition` (instanced UObject on the scenario): timer expired, alarm count reached, protected object destroyed, player caught, and custom from Blueprint.
- **P2.5 Stats & results.** `UIdiotsTogetherStatsComponent` on PlayerState counts falls, drops, objects broken, alarms triggered, distance run, time idle and "last to finish." The Results screen picks 3–4 funny awards (for example "Most Destructive," "Dropped It 7 Times"). Award definitions are data-driven.

**Done when:** a test scenario is defined entirely in its data asset, replays cleanly 3 times with a fresh world each time, and shows awards.

---

## Phase 3: Interaction & Physics Carrying (highest technical risk)

- **P3.1 Interaction framework v2.** Add interaction types: instant, hold-to-interact (with progress, cancellable), toggle, and requires-item (for example a keycard). Add multi-player interactions: N players holding at once, or simultaneous presses within a time window. Interaction state is replicated.
- **P3.2 Single-player carry.** Pick up, carry, drop and throw small physics objects. The server owns the object. Use attach-while-held for small items, and physics-handle or constraint behaviour for medium items so they swing and bonk into things.
- **P3.3 Co-op carry.** Large objects (box, sofa, piano) need 2+ grab points and move according to the combined input of the carriers. Too few carriers means the object drags, tilts or drops. This is the centrepiece mechanic. Make the weight and required carriers data-driven.
- **P3.4 Network physics evaluation.** Compare plain replicated movement with interpolation against Chaos physics replication modes. Test with network emulation at 150 ms latency and 2% packet loss. Write findings to `docs/NETWORK_PHYSICS.md` and pick one approach. Set a budget for the maximum number of simulated physics actors per scenario.
- **P3.5 Character physical comedy hooks.** Stumble or knockdown when hit by heavy objects or when falling a long way, a short ragdoll with get-up, and replicated ragdoll state. Keep the ragdoll pose cosmetic on clients and keep the server capsule authoritative.

**Done when:** 4 clients at 150 ms can carry a sofa through a doorway together, it looks acceptable on every screen, and dropping it is funny rather than glitchy. *If this doesn't feel right, stop and iterate here. Everything later depends on it.*

---

## Phase 4: Gameplay Building Blocks

Goal: a level designer builds missions by placing actors and linking them, with no code.

- **P4.1 Objective types.** Base class `UIdiotsTogetherObjective` (instanced in the scenario) with these subclasses: Collect, Deliver-to-zone, Activate, Deactivate, Reach zone, All players in zone, Protect (must survive), Destroy, Sequence (ordered steps), and Keep-alive (for example a fuel level). Objectives can be hidden or revealed, optional, or bonus.
- **P4.2 Trigger system.** Configurable trigger volumes: filter by player, specific object or object tag, enter/exit/stay-for-N-seconds, and once or repeatable.
- **P4.3 Event or signal system.** A level-wiring system. A `SignalEmitter` component fires named signals (for example "SafeOpened"), and a `SignalReceiver` component maps signals to actions (activate alarm, lock door, complete objective, reveal objective, start timer, play sound). Make it fully configurable in the details panel. It must be editable by a designer and debuggable (in-editor visualisation of links, and a log in dev builds).
- **P4.4 Chaos components library.** Door (locked/unlocked, keycard), keypad (code from a randomized clue), safe (multi-step), lever/button/switch, breakable (health, fragile, shatter), alarm (cooldown, escalation), timed hazard (sprinklers, electric floor), moving machinery (conveyors, swinging arms), rising water volume, power-cut controller (lights off, some devices disabled), and noise emitter/listener (for the "Don't Get Caught" category).
- **P4.5 Simple guard AI.** One reusable NPC with a patrol path, a vision cone, a hearing response to the noise system, investigate → alert → "caught" failure, and a sleeping state. Keep it simple. It serves the comedy rather than stealth depth.
- **P4.6 Editor validation.** Add an `EditorValidator` that flags broken signal links, objectives with no completing actor, and missing spawn points when a scenario map is saved.

**Done when:** a human builds a small "steal the box, open the door, escape" test mission entirely in the editor without writing C++.

---

## Phase 5: Controlled Randomization

- **P5.1 Seed.** The server picks a seed before Briefing and replicates it. All randomness uses an `FRandomStream` from that seed so results can be reproduced, and a seed can be entered manually in dev builds.
- **P5.2 Random slots.** `RandomSlot` actors with tags: candidate locations for objects, alternative routes (doors open/closed), and hazard placements. The scenario's randomization profile states the rules (for example "target in 1 of these 4 slots" or "2 of 5 hazards active").
- **P5.3 Solvability validation.** Use a rule-based validator: required items must be reachable before the doors that need them, and the escape route must exist. Invalid seeds are re-rolled automatically. Add an automation test that generates 1,000 seeds per scenario and asserts they are all valid.
- **P5.4 Briefing information.** The briefing and planning UI shows the randomized layout (a top-down map with markers for target, equipment and hazards) so players can plan around it.

**Done when:** the test mission produces visibly different layouts that are all solvable, and the 1,000-seed test passes.

---

## Phase 6: Scenario #001 "The Con" (Vertical Slice)

- **P6.1 Design doc.** Claude Code drafts `docs/scenarios/001_the_con.md` from the brief: layout, objectives, randomization profile, chaos beats and target duration. A human approves it.
- **P6.2 Greybox build.** Mostly human editor work, assisted by Claude Code Python scripts and EDITOR STEPS. List any missing reusable mechanic, and build each one as a generic component.
- **P6.3 Tuning pass.** Phase durations, guard sensitivity and carry weights, all set in data.
- **P6.4 Playtest instrumentation.** Log every round to a local JSON file (seed, success or failure, failure reason, time, stats) so the playtests produce data.
- **GATE: playtest with at least 5 external friend groups.** Are they laughing? Do they ask to play again? Do failures feel funny rather than unfair? Fix the framework *before* making more content.

---

## Phase 7: Characters, Animation, Audio & Juice

Art is sourced by humans (artist or licensed packs). Claude Code builds the integration.

- **P7.1 Character data.** A `UIdiotsTogetherCharacterIdentity` data asset (mesh, materials, colour variants, voice-line set, idle quirks). Swap the placeholder static mesh for a skeletal mesh with an AnimInstance C++ base class (exposing locomotion, carrying, panic and falling state).
- **P7.2 Expressive animation hooks.** Carry poses (one-hand, two-hand, co-op), a panic run when the alarm is active or the timer is under 30 s, idle fidgets, and 4–6 emotes with a replicated emote wheel.
- **P7.3 Audio system.** Sound cues per event through a data table, music states per phase (calm planning, tense execution, panic final 30 s), and a voice-line or grunt system per identity.
- **P7.4 Feedback and juice.** Interaction highlights, objective popups, camera shake, hit-stop on big crashes, and a results-screen replay of the "best disaster" moment as an optional stretch goal.
- **P7.5 Low-poly rendering pass.** Material conventions, a lighting preset for stylized scenes, and scalability settings for the GTX 1650 target.

---

## Phase 8: Steam Online & Voice

**Human prerequisites:** a Steamworks partner account, an App ID, and the Steamworks SDK. Check current Steamworks fees and rules on partner.steamgames.com.

- **P8.1 Steam backend** behind the Phase 1 session interface. Use Unreal's Steam online subsystem or online services plugin, and keep the Null backend for development. Use App ID 480 for local testing only.
- **P8.2 Steam lobbies & invites.** Invite friends from the lobby, join through the Steam overlay or friends list, rich presence ("Planning a heist – 3/6"), and private (invite-only) lobbies by default.
- **P8.3 Voice.** Pick one: Steam voice through the subsystem, or EOS voice (free, and it works across platforms, which helps the PlayStation goal). Add push-to-talk and open-mic options, per-player mute, volume controls and a speaking indicator. Planning phase uses team-wide voice. **Proximity voice during execution is a stretch toggle**, gated behind a setting.
- **P8.4 Internet testing.** Test NAT traversal through Steam relay and real-world latency. Write a test matrix and run it with players in different cities.

---

## Phase 9: Scenario Production (Launch Set)

Recommended launch set of 7, each adding one distinct mechanic:

| # | Scenario | New mechanic introduced |
|---|---|---|
| 001 | The Con | Interaction, guards, randomization (baseline) |
| 014 | The Piano | Co-op carry through tight spaces |
| 021 | One Car | **Split vehicle controls** (steer / throttle / brake / horn per player). This needs a vehicle system task first: Chaos Vehicle plus input split by seat |
| 031 | Don't Wake Him | Noise system plus a sleeping NPC |
| 043 | Flooded Building | Rising water and timed hazards |
| 045 | Restaurant Rush | Station-based throughput tasks |
| 042 | Power Cut | Darkness plus power routing |

- **P9.0 Scenario template.** A template map plus a data asset preset, and a "New Scenario" editor Python script.
- **P9.x** One task series per scenario: design doc → missing mechanics → greybox → playtest → polish. Run a playtest after each scenario.
- Additional scenarios after launch should mostly reuse these mechanics (for example Museum Job and Jewelry Store reuse The Con's systems).

---

## Phase 10: Release Preparation

- **P10.1 Settings.** Graphics (scalability presets), audio, keybind remapping, gamepad support, mouse sensitivity, and accessibility (subtitles for voice lines, colourblind-safe player colours).
- **P10.2 Save system.** Settings, best ratings per scenario, and unlocked cosmetics.
- **P10.3 Steam features.** Achievements, stats and cloud saves.
- **P10.4 Performance.** Profile each scenario at 6 players on GTX 1650-class hardware (target 60 fps at 1080p medium). Measure network bandwidth per client.
- **P10.5 Shipping builds.** Shipping configuration, ForDistribution, packaging scripts (BuildCookRun), crash reporting, and a check that no dev cheats remain.
- **P10.6 SteamPipe.** Depot configuration, a `steamcmd` upload script, and beta branches for playtesters.
- **P10.7 Store and launch (human-led).** Capsule art, trailer and screenshots, the Coming Soon page live early (Steam requires a waiting period, so check the current rule), a Steam Playtest or demo to build wishlists, regional pricing for INR, and a decision on Early Access versus full release.

---

## Prompt template for Claude Code

Paste this, with the task filled in, at the start of each task:

```
Read CLAUDE.md and docs/ROADMAP.md. We are on task P<phase>.<task>: <task name>.

Task: <copy the task description from ROADMAP.md>

Context: <anything decided since, e.g. "We chose option (a) for duplicate characters">

Requirements:
- Server-authoritative; list every new replicated property and RPC.
- Expose tunables as UPROPERTYs; no magic numbers.
- Add or extend automation tests where logic is testable without a level.
- Compile and run tests before reporting done.

Before coding: summarise your plan and the files you'll touch, and wait for my OK.
When done: summary, files changed, test steps (PIE listen server + 3 clients),
EDITOR STEPS FOR HUMAN, known limitations.
```

Tips:
- Run Claude Code locally on the machine that has UE 5.8 installed, so it can compile and run tests itself.
- One task per session. Start a fresh session for each new task, and commit between tasks.
- For Phase 3 and any other "feel" work, expect several rounds: test in PIE, describe what felt wrong, and have it adjust.
