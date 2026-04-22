<!--
Copyright (c) 2026 O.S. Systems Software LTDA.
Copyright (c) 2026 Freedom Veiculos Eletricos
SPDX-License-Identifier: Apache-2.0
-->

# Zephyr Behaviour Tree - Robot Patrol

## Overview

Advanced sample demonstrating a ROS-like mobile robot that patrols
waypoints on an 8x8 grid world with a live terminal UI animation. The
robot navigates using BFS pathfinding, scans for targets at each
waypoint with animated phases, manages battery with gradual charging,
handles emergencies, and cycles through random missions.

This sample exercises all major ZephyrBT Lua features:

- **2 subtrees with port aliasing**: `HandleEmergency` (2 instances
  aliased to battery/obstacle metrics) and `NavigateTo` (3 instances
  aliased to patrol waypoint / home / emergency response), each with
  different port bindings
- **Lua conditions inside subtrees**: port references resolved per
  instance at code generation time
- **4 user Lua override files**: organized by concern (diagnostics,
  navigation, safety, mission)
- **Blackboard shared between C and Lua**: C nodes write sensor data,
  Lua predicates read it to gate BT branches
- **Clear separation of concerns**: BT XML orchestrates, Lua gates
  (`_skipIf` / `_failureIf` / `_while`), C computes (BFS, grid scan,
  battery drain, terminal rendering)
- **Real computation**: BFS pathfinding, battery drain model, area
  scanning with animated phases
- **Terminal UI**: ANSI escape codes for in-place grid animation

<div align="center">

https://github.com/user-attachments/assets/5f135e0a-d202-4a22-948a-bb1fcad58e5f

</div>

## Terminal UI

The display redraws in place each tick using ANSI escape codes:

```text
+---------------------------------------------+
| ZephyrBT Robot Patrol                       |
+---------------------------------------------+
|   0 1 2 3 4 5 6 7 Bat [========  ]  82      |
| 0 C . # . . * v .  Pos     (3,7)            |
| 1 . . # . . . . .  Target  W4 (5,7)         |
| 2 . . . . # . . .  Step    3/5              |
| 3 v * . . # . . .  Scanned 3/4              |
| 4 . . . . . . . .  Targets 3                |
| 5 . # # . . . * v  Tick    52               |
| 6 . . . . . . . .  Navigating...            |
| 7 H . . R * W . .                           |
| R=Robot C=Charger H=Home W=Waypoint         |
| v=Visited #=Obstacle *=Target               |
+---------------------------------------------+
```

Icons:

- `R` (green) = Robot
- `C` (yellow) = Charger
- `H` (cyan) = Home
- `W` (yellow) = Waypoint (unvisited)
- `v` (cyan) = Visited waypoint
- `#` (red) = Obstacle
- `*` (magenta) = Scan target
- `.` (dim) = Free cell

Scan animation at waypoints:

- `R` = Arrived at waypoint (1 tick)
- `?` (magenta) = Searching... (1 tick)
- `!` (green) = Found N targets! (1 tick)
- `!` = Heading home... (1 tick hold)

## Grid World

The initial grid is fixed (8x8). After each mission completes, a new
random grid is generated with randomized obstacles, waypoints, and
targets (seeded from `k_uptime_get()`):

```text
        0   1   2   3   4   5   6   7
      +---+---+---+---+---+---+---+---+
   0  | C |   | # |   |   |   | W2|   |
      +---+---+---+---+---+---+---+---+
   1  |   |   | # |   |   |   |   |   |
      +---+---+---+---+---+---+---+---+
   2  |   |   |   |   | # |   |   |   |
      +---+---+---+---+---+---+---+---+
   3  | W1|   |   |   | # |   |   |   |
      +---+---+---+---+---+---+---+---+
   4  |   |   |   |   |   |   |   |   |
      +---+---+---+---+---+---+---+---+
   5  |   | # | # |   |   |   |   | W3|
      +---+---+---+---+---+---+---+---+
   6  |   |   |   |   |   |   |   |   |
      +---+---+---+---+---+---+---+---+
   7  | R |   |   |   |   | W4|   |   |
      +---+---+---+---+---+---+---+---+
```

Fixed positions (preserved across missions):

- **C** = Charger at (0,0)
- **H** = Home at (0,7) — also robot start position
- Scan targets placed within 1 cell of each waypoint

## Behaviour Tree Architecture

Each tick runs a four-stage outer Sequence wrapped in a `Delay`
throttle. Decision-making uses a priority-based Fallback (selector)
pattern, common in ROS Nav2 navigation stacks:

