# Copyright (c) 2026 O.S. Systems Software LTDA.
# Copyright (c) 2026 Freedom Veiculos Eletricos
# SPDX-License-Identifier: Apache-2.0

import re

from twister_harness import DeviceAdapter


def test_port_alias_values_increase(dut: DeviceAdapter):
    write_re = re.compile(r"write_value \[(\d+)\]: wrote (\d+)")
    read_re = re.compile(r"read_value \[(\d+)\]: read (\d+)")

    last_write = {}
    last_read = {}

    lines = dut.readlines_until(
        num_of_lines=80,
        timeout=30,
    )

    write_count = 0
    read_count = 0

    for line in lines:
        wm = write_re.search(line)
        if wm:
            idx = int(wm.group(1))
            val = int(wm.group(2))
            if idx in last_write:
                assert val > last_write[idx], (
                    f"write_value [{idx}]: {val} did not"
                    f" increase from {last_write[idx]}"
                )
            last_write[idx] = val
            write_count += 1

        rm = read_re.search(line)
        if rm:
            idx = int(rm.group(1))
            val = int(rm.group(2))
            if idx in last_read:
                assert val > last_read[idx], (
                    f"read_value [{idx}]: {val} did not"
                    f" increase from {last_read[idx]}"
                )
            last_read[idx] = val
            read_count += 1

    assert write_count >= 4, f"Expected >=4 write lines, got {write_count}"
    assert read_count >= 4, f"Expected >=4 read lines, got {read_count}"
    assert len(last_write) == 2, f"Expected 2 write nodes, got {len(last_write)}"
    assert len(last_read) == 2, f"Expected 2 read nodes, got {len(last_read)}"
