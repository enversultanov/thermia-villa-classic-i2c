#pragma once
// I2C-slav mot Thermia Villa-serviceporten. Bara läsning – se __init__.py och docs/registerkarta.md.
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include <Wire.h>

namespace esphome {
namespace thermia_i2c {

static const char *const TAG = "thermia_i2c";

class ThermiaI2C : public Component {
 public:
  void set_pins(uint8_t sda, uint8_t scl) { sda_ = sda; scl_ = scl; }
  void set_address(uint8_t a) { address_ = a; }
  void set_request_registers(bool r) { request_ = r; }
  void set_last_register(uint8_t r) { last_register_ = r; }
  float get_setup_priority() const override { return setup_priority::DATA; }

  void setup() override {
    instance_ = this;
    Wire.begin(sda_, scl_, address_);
    Wire.onReceive(on_receive_);
    Wire.onRequest(on_request_);
    ESP_LOGI(TAG, "I2C-slav på 0x%02X (SDA GPIO%u, SCL GPIO%u), registerförfrågan %s", address_, sda_, scl_,
             request_ ? "PÅ" : "AV");
  }

  void loop() override {
    while (tail_ != head_) {
      Frame &f = buf_[tail_];
      handle_frame_(f.d, f.len);
      tail_ = (tail_ + 1) % NBUF;
    }
    if (overflow_) {
      ESP_LOGW(TAG, "Buffert full – %u ramar tappade", overflow_);
      overflow_ = 0;
    }
    uint32_t now = millis();
    if (now - last_summary_ > 60000) {
      last_summary_ = now;
      log_summary_();
    }
  }

  bool has(uint8_t r) const { return r < 128 && seen_[r]; }
  float value(uint8_t r) const { return has(r) ? (float) regs_[r] : NAN; }
  bool flag(uint8_t r, uint8_t mask) const { return has(r) && (regs_[r] & mask); }
  uint32_t frames() const { return frames_; }
  uint32_t updates() const { return updates_; }
  uint8_t registers_seen() const {
    uint8_t n = 0;
    for (bool s : seen_) n += s;
    return n;
  }
  std::string last_frame() const { return last_frame_; }
  // Alla sedda register som "00:19 01:1 ..." (för HA-sensorn Registerdump)
  // HA-textsensorer max 255 tecken -> dela upp i intervall (från, till).
  std::string dump(uint8_t from = 0, uint8_t to = 127) const {
    std::string out;
    char b[12];
    for (uint8_t r = from; r <= to && r < 128; r++)
      if (seen_[r]) { snprintf(b, sizeof(b), "%02X:%d ", r, regs_[r]); out += b; }
    return out;
  }

 protected:
  static const uint8_t NBUF = 32;
  struct Frame {
    uint8_t len;
    uint8_t d[32];
  };

  // Anropas av Wire när pumpen skriver till oss – bara kopiera till ringbufferten.
  static void on_receive_(int n) {
    ThermiaI2C *s = instance_;
    uint8_t next = (s->head_ + 1) % NBUF;
    if (next == s->tail_) {
      while (Wire.available()) Wire.read();
      s->overflow_++;
      return;
    }
    Frame &f = s->buf_[s->head_];
    f.len = 0;
    while (Wire.available() && f.len < sizeof(f.d)) f.d[f.len++] = Wire.read();
    while (Wire.available()) Wire.read();
    s->head_ = next;
  }

  // Pumpen läser från oss: 0xFF = inget att begära, annars ett registernummer (< 0x80, alltid läsning).
  static void on_request_() {
    ThermiaI2C *s = instance_;
    if (!s->request_) {
      Wire.write((uint8_t) 0xFF);
      return;
    }
    uint8_t r = s->next_req_;
    s->next_req_ = (r >= s->last_register_) ? 0 : r + 1;
    Wire.write((uint8_t) (r & 0x7F));
    s->requests_++;
  }

  void handle_frame_(const uint8_t *d, uint8_t len) {
    frames_++;
    char hex[3 * 32 + 1];
    size_t p = 0;
    for (uint8_t i = 0; i < len && p + 3 < sizeof(hex); i++) p += snprintf(hex + p, sizeof(hex) - p, "%02X ", d[i]);
    hex[p] = 0;
    last_frame_ = hex;
    ESP_LOGV(TAG, "ram %u byte: %s", len, hex);
    // [reg, låg, hög] (Danfoss/ThermIQ) – även flera tripletter i följd
    if (len >= 3 && len % 3 == 0) {
      for (uint8_t i = 0; i + 2 < len; i += 3) store_(d[i], (int16_t) (d[i + 1] | (d[i + 2] << 8)));
    } else if (len == 2) {  // [reg, data] (beskrivet på Värmepumpsforum för Villa Classic)
      store_(d[0], d[1]);
    }
  }

  void store_(uint8_t reg, int16_t v) {
    if (reg >= 0x80) return;  // skrivflagga – ska aldrig komma från pumpen till oss
    if (!seen_[reg] || regs_[reg] != v) ESP_LOGI(TAG, "r%02X (%3u) = %d", reg, reg, v);
    regs_[reg] = v;
    seen_[reg] = true;
    updates_++;
  }

  void log_summary_() {
    ESP_LOGI(TAG, "Sammanfattning: %u ramar, %u registervärden, %u olika register, %u förfrågningar", frames_, updates_,
             registers_seen(), requests_);
    for (uint8_t r = 0; r < 128; r++)
      if (seen_[r]) ESP_LOGI(TAG, "  r%02X = %d", r, regs_[r]);
  }

  static ThermiaI2C *instance_;
  uint8_t sda_{4}, scl_{5}, address_{0x2E}, last_register_{0x74};
  bool request_{false};
  volatile uint8_t head_{0}, tail_{0};
  volatile uint32_t overflow_{0}, requests_{0};
  volatile uint8_t next_req_{0};
  Frame buf_[NBUF];
  int16_t regs_[128]{};
  bool seen_[128]{};
  uint32_t frames_{0}, updates_{0}, last_summary_{0};
  std::string last_frame_;
};

}  // namespace thermia_i2c
}  // namespace esphome
