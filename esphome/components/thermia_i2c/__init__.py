"""ESPHome-komponent: I2C-slav mot servicporten på Thermia Villa (Classic) – se docs/registerkarta.md.

Pumpens reglerdator är I2C-master och skickar [register, låg, hög] till slaven på 0x2E.
Komponenten läser bara: den skickar aldrig skrivkommandon (register | 0x80).
"""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_ADDRESS, CONF_SDA, CONF_SCL
from esphome import pins

CODEOWNERS = []
DEPENDENCIES = []

CONF_REQUEST_REGISTERS = "request_registers"
CONF_LAST_REGISTER = "last_register"

thermia_ns = cg.esphome_ns.namespace("thermia_i2c")
ThermiaI2C = thermia_ns.class_("ThermiaI2C", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(ThermiaI2C),
        cv.Optional(CONF_ADDRESS, default=0x2E): cv.i2c_address,
        cv.Optional(CONF_SDA, default=4): pins.internal_gpio_pin_number,
        cv.Optional(CONF_SCL, default=5): pins.internal_gpio_pin_number,
        # false = svara bara "inget" (0xFF) när pumpen frågar; true = be pumpen skicka register 0..last_register i tur och ordning
        cv.Optional(CONF_REQUEST_REGISTERS, default=False): cv.boolean,
        cv.Optional(CONF_LAST_REGISTER, default=0x74): cv.int_range(min=0, max=0x7F),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_pins(config[CONF_SDA], config[CONF_SCL]))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_request_registers(config[CONF_REQUEST_REGISTERS]))
    cg.add(var.set_last_register(config[CONF_LAST_REGISTER]))
    cg.add_library("Wire", None)
