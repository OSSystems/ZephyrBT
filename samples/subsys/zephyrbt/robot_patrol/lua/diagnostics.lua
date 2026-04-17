-- Copyright (c) 2026 O.S. Systems Software LTDA.
-- Copyright (c) 2026 Freedom Veiculos Eletricos
-- SPDX-License-Identifier: Apache-2.0

-- diagnostics.lua: Shared helpers, counters, idle tracking.
-- Loaded first since other Lua files depend on manhattan().

-- Manhattan distance between two points.
function manhattan(x1, y1, x2, y2)
    local dx = x1 - x2
    local dy = y1 - y2
    if dx < 0 then dx = -dx end
    if dy < 0 then dy = -dy end
    return dx + dy
end

-- Called when the robot is idle.
-- Handles mission complete countdown and new mission.
function on_idle_tick(bb)
    bb.idle_count = bb.idle_count + 1

    if bb.scans_completed >= 4 and
       bb.pos_x == bb.home_x and
       bb.pos_y == bb.home_y then
        if bb.idle_count <= 5 then
            bb.mission_progress = 100
        else
            -- Signal C code to generate new mission.
            -- Nav-status resets are owned by
            -- on_waypoint_selected (navigation.lua).
            bb.new_mission = 1
            bb.scans_completed = 0
            bb.mission_progress = 0
            bb.wp_index = 0
            bb.wp_x = -1
            bb.wp_y = -1
            bb.idle_count = 0
            bb.scan_errors = 0
        end
    end
end

-- Called at the end of the HandleEmergency sequence.
-- Reset nav/charge state so patrol can resume fresh.
function on_emergency_response(bb)
    bb.idle_count = 0
    bb.charging = 0
    bb.nav_patrol = 0
    bb.nav_home = 0
    bb.nav_emergency = 0
    bb.wp_x = -1
    bb.wp_y = -1
end
