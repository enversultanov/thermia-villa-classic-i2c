# Thermia Villa Classic – I2C service port to Home Assistant (ESPHome)

Read an older **Thermia Villa Classic** ground-source heat pump (controller board **901 353**) in Home Assistant
through the pump's 4-pin I2C service port ("INTERFACE"), using an ESP8266/ESP32 and ESPHome. No cloud, no
commercial logger needed.

> ## ⚠️ Disclaimer – read this first
>
> **Everything you do with this project, you do entirely at your own risk.**
>
> - This is a hobby project. It is **not** approved, reviewed or supported by Thermia, Danfoss or any other manufacturer.
> - You are tampering with the electronics of a heat pump. Wrong wiring can **destroy the controller board, the pump
>   or your ESP**, stop your heating in the middle of winter, cause freezing or water damage, and may affect your
>   **warranty and insurance**.
> - Working inside the pump involves a risk of **electric shock (230/400 V mains)**. Switch off the power before
>   opening the cover, and hire a licensed electrician if you are the least bit unsure.
> - The register map below was worked out by comparing data with the display of **one** pump. It may be wrong or
>   incomplete for your pump or software version.
> - The code is provided **"as is", without any warranty** (see [LICENSE](LICENSE)). The authors are not liable
>   for any damage, direct or indirect, resulting from the use of the code, the documentation or the advice here.
>
> If you do not accept this – do not use the project.

## Status

| Part | Status |
|---|---|
| Receiving data from the pump (listen only) | ✅ Works, ~4 frames/s, 61 registers (r00–r3C) |
| Register map (temperatures, settings) | 🟡 Partly confirmed – see [docs/register-map.md](docs/register-map.md) |
| Status LEDs (compressor, hot water …) | 🟡 Partly (some calculated/probable) |
| Writing settings (e.g. room setpoint) | ❌ Not implemented – the component never sends anything to the pump |

Tested on: Thermia Villa Classic 55, controller board 901 353 E (software 2.11c), NodeMCU V3.4 (ESP8266), ESPHome 2026.10.

## How it works

The pump's controller is the **I2C master** and sends 2-byte frames `[register, value]` on its own to a slave at
7-bit address **0x2E**. The ESP acts as that slave (Arduino `Wire` in slave mode) and stores the values; ESPHome
template sensors read them out. Temperatures from sensors are sent as **°C + 40** (so that −40 °C fits in a byte).

## Hardware

- ESP8266 (e.g. NodeMCU) or **preferably an ESP32** (better I2C slave support).
- A **level shifter** BSS138 (4 channels, e.g. Adafruit 757) or, safer, an **I2C isolator** (e.g. Adafruit ISO1540).
  The pump's bus is **5 V** – the ESP's pins are officially only 3.3 V tolerant.
- A **separate power supply for the ESP** (USB charger).

### The service port (measured on a 901 353 E)

4 pins, from the top: **GND, SCL, SDA, +5V**. Measure yourself first! Against GND: +5V ≈ 5.0 V, SDA ≈ 5.0 V
when idle, SCL 4.6–4.9 V on a multimeter because it is clocking.

### Wiring with a BSS138 level shifter

| Level shifter | Connect to |
|---|---|
| HV | pump +5V (reference only) |
| LV | ESP 3V3 |
| GND | pump GND **and** ESP GND |
| HV1 / LV1 | pump SCL / ESP D1 (GPIO5) |
| HV2 / LV2 | pump SDA / ESP D2 (GPIO4) |

> **NEVER power the ESP from the pump's +5V pin.** We tried – the pump's front panel went dark immediately. The
> port only supplies a few mA (enough as a reference for a level shifter/isolator), not an ESP with WiFi
> (300–400 mA peaks).

## Installation

1. Copy `esphome/` into your ESPHome configuration (or run it directly with Docker, see the comment at the top of
   `esphome/thermia.yaml`).
2. `cp esphome/secrets.yaml.example esphome/secrets.yaml` and fill in WiFi etc.
3. Flash the first time over USB, then OTA.
4. Add the device in Home Assistant (the ESPHome integration discovers it).
5. Compare the **Register dump** sensors with the pump's display before trusting any value.

Tip: the ESP8266 only allows a few API connections. Don't leave old `esphome logs` processes/containers running –
they take the slots and Home Assistant can no longer connect (`EncryptionHelloAPIError`).

## Contributing

Do you have a Villa Classic (or a relative with the same controller board)? Compare the register dump with your
display and open an issue or pull request with what you find – especially the compressor, auxiliary heater and
alarm bits, which are still missing.

## Sources and thanks

- Värmepumpsforum (Swedish heat pump forum): threads on the Villa Classic 55 and the service port pinout.
- [omakotikotitalomme.blogspot.com – Danfoss lämpöpumpun salaisuudet](https://omakotikotitalomme.blogspot.com/2015/03/danfoss-lampopumpun-salaisuudet.html) (protocol of the Danfoss/Thermia family).
- [rainisto/arduino_i2c_orja](https://github.com/rainisto/arduino_i2c_orja) (Arduino slave, GPL-3.0 – no code copied from it).
- [ThermIQ/thermiq_mqtt-ha](https://github.com/ThermIQ/thermiq_mqtt-ha) (register map for newer controller boards – does **not** apply to the Villa Classic 901 353).

Thermia is a trademark of its owner; this project is not affiliated with Thermia.

---

## Svenska (kort)

ESPHome-komponent som läser serviceporten (I2C, slav 0x2E) på äldre **Thermia Villa Classic** (reglerkort 901 353)
och visar värdena i Home Assistant. Bara läsning. **Allt sker helt på egen risk** – fel inkoppling kan förstöra
reglerkortet eller pumpen, och arbete inne i pumpen innebär risk för elektrisk stöt. Mata aldrig ESP:n från pumpens
+5V. Registerkartan finns i [docs/register-map.md](docs/register-map.md) (displayens svenska menynamn står med).
