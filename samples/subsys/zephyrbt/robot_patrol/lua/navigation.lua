-- Copyright (c) 2026 O.S. Systems Software LTDA.
-- Copyright (c) 2026 Freedom Veiculos Eletricos
-- SPDX-License-Identifier: Apache-2.0

-- navigation.lua: Waypoint selection conditions.

-- Skip waypoint selection if we already have a valid
-- target and are not back at a base position.
-- Returns false (don't skip) when:
--   1. No waypoint selected (wp_x < 0)
--   2. Robot is at home (completed patrol cycle)
--   3. Robot is at charger (after emergency recharge)
function waypoint_already_selected(bb)
    if bb.wp_x < 0 then
        return false
    end
    if bb.pos_x == bb.home_x and
       bb.pos_y == bb.home_y then
        return false
    end
    if bb.pos_x == bb.charger_x and
       bb.pos_y == bb.charger_y then
        return false
    end
    return true
end

-- After selecting a new waypoint, track selection count
-- and reset nav statuses so ComputePath runs fresh for
-- every instance (patrol, home, emergency).
function on_waypoint_selected(bb)
    bb.wp_selected_count = bb.wp_selected_count + 1
    bb.nav_patrol = 0
    bb.nav_home = 0
    bb.nav_emergency = 0
end
