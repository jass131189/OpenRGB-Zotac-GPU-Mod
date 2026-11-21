#include "RGBController_Zotac5070Ti.h"
#include <cstdio>

Zotac5070TiController::Zotac5070TiController(i2c_smbus_interface* bus_ptr, uint8_t dev_addr)
    : bus(bus_ptr), dev(dev_addr)
{
}

Zotac5070TiController::~Zotac5070TiController() = default;

std::string Zotac5070TiController::GetDeviceLocation()
{
    char addr_buf[32];
    std::snprintf(addr_buf, sizeof(addr_buf), "I2C: %s, address 0x%02X", bus->device_name, dev);
    return std::string(addr_buf);
}

void Zotac5070TiController::SetZoneColor(
    uint8_t zone,
    uint8_t mode,
    uint8_t r,
    uint8_t g,
    uint8_t b,
    uint8_t speed,
    uint8_t brightness
)
{
    uint8_t packet[10] =
    {
        0x0F,
        0x00,
        zone,
        mode,
        r,
        g,
        b,
        speed,
        static_cast<uint8_t>((brightness * 255) / 100),
        0x01
    };
    bus->i2c_smbus_write_i2c_block_data(dev, 0x0F, sizeof(packet), packet);
}

RGBController_Zotac5070Ti::RGBController_Zotac5070Ti(Zotac5070TiController* controller_ptr)
    : controller(controller_ptr)
{
    name        = "ZOTAC GAMING GeForce RTX 5070 Ti SOLID OC";
    vendor      = "ZOTAC";
    type        = DEVICE_TYPE_GPU;
    description = "ZOTAC GAMING GeForce RTX 5070 Ti SPECTRA RGB Controller";
    location    = controller->GetDeviceLocation();

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = ZOTAC_5070TI_MODE_DIRECT;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Direct.color_mode = MODE_COLORS_PER_LED;
    Direct.brightness_min = 0;
    Direct.brightness_max = 100;
    Direct.brightness     = 100;
    modes.push_back(Direct);

    mode Static;
    Static.name       = "Static";
    Static.value      = ZOTAC_5070TI_MODE_STATIC;
    Static.flags      = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Static.color_mode = MODE_COLORS_PER_LED;
    Static.brightness_min = 0;
    Static.brightness_max = 100;
    Static.brightness     = 100;
    modes.push_back(Static);

    mode Breathing;
    Breathing.name       = "Breathing";
    Breathing.value      = ZOTAC_5070TI_MODE_BREATHING;
    Breathing.flags      = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    Breathing.color_mode = MODE_COLORS_PER_LED;
    Breathing.speed_min  = 1;
    Breathing.speed_max  = 10;
    Breathing.speed      = 5;
    Breathing.brightness_min = 0;
    Breathing.brightness_max = 100;
    Breathing.brightness     = 100;
    modes.push_back(Breathing);

    mode ColorCycle;
    ColorCycle.name       = "Spectrum Cycle";
    ColorCycle.value      = ZOTAC_5070TI_MODE_COLOR_CYCLE;
    ColorCycle.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    ColorCycle.color_mode = MODE_COLORS_NONE;
    ColorCycle.speed_min  = 1;
    ColorCycle.speed_max  = 10;
    ColorCycle.speed      = 5;
    ColorCycle.brightness_min = 0;
    ColorCycle.brightness_max = 100;
    ColorCycle.brightness     = 100;
    modes.push_back(ColorCycle);

    mode Rainbow;
    Rainbow.name       = "Rainbow Wave";
    Rainbow.value      = ZOTAC_5070TI_MODE_RAINBOW;
    Rainbow.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    Rainbow.color_mode = MODE_COLORS_NONE;
    Rainbow.speed_min  = 1;
    Rainbow.speed_max  = 10;
    Rainbow.speed      = 5;
    Rainbow.brightness_min = 0;
    Rainbow.brightness_max = 100;
    Rainbow.brightness     = 100;
    modes.push_back(Rainbow);

    mode Off;
    Off.name       = "Off";
    Off.value      = ZOTAC_5070TI_MODE_OFF;
    Off.flags      = 0;
    Off.color_mode = MODE_COLORS_NONE;
    modes.push_back(Off);

    SetupZones();
}

RGBController_Zotac5070Ti::~RGBController_Zotac5070Ti()
{
    delete controller;
}

void RGBController_Zotac5070Ti::SetupZones()
{
    zone z_side_lightbar;
    z_side_lightbar.name       = "Side Lightbar";
    z_side_lightbar.type       = ZONE_TYPE_SINGLE;
    z_side_lightbar.leds_min   = 1;
    z_side_lightbar.leds_max   = 1;
    z_side_lightbar.leds_count = 1;
    z_side_lightbar.matrix_map = nullptr;
    zones.push_back(z_side_lightbar);

    led l_side_lightbar;
    l_side_lightbar.name = "Side Lightbar";
    leds.push_back(l_side_lightbar);

    zone z_spectra_emblem;
    z_spectra_emblem.name       = "SPECTRA Emblem";
    z_spectra_emblem.type       = ZONE_TYPE_SINGLE;
    z_spectra_emblem.leds_min   = 1;
    z_spectra_emblem.leds_max   = 1;
    z_spectra_emblem.leds_count = 1;
    z_spectra_emblem.matrix_map = nullptr;
    zones.push_back(z_spectra_emblem);

    led l_spectra_emblem;
    l_spectra_emblem.name = "SPECTRA Emblem";
    leds.push_back(l_spectra_emblem);

    SetupColors();
}

void RGBController_Zotac5070Ti::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /* Fixed hardware zones */
}

void RGBController_Zotac5070Ti::DeviceUpdateLEDs()
{
    for(size_t i = 0; i < zones.size(); i++)
    {
        UpdateZoneLEDs(static_cast<int>(i));
    }
}

void RGBController_Zotac5070Ti::UpdateZoneLEDs(int zone)
{
    if(zone < 0 || static_cast<size_t>(zone) >= colors.size())
    {
        return;
    }

    RGBColor color      = colors[zone];
    uint8_t  r          = RGBGetRValue(color);
    uint8_t  g          = RGBGetGValue(color);
    uint8_t  b          = RGBGetBValue(color);
    uint8_t  mode_val   = static_cast<uint8_t>(modes[active_mode].value);
    uint8_t  speed      = static_cast<uint8_t>(modes[active_mode].speed);
    uint8_t  brightness = static_cast<uint8_t>(modes[active_mode].brightness);

    controller->SetZoneColor(static_cast<uint8_t>(zone), mode_val, r, g, b, speed, brightness);
}

void RGBController_Zotac5070Ti::UpdateSingleLED(int led)
{
    UpdateZoneLEDs(led);
}

void RGBController_Zotac5070Ti::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}
