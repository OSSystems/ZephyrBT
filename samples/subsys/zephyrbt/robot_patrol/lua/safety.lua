-- Copyright (c) 2026 O.S. Systems Software LTDA.
-- Copyright (c) 2026 Freedom Veiculos Eletricos
-- SPDX-License-Identifier: Apache-2.0

-- safety.lua: Battery and obstacle threshold logic.
-- Depends on manhattan() from diagnostics.lua.

-- Check if performing a scan is too risky given
-- current battery and distance to charger.
-- Cost estimate: return trip + scan + safety margin.
function scan_too_risky(bb)
    if bb.charging > 0 then return true end
    return bb.battery < bb.critical_battery
end
