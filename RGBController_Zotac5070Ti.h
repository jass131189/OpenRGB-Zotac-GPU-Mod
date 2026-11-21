#pragma once

#include "RGBController.h"
#include "i2c_smbus.h"
#include <cstdint>
#include <string>

enum
{
    ZOTAC_5070TI_MODE_DIRECT      = 0x00,
    ZOTAC_5070TI_MODE_STATIC      = 0x01,
    ZOTAC_5070TI_MODE_BREATHING   = 0x02,
    ZOTAC_5070TI_MODE_COLOR_CYCLE = 0x03,
    ZOTAC_5070TI_MODE_RAINBOW     = 0x04,
    ZOTAC_5070TI_MODE_OFF         = 0x05
};

class Zotac5070TiController
{
public:
    Zotac5070TiController(i2c_smbus_interface* bus, uint8_t dev_addr);
    ~Zotac5070TiController();

    std::string GetDeviceLocation();
    void        SetZoneColor(uint8_t zone, uint8_t mode, uint8_t r, uint8_t g, uint8_t b, uint8_t speed, uint8_t brightness);

private:
    i2c_smbus_interface* bus;
    uint8_t              dev;
};

class RGBController_Zotac5070Ti : public RGBController
{
public:
    explicit RGBController_Zotac5070Ti(Zotac5070TiController* controller_ptr);
    ~RGBController_Zotac5070Ti() override;

    void SetupZones() override;
    void ResizeZone(int zone, int new_size) override;
    void DeviceUpdateLEDs() override;
    void UpdateZoneLEDs(int zone) override;
    void UpdateSingleLED(int led) override;
    void DeviceUpdateMode() override;

private:
    Zotac5070TiController* controller;
};