```text
Delay(delay_msec)
+-- Sequence
    +-- Render           (C: commit previous tick's state)
    +-- Respawn          (C: regen world when Lua requests)
    +-- Sense            (C: obstacle_dist, tick, charging)
    +-- Fallback "decide"
        +-- HandleEmergency [battery < 20 -> charger]
        +-- HandleEmergency [obstacle < 1 -> safe pos]
        +-- Fallback "patrol_or_return"
        |   +-- Sequence (skipIf patrol_done)
        |   |   +-- SelectWaypoint    (C: next waypoint)
        |   |   +-- NavigateTo        (patrol instance)
        |   |   +-- PerformScan       (C: 4-phase animation)
        |   |   +-- NavigateTo        (home instance)
        |   +-- Sequence (skipIf at_home)
        |       +-- NavigateTo        (home instance, recovery)
        +-- AlwaysSuccess "idle"  (Lua: celebrate/new mission)
```

Orchestration lives entirely in the XML; the Lua `_skipIf` /
`_failureIf` / `_while` predicates gate transitions; the C nodes only
compute one physical step at a time.

**Subtree 1: HandleEmergency** (port-aliased, 2 instances)

```text
Sequence
+-- CheckMetric      (C: metric < threshold or charging?)
+-- NavigateTo       (emergency instance: drive to response pos)
+-- Charge           (C: +5/2 ticks, skipIf not at charger)
+-- AlwaysSuccess    (Lua _post: on_emergency_response)
```

Instance 1 monitors the battery and responds by navigating to the
charger, then charging. Instance 2 monitors the obstacle distance and
steps aside to a safe position; its `Charge` node short-circuits via
`_skipIf` because the response position is not the charger.

**Subtree 2: NavigateTo** (port-aliased, 3 instances)

```text
Sequence
+-- ComputePath   (C: BFS, skipIf nav_status >= 2)
+-- FollowPath    (C: one step per tick, while nav_status == 2)
```

Used for patrol (to `wp_x` / `wp_y`), return home (to `home_x` /
`home_y`), and emergency navigation (to `response_x` / `response_y`).
Each instance has its own `nav_status` so BFS results stay
instance-local; `pos_x` / `pos_y` and `battery` are shared (robot has
one body).

## Gameplay Loop

1. Robot starts at Home (0,7) with 100% battery
2. Patrols waypoints W1 -> W2 -> W3 -> W4:

   - Navigate to waypoint via BFS (1 step per tick)
   - Scan animation: arrive -> search -> found -> hold (4 ticks)
   - Navigate home via BFS

3. When battery < 20%:

   - `HandleEmergency` wins the outer `Fallback`
   - `NavigateTo` drives the robot to the charger step by step
   - `Charge` adds +5 per 2 ticks until 100%
   - `on_emergency_response` (Lua) resets transient state, patrol
     resumes from the charger

4. After all 4 waypoints scanned (`patrol_done(bb)` is true):

   - Scan branch `_skipIf`'s itself out of the Fallback
   - Home branch runs `NavigateTo` to home
   - At home: `at_home(bb)` is true, Fallback falls through to `idle`
   - `on_idle_tick` holds `Mission N complete!` for 5 ticks, then
     signals `Respawn` via `new_mission` for a fresh grid

## Battery Model

- Start: 100 units
- Movement: -1 per grid step (navigation AND emergency)
- Scanning: -5 per scan operation
- Critical threshold: 20 (triggers emergency)
- Charging: +5 per 2 ticks at charger, until 100

## Port Aliasing

Port aliasing allows the same subtree to operate on different
blackboard variables per instance. `HandleEmergency` is instantiated
twice with different ports; `NavigateTo` is instantiated three times
(patrol / home / emergency) with different targets and nav-status
slots:

```xml
<!-- Battery emergency: metric=battery, threshold=critical -->
<SubTree ID="HandleEmergency"
    metric="{battery}" threshold="{critical_battery}"
    response_x="{charger_x}" response_y="{charger_y}"
    pos_x="{pos_x}" pos_y="{pos_y}"
    battery="{battery}" charging="{charging}"
    nav_status="{nav_emergency}"/>

<!-- Obstacle emergency: metric=obstacle_dist, threshold=clearance -->
<SubTree ID="HandleEmergency"
    metric="{obstacle_dist}" threshold="{min_clearance}"
    response_x="{safe_x}" response_y="{safe_y}"
    pos_x="{pos_x}" pos_y="{pos_y}"
    battery="{battery}" charging="{charging}"
    nav_status="{nav_emergency}"/>

<!-- Patrol navigation instance -->
<SubTree ID="NavigateTo"
    target_x="{wp_x}" target_y="{wp_y}"
    pos_x="{pos_x}" pos_y="{pos_y}"
    battery="{battery}" nav_status="{nav_patrol}"/>

<!-- Home navigation instance -->
<SubTree ID="NavigateTo"
    target_x="{home_x}" target_y="{home_y}"
    pos_x="{pos_x}" pos_y="{pos_y}"
    battery="{battery}" nav_status="{nav_home}"/>
```

