# Registerkarta – Thermia Villa Classic, reglerkort 901 353 (programversion 2.11c)

Framtagen 2026-10-29 genom att jämföra registerdumpen med pumpens display och en extern utegivare.
**Gäller en pump. Kan vara fel för din.** ThermIQ:s registerkarta (nyare kort) gäller INTE här.

Pumpen skickar själv ramar `[reg, värde]` (8 bit), register 0x00–0x3C, ~4 ramar/s. Ingen förfrågan behövs.

## Bekräftade / troliga

| Reg | Betydelse | Kodning | Säkerhet |
|---|---|---|---|
| 0x00 | Rum börvärde (°C) | rått | **Bekräftat** (ändrat 19→20→19 på displayen) |
| 0x03 | Kurva | rått | Troligt (= display 33) |
| 0x06 | Kurva min (°C) | rått | Troligt (= display 10) |
| 0x07 | Kurva max (°C) | rått | Troligt (= display 55) |
| 0x0B | Max retur (°C) | rått | Troligt (= display "Returl (48)") |
| 0x11 | Integral (°min) | int8 | Troligt (= display +7) |
| 0x28 | Framledning (°C) | rått | Troligt (1–2 °C under displayen) |
| 0x29 | Returledning (°C) | rått | Troligt (1–2 °C under displayen) |
| 0x2B | Köldbärare till borrhål (°C) | **värde − 40** | Troligt (= display "Kb till 9") |
| 0x2C | Okänd givare (°C) | värde − 40 | Okänd (~13 °C, sjunker till ~5 när kompressorn går) |
| 0x2D | Köldbärare från borrhål (°C) | **värde − 40** | Troligt (= display "Kb fr. 10") |
| 0x30 | Utetemperatur (°C) | **värde − 40** | **Bekräftat** (14 och 10 °C, följer extern givare) |
| 0x32 | Framledning börvärde (°C) | rått | Troligt (= display "Framl (21)", stiger när ute sjunker) |
| 0x33 | Statusbitar | bit 5 (0x20) = värmebärarpump?, bit 4 (0x10) = varmvattenproduktion?, bit 1 (0x02) = ? | Troligt/okänt |
| 0x38 | Klocka, sekunder | rått | Bekräftat |
| 0x39 | Klocka, minuter | rått | Bekräftat |
| 0x3A, 0x3C | Räknare (+1 per minut) | | Okänd betydelse |

## Konstanta / okända (en pump, ett dygn)

`01:1 02:16 04:4 05:1 08:25 09:60 0A:60 0C:44 0D:20 0E:20 0F:17 10:20 12:15 13:55 14:50 15:60 16:14 17:1 18:60
19–1D:5 1E:8 1F:5 20:56 21:97 22:255 23:8 24:27 25:41 26:0 27:1 2A:0 2E:20 2F:0 31:19 34:16–20 35:0 36:129 37:0/64 3B:0`

OBS: r16 = 14 råkade vara lika med utetemperaturen en gång – det är INTE utetemperaturen (står still). r0C = 44 är
inte ström i ampere.

## Saknas

Kompressor (direkt bit), tillsatsvärme/elpatron, larm, varmvattentemperatur (Villa Classic styr varmvatten med
pressostat), drifttider. I ESPHome-exemplet beräknas "Värmepump" (kompressor) som köldbärare fr − till ≥ 2 °C.

## Rådata

[exempel-ralogg-2026-10-29.txt](exempel-ralogg-2026-10-29.txt) – ESPHome-logg med varje mottagen ram (4 minuter).
