# Thermia Villa Classic – I2C-serviceporten till Home Assistant (ESPHome)

Läs av en äldre **Thermia Villa Classic** bergvärmepump (reglerkort **901 353**) i Home Assistant via pumpens
4-poliga I2C-serviceport ("INTERFACE") med en ESP8266/ESP32 och ESPHome. Ingen molntjänst, ingen extra logger att köpa.

*English summary at the bottom.*

> ## ⚠️ Ansvarsfriskrivning – läs först
>
> **Allt du gör med det här projektet gör du helt på egen risk.**
>
> - Det här är ett hobbyprojekt. Det är **inte** godkänt, granskat eller supporterat av Thermia, Danfoss eller någon annan tillverkare.
> - Du ingriper i elektroniken i en värmepump. Fel inkoppling kan **förstöra reglerkortet, pumpen eller din ESP**, ge
>   driftstopp mitt i vintern, frysskador eller vattenskador, och kan påverka **garanti och försäkring**.
> - Arbete inne i pumpen innebär risk för **elektrisk stöt (230/400 V)**. Bryt strömmen innan du öppnar luckan och
>   anlita behörig elektriker om du är det minsta osäker.
> - Registerkartan nedan är framtagen genom att jämföra data med displayen på **en** pump. Den kan vara fel eller
>   ofullständig för din pump/programversion.
> - Koden levereras **i befintligt skick, utan några garantier** (se [LICENSE](LICENSE)). Författarna ansvarar inte för
>   några skador, direkta eller indirekta, som uppstår genom användning av koden, dokumentationen eller råden här.
>
> Om du inte accepterar detta – använd inte projektet.

## Status

| Del | Status |
|---|---|
| Ta emot data från pumpen (bara lyssna) | ✅ Fungerar, ~4 ramar/s, 61 register (r00–r3C) |
| Registerkarta (temperaturer, inställningar) | 🟡 Delvis bekräftad – se [docs/registerkarta.md](docs/registerkarta.md) |
| Driftlampor (kompressor, varmvatten …) | 🟡 Delvis (vissa beräknade/troliga) |
| Skriva inställningar (t.ex. rumsbörvärde) | ❌ Inte implementerat – komponenten skickar aldrig något till pumpen |

Testat på: Thermia Villa Classic 55, reglerkort 901 353 E (programversion 2.11c), NodeMCU V3.4 (ESP8266), ESPHome 2026.10.

## Hur det fungerar

Pumpens reglerdator är **I2C-master** och skickar själv 2-byte-ramar `[register, värde]` till en slav på
7-bitarsadress **0x2E**. ESP:n agerar slav (Arduino `Wire` i slavläge) och lagrar värdena; ESPHome-sensorer läser
ut dem. Temperaturer från givare skickas som **°C + 40** (så att −40 °C ryms i en byte).

## Hårdvara

- ESP8266 (t.ex. NodeMCU) eller **helst ESP32** (bättre I2C-slavstöd).
- **Nivåomvandlare** BSS138 (4 kanaler, t.ex. Adafruit 757 / Electrokit 41011576) eller, säkrare, en
  **I2C-isolator** (t.ex. Adafruit ISO1540). Pumpens buss är **5 V** – ESP:ns stift tål officiellt bara 3,3 V.
- **Egen strömförsörjning till ESP:n** (USB-laddare).

### Serviceporten (uppmätt på en 901 353 E)

4 stift, uppifrån: **GND, SCL, SDA, +5V** (mät själv först! Mot GND: +5V ≈ 5,0 V, SDA ≈ 5,0 V i vila,
SCL 4,6–4,9 V i multimeter eftersom den klockar).

### Koppling med BSS138-nivåomvandlare

| Omvandlare | Ansluts till |
|---|---|
| HV | pumpens +5V (endast referens) |
| LV | ESP 3V3 |
| GND | pumpens GND **och** ESP GND |
| HV1 / LV1 | pump SCL / ESP D1 (GPIO5) |
| HV2 / LV2 | pump SDA / ESP D2 (GPIO4) |

> **Mata ALDRIG ESP:n från pumpens +5V.** Vi provade – pumpens panel slocknade direkt. Porten klarar bara
> några mA (räcker till referens för nivåomvandlare/isolator), inte en ESP med WiFi (300–400 mA i toppar).

## Installation

1. Kopiera `esphome/` till din ESPHome-konfiguration (eller kör direkt med Docker, se kommentaren överst i
   `esphome/thermia.yaml`).
2. `cp esphome/secrets.yaml.example esphome/secrets.yaml` och fyll i WiFi m.m.
3. Programmera första gången via USB, sedan OTA.
4. Lägg till enheten i Home Assistant (ESPHome-integrationen hittar den automatiskt).
5. Jämför **Registerdump**-sensorerna med pumpens display innan du litar på något värde.

Tips: ESP8266 har få API-anslutningar. Glöm inte gamla `esphome logs`-processer/containrar igång – de tar
platserna och då kan Home Assistant inte ansluta (`EncryptionHelloAPIError`).

## Bidra

Har du en Villa Classic (eller släkting med samma reglerkort)? Jämför registerdumpen med din display och öppna
en issue/PR med vad du hittar – särskilt kompressor-, tillsats- och larmbitarna som ännu saknas.

## Källor och tack

- Värmepumpsforum: trådar om Villa Classic 55 och serviceportens pinout.
- [omakotikotitalomme.blogspot.com – Danfoss lämpöpumpun salaisuudet](https://omakotikotitalomme.blogspot.com/2015/03/danfoss-lampopumpun-salaisuudet.html) (protokoll för Danfoss/Thermia-familjen).
- [rainisto/arduino_i2c_orja](https://github.com/rainisto/arduino_i2c_orja) (Arduino-slav, GPL-3.0 – ingen kod kopierad härifrån).
- [ThermIQ/thermiq_mqtt-ha](https://github.com/ThermIQ/thermiq_mqtt-ha) (registerkarta för nyare reglerkort – gäller **inte** Villa Classic 901 353).

Thermia är ett varumärke som tillhör sin ägare; projektet har ingen koppling till Thermia.

---

## English summary

ESPHome external component that turns an ESP8266/ESP32 into an I2C slave (address 0x2E) on the 4-pin service
port of an older **Thermia Villa Classic** ground-source heat pump (controller board 901 353). The controller
pushes `[register, value]` frames by itself; sensor temperatures are encoded as °C + 40. Partial register map in
[docs/registerkarta.md](docs/registerkarta.md) (Swedish, tables are easy to follow). Read-only – nothing is ever
written to the pump. **Use entirely at your own risk**: wrong wiring can destroy the controller or the pump, and
working inside it involves mains voltage. Never power the ESP from the pump's +5 V pin. No warranty, MIT licence.
