# Register map – Thermia Villa Classic, controller board 901 353 (software 2.11c)

Worked out on 2026-10-29 by comparing the register dump with the pump's display and an external outdoor sensor.
**Valid for one pump. May be wrong for yours.** The ThermIQ register map (newer boards) does NOT apply here.

The pump sends frames `[reg, value]` (8 bit) on its own, registers 0x00–0x3C, ~4 frames/s. No request needed.

## Confirmed / probable

The display text (Swedish) is given so you can find the value in the pump's menus.

| Reg | Meaning | Display text | Encoding | Confidence |
|---|---|---|---|---|
| 0x00 | Room setpoint (°C) | RUM | raw | **Confirmed** (changed 19→20→19 on the display) |
| 0x03 | Heating curve | Kurva | raw | Probable (= display 33) |
| 0x06 | Curve min (°C) | Kurva min | raw | Probable (= display 10) |
| 0x07 | Curve max (°C) | Kurva max | raw | Probable (= display 55) |
| 0x0B | Max return (°C) | Returl (48) | raw | Probable (= value in brackets) |
| 0x11 | Integral (°min) | Integral | int8 | Probable (= display +7) |
| 0x28 | Supply line (°C) | Framl | raw | Probable (1–2 °C below the display) |
| 0x29 | Return line (°C) | Returl | raw | Probable (1–2 °C below the display) |
| 0x2B | Brine out, to borehole (°C) | Kb till | **value − 40** | Probable (= display 9) |
| 0x2C | Unknown sensor (°C) | – | value − 40 | Unknown (~13 °C, drops to ~5 when the compressor runs) |
| 0x2D | Brine in, from borehole (°C) | Kb fr. | **value − 40** | Probable (= display 10) |
| 0x30 | Outdoor temperature (°C) | Ute | **value − 40** | **Confirmed** (14 and 10 °C, follows an external sensor) |
| 0x32 | Supply line setpoint (°C) | Framl (21) | raw | Probable (= value in brackets, rises as it gets colder outside) |
| 0x33 | Status bits | – | bit 5 (0x20) = circulation pump?, bit 4 (0x10) = hot water production?, bit 1 (0x02) = ? | Probable/unknown |
| 0x38 | Clock, seconds | – | raw | Confirmed |
| 0x39 | Clock, minutes | – | raw | Confirmed |
| 0x3A, 0x3C | Counters (+1 per minute) | – | | Meaning unknown |

## Constant / unknown (one pump, one day)

`01:1 02:16 04:4 05:1 08:25 09:60 0A:60 0C:44 0D:20 0E:20 0F:17 10:20 12:15 13:55 14:50 15:60 16:14 17:1 18:60
19–1D:5 1E:8 1F:5 20:56 21:97 22:255 23:8 24:27 25:41 26:0 27:1 2A:0 2E:20 2F:0 31:19 34:16–20 35:0 36:129 37:0/64 3B:0`

Note: r16 = 14 happened to equal the outdoor temperature once – it is NOT the outdoor temperature (it never
changes). r0C = 44 is not the current in amperes.

## Missing

Compressor (a direct bit), auxiliary heater, alarm, hot water temperature (the Villa Classic controls hot water with
a pressostat), run-time counters. The ESPHome example calculates "Heat pump (compressor)" as brine in − out ≥ 2 °C.

## Raw data

[example-raw-log-2026-10-29.txt](example-raw-log-2026-10-29.txt) – ESPHome log of every received frame
(4 minutes; "ram 2 byte" = "frame 2 bytes", from an earlier Swedish version of the component).