The code generator resolves port references per instance at build
time, so each instance operates on its own blackboard slots.

## C Node Implementations

| Node | Computation |
|------|-------------|
| **Sense** | One-time world + blackboard init (grid, RNG seed, tree-level constants, `delay_msec`). Per tick: compute obstacle distance, advance tick counter, mirror charging flag. |
| **Respawn** | When `new_mission` is set by Lua, regenerates obstacles, waypoints, and targets for the next mission. |
| **Render** | Commits the terminal UI for the state captured at the end of the previous tick. |
| **SelectWaypoint** | Picks the next waypoint from the list and advances the index. Gated by `_skipIf="patrol_done(bb)"` in the XML; no C-side home retargeting. |
| **ComputePath** | BFS flood-fill on 8x8 grid. Stores path per-instance using `nav_status` pointer as key. Skipped by Lua when already computed (`_skipIf nav_status >= 2`). |
| **FollowPath** | Pops one step per tick, updates position, drains battery via the `battery` port (no global-state reach-through). Guarded by Lua `_while nav_status == 2`. |
| **PerformScan** | 4-phase animated scan: ARRIVED (R), SEARCH (?), FOUND (!), HOLD (!). Scans 3x3 area, costs 5 battery. |
| **CheckMetric** | Compares metric vs threshold (also triggers while charging), announces `"Emergency: moving"` on SUCCESS. |
| **Charge** | Gradual charge (+5 per 2 ticks) until full. Owns the charging flag. Gated by `_skipIf` for non-charger response positions (obstacle emergency no-ops through). |

## Lua Files

| File | Purpose |
|------|---------|
| `lua/diagnostics.lua` | Shared `manhattan()` helper. `on_idle_tick()` handles the post-mission celebration countdown and signals `new_mission` to C after 5 idle ticks. `on_emergency_response()` resets transient nav / charge blackboard state when the emergency sequence finishes. |
| `lua/navigation.lua` | `waypoint_already_selected()` skips selection when navigating (returns false at home/charger to trigger reselection). `on_waypoint_selected()` resets nav states. |
| `lua/safety.lua` | `scan_too_risky()` prevents scan when battery below critical threshold or charging. |
| `lua/mission.lua` | `patrol_done(bb)`, `at_home(bb)`, and `mission_complete(bb)` are the branch gates used by `_skipIf` / `_failureIf` in the patrol sequence. `on_scan_complete()` tracks progress. |

## Prerequisites

This sample requires the `lua_zephyr` module:

```console
west update
cd deps/modules/lib/lua_zephyr
git submodule update --init
cd -
```

## Building and Running

This application can be built and executed on `native_sim/native/64`
as follows:

```console
west build -p -b native_sim/native/64 samples/subsys/zephyrbt/robot_patrol -t run
```

> **Note:** This sample requires `native_sim/native/64` for the
> terminal UI (ANSI escape codes via printf). The `qemu_cortex_m3`
> board is not supported (insufficient RAM for Lua).

## File Structure

```text
robot_patrol/
+-- CMakeLists.txt           Build configuration
+-- prj.conf                 Zephyr/BT/Lua config
+-- sample.yaml              Twister test harness
+-- README.md                This file
+-- models/
|   +-- robot_patrol.xml     Behaviour tree definition
+-- include/
|   +-- zephyrbt_user.h      Required header
|   +-- robot_state.h        Shared state, BFS, display API
+-- src/
|   +-- main.c               Entry point
|   +-- world.c              Sense/Respawn/Render, grid, mission regen
|   +-- navigation.c         ComputePath, FollowPath, BFS
|   +-- charge.c             Charge (gradual recharge)
|   +-- waypoint.c           SelectWaypoint (patrol only)
|   +-- scan.c               PerformScan: 4-phase animation
|   +-- emergency.c          CheckMetric: emergency trigger
|   +-- display.c            Terminal UI rendering
|   +-- lua_scripts.c        User Lua file wiring
|   +-- posix_stubs.c        POSIX stubs for bare-metal
+-- lua/
    +-- diagnostics.lua      Shared helpers, tick, idle
    +-- navigation.lua       Waypoint selection logic
    +-- safety.lua           Battery threshold check
    +-- mission.lua          Scan tracking, mission complete
```
