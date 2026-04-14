# Copyright (c) 2026 O.S. Systems Software LTDA.
# Copyright (c) 2026 Freedom Veiculos Eletricos
# SPDX-License-Identifier: Apache-2.0

import re

from twister_harness import DeviceAdapter


def test_lua_pre_conditions(dut: DeviceAdapter):
    """Validate that Lua pre-conditions control node execution.

    - increment logs should show increasing counter values
    - check_value tick logs should only appear for odd
      counter values (even values short-circuited by
      _successIf)
    - guarded_action should execute at least once
    """
    inc_re = re.compile(r"increment: counter=(\d+)")
    check_re = re.compile(r"check_value: ticked")
    guard_re = re.compile(r"guarded_action: executed")

    lines = dut.readlines_until(
        num_of_lines=60,
        timeout=30,
    )

    counters = []
    check_count = 0
    guard_count = 0

    for line in lines:
        m = inc_re.search(line)
        if m:
            counters.append(int(m.group(1)))
        if check_re.search(line):
            check_count += 1
        if guard_re.search(line):
            guard_count += 1

    # Counter should increment at least a few times
    assert len(counters) >= 3, f"Expected >=3 increments, got {len(counters)}"

    # Counter values should be monotonically increasing
    for i in range(1, len(counters)):
        assert counters[i] > counters[i - 1], f"Counter not increasing: {counters}"

    # check_value should NOT tick every time
    # (skipped when counter is even via _successIf)
    assert check_count < len(counters), (
        f"check_value ticked {check_count} times "
        f"but should be skipped on even counters"
    )

    # guarded_action should execute at least once
    assert guard_count >= 1, "guarded_action never executed"
