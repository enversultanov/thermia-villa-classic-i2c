"""ESPHome component: I2C slave on the service port of Thermia Villa (Classic) heat pumps – see docs/register-map.md.

The pump controller is the I2C master and sends frames to the slave at 0x2E: [register, value] (Villa Classic 901 353)
or [register, low, high] (Danfoss/ThermIQ family). Read-only: the component never sends write commands (register | 0x80).
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
        # false = only answer "nothing" (0xFF) when polled; true = ask the pump for registers 0..last_register in turn
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
