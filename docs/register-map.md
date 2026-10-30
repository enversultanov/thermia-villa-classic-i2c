# Register map – Thermia Villa Classic, controller board 901 353 (software 2.11c)

Worked out on 2026-10-29 by comparing the register dump with the pump's display and an external outdoor sensor.
**Valid for one pump. May be wrong for yours.** The ThermIQ register map (newer boards) does NOT apply here.

The pump sends frames `[reg, value]` (8 bit) on its own, registers 0x00–0x3C, ~4 frames/s. No request needed.

## Confirmed / probable

The display text (Swedish) is given so you can find the value in the pump's menus.

| Reg | Meaning | Display text | Encoding | Confidence |
|---|---|---|---|---|
| 0x00 | Room setpoint (°C) | RUM | raw | **Confirmed** (changed 19→20→19 on the display) |
| 0x01 | Operating mode | Driftläge | 1 Auto, 2 Hot water only (Bara VV), 3 Heat pump only (Bara VP), 4 Aux heater only (Bara TS); 0 = off (assumed) | **Confirmed** (all four modes switched on the display) |
| 0x03 | Heating curve | Kurva | raw | Probable (= display 33) |
| 0x06 | Curve min (°C) | Kurva min | raw | Probable (= display 10) |
| 0x07 | Curve max (°C) | Kurva max | raw | Probable (= display 55) |
| 0x0B | Max return (°C) | Returl (48) | raw | Probable (= value in brackets) |
| 0x11 | Integral (°min)? | Integral | int8 | Doubtful (= display +7 once, but constant all night) |
| 0x28 | Supply line (°C) | Framl | raw | Probable (1–2 °C below the display) |
| 0x29 | Return line (°C) | Returl | raw | Probable (1–2 °C below the display) |
| 0x2B | Brine out, to borehole (°C)? | Kb till | **value − 40** | Doubtful (= display 9 once, but in − out ≈ 0 while the compressor runs) |
| 0x2C | Probably brine out or evaporator (°C) | – | value − 40 | Probable (drops to ~4–5 °C every time the compressor runs) |
| 0x2D | Brine in, from borehole (°C) | Kb fr. | **value − 40** | Probable (= display 10) |
| 0x30 | Outdoor temperature (°C) | Ute | **value − 40** | **Confirmed** (14 and 10 °C, follows an external sensor) |
| 0x20 | Indoor/room sensor? (°C) | – | value − 40 | Guess (16–20 °C, slowly varying) |
| 0x31 | Room setpoint, copy (°C) | – | raw | Probable (follows 0x00 when the setpoint is changed) |
| 0x32 | Supply line setpoint (°C) | Framl (21) | raw | **Confirmed** (25 → 40 → 25 when the room setpoint was changed 19 → 24 → 19) |
| 0x33 | Status bits | – | bit 5 (0x20) = circulation pump (**confirmed**: off in the non-auto modes, on with its LED), bit 1 (0x02) = **compressor** (12 starts overnight, the house draws +1.5 kW while set), bit 4 (0x10) = **hot water production** (7 runs, supply line 54–55 °C each time), bit 5 (0x20) = circulation pump (off in the non-auto modes) | **Confirmed** |
| 0x38 | Clock, seconds | – | raw | Confirmed |
| 0x39 | Clock, minutes | – | raw | Confirmed |
| 0x3A, 0x3C | Counters (+1 per minute) | – | | Meaning unknown |

## Constant / unknown (one pump, one day)

`02:16 04:4 05:1 08:25 09:60 0A:60 0C:44 0D:20 0E:20 0F:17 10:20 12:15 13:55 14:50 15:60 16:14 17:1 18:60
19–1D:5 1E:8 1F:5 21:97 22:255 23:8 24:27 25:41 26:0 27:1 2A:0 2E:20 2F:0 34:16–20 (changes with the operating mode) 35:0 36:129 37:0/64 3B:0`

Note: r16 = 14 happened to equal the outdoor temperature once – it is NOT the outdoor temperature (it never
changes). r0C = 44 is not the current in amperes.

## Missing

Auxiliary heater, alarm, hot water temperature (the Villa Classic controls hot water with
a pressostat), run-time counters. Compressor electrical power measured at ≈ 1.5 kW (Villa Classic 55, outdoor 5–13 °C).

## Raw data

[example-raw-log-2026-10-29.txt](example-raw-log-2026-10-29.txt) – ESPHome log of every received frame
(4 minutes; "ram 2 byte" = "frame 2 bytes", from an earlier Swedish version of the component).
