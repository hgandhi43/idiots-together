# Idiots Together: Game Design Brief

> **Status:** reconstructed in P0.1 from `CLAUDE.md` and `docs/ROADMAP.md`, because the original brief was not in the repo.
> If a fuller original brief exists, replace or merge it here. This file is the source of truth for design-level decisions.

## One line
An online co-op chaos party game for 2–6 friends: plan the perfect mission together, then watch it fall apart.

**Core philosophy: Plan perfectly. Execute terribly.**

## The round loop
| Phase | Length (default, set per scenario) | What happens |
|---|---|---|
| Briefing | short | Mission goal, map and the randomized layout are shown. Input is locked. |
| Planning | ~5 min | Players talk (voice is open), look over the map and assign roles. Objects can't be touched yet. |
| Execution | ~3 min | Everything is live. Physics, time pressure and human error take over. |
| Success / Failure | — | Early success as soon as all required objectives are complete. Failure when the timer runs out or a failure condition fires. |
| Results | — | Outcome, stats and funny awards ("Most Destructive", "Dropped It 7 Times"). |

A run is a configurable number of rounds. Every attempt starts from a fresh world state.

## Players and characters
- 2–6 players, online co-op over a listen server.
- Four identities: **Boss, Nerd, Muscle, Intern**. They are **cosmetic only**, and every identity has identical abilities.
- Duplicates are allowed. Each player gets a unique colour, so 5–6 players work (decision (a), 2026-11-03).

## Where the comedy comes from
- **Physics:** carrying, dropping and co-op lifting of awkward objects (sofas, pianos, boxes).
- **Time pressure:** a short execution window, with panic in the final 30 s.
- **Human mistakes:** the plan is good but the people are not.
- **Controlled randomization:** layouts change every round (seeded), but are always solvable.
- Failures must feel *funny*, never *unfair*.

## Scenarios
Scenarios are **data, not code**. They're built from reusable systems: objectives, triggers, signals, chaos components, a simple guard AI and random slots.

Planned launch set (each adds one distinct mechanic):

| # | Scenario | Mechanic |
|---|---|---|
| 001 | The Con | Interaction, guards, randomization (baseline) |
| 014 | The Piano | Co-op carry through tight spaces |
| 021 | One Car | Split vehicle controls (one player steers, one brakes...) |
| 031 | Don't Wake Him | Noise system plus a sleeping NPC |
| 042 | Power Cut | Darkness plus power routing |
| 043 | Flooded Building | Rising water and timed hazards |
| 045 | Restaurant Rush | Station-based throughput tasks |

There are about 50 scenario ideas in total. Later ones should mostly reuse mechanics from the launch set.

## What the game is NOT
Not open world, RPG, shooter, survival/crafting, class-based, story-heavy, or a job simulator.

**Feature test:** *Does this make the multiplayer experience more entertaining without unnecessarily increasing development complexity?*

## Platform and tech constraints
- Unreal Engine 5.8, C++ first, server-authoritative listen server.
- Steam (PC) first. A PlayStation port must stay possible, so platform SDK calls only appear in `Online/`.
- Target hardware: GTX 1650 4 GB / 16 GB RAM. Low-poly stylized art, no Lumen, no Nanite.
