-- Copyright (c) 2026 O.S. Systems Software LTDA.
-- Copyright (c) 2026 Freedom Veiculos Eletricos
-- SPDX-License-Identifier: Apache-2.0

-- mission.lua: Scan management and mission progress tracking.

-- All four waypoints have been scanned.
function patrol_done(bb)
    return bb.scans_completed >= 4
end

-- Robot is currently sitting at the home cell.
function at_home(bb)
    return bb.pos_x == bb.home_x and
           bb.pos_y == bb.home_y
end

-- Mission complete when all scanned AND at home.
function mission_complete(bb)
    return patrol_done(bb) and at_home(bb)
end

-- After a failed scan, track the error.
-- (Success path is handled in C's SCAN_FOUND phase so the
-- counter updates atomically with the real scan work.)
function on_scan_failed(bb)
    bb.scan_errors = bb.scan_errors + 1
end
