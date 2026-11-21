#include "OpenRGBZotac5070TiPlugin.h"
#include "RGBController_Zotac5070Ti.h"
#include "i2c_smbus.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#define NVIDIA_VEN                  0x10DE
#define NVIDIA_RTX5070TI_DEV        0x2C05
#define ZOTAC_SUB_VEN               0x19DA
#define ZOTAC_RTX5070TI_SUB_DEV     0x2765
#define ZOTAC_SPECTRA_I2C_ADDR      0x49

OpenRGBZotac5070TiPlugin::OpenRGBZotac5070TiPlugin() = default;
OpenRGBZotac5070TiPlugin::~OpenRGBZotac5070TiPlugin() = default;

OpenRGBPluginInfo OpenRGBZotac5070TiPlugin::GetPluginInformation()
{
    OpenRGBPluginInfo info;
    info.Name           = "ZOTAC RTX 5070 Ti SPECTRA Bridge";
    info.Description    = "Registers ZOTAC GAMING GeForce RTX 5070 Ti SOLID OC (0x19DA:0x2765) over NVAPI/I2C directly into OpenRGB.";
    info.Version        = "1.0.0";
    info.Repository     = "https://gitlab.com/CalcProgrammer1/OpenRGB";
    info.License        = "GPL-2.0";
    info.Location       = OPENRGB_PLUGIN_LOCATION_TOP;
    info.Label          = "Zotac 5070 Ti";
    return info;
}

unsigned int OpenRGBZotac5070TiPlugin::GetPluginAPIVersion()
{
    return OPENRGB_PLUGIN_API_VERSION;
}

void OpenRGBZotac5070TiPlugin::Load(ResourceManagerInterface* resource_manager_ptr)
{
    rm = resource_manager_ptr;
    ScanAndRegisterZotacGPU();
}

void OpenRGBZotac5070TiPlugin::ScanAndRegisterZotacGPU()
{
    if(rm == nullptr)
    {
        return;
    }

    std::vector<i2c_smbus_interface*>& busses = rm->GetI2CBusses();

    for(size_t i = 0; i < busses.size(); i++)
    {
        i2c_smbus_interface* bus = busses[i];

        if(bus->pci_vendor != NVIDIA_VEN || bus->pci_device != NVIDIA_RTX5070TI_DEV)
        {
            continue;
        }

        if(bus->pci_subsystem_vendor != ZOTAC_SUB_VEN)
        {
            continue;
        }

        if(bus->pci_subsystem_device != ZOTAC_RTX5070TI_SUB_DEV)
        {
            continue;
        }

        /* Probe 7-bit I2C address 0x49 before registering controller */
        int probe = bus->i2c_smbus_read_byte_data(ZOTAC_SPECTRA_I2C_ADDR, 0x00);
        if(probe < 0)
        {
            /* Fallback: some Blackwell I2C bridges NACK reads but ACK writes */
            unsigned char ping_pkt[2] = { 0x00, 0x00 };
            if(bus->i2c_write_block_data(ZOTAC_SPECTRA_I2C_ADDR, 0x00, 2, ping_pkt) < 0)
            {
                status_line = "Matched PCI ID on bus " + std::string(bus->device_name) + " but I2C 0x49 did not ACK (check if FireStorm is running).";
                continue;
            }
        }

        Zotac5070TiController* controller = new Zotac5070TiController(bus, ZOTAC_SPECTRA_I2C_ADDR);
        RGBController_Zotac5070Ti* rgb_controller = new RGBController_Zotac5070Ti(controller);
        rgb_controller->name = "ZOTAC GAMING GeForce RTX 5070 Ti SOLID OC";

        rm->RegisterRGBController(rgb_controller);
        detected = true;
        status_line = "Registered ZOTAC GAMING GeForce RTX 5070 Ti SOLID OC on " + std::string(bus->device_name) + " (I2C 0x49).";
        break;
    }

    if(!detected && status_line.empty())
    {
        status_line = "No matching NVIDIA 0x10DE:0x2C05 / ZOTAC 0x19DA:0x2765 I2C bus found. Run OpenRGB as Administrator / check i2c-dev.";
    }
}

QWidget* OpenRGBZotac5070TiPlugin::GetWidget()
{
    if(widget != nullptr)
    {
        return widget;
    }

    widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QLabel* title = new QLabel("ZOTAC GAMING GeForce RTX 5070 Ti SOLID OC Bridge");
    QLabel* pci_label = new QLabel("Target PCI: 0x10DE:0x2C05 | Subsys: 0x19DA:0x2765 | I2C: 0x49");
    QLabel* state_label = new QLabel(QString::fromStdString(status_line));
    state_label->setWordWrap(true);

    QPushButton* rescan_btn = new QPushButton("Re-Scan I2C Busses");
    QObject::connect(rescan_btn, &QPushButton::clicked, [this, state_label]() {
        if(!detected)
        {
            ScanAndRegisterZotacGPU();
            state_label->setText(QString::fromStdString(status_line));
        }
    });

    layout->addWidget(title);
    layout->addWidget(pci_label);
    layout->addWidget(state_label);
    layout->addWidget(rescan_btn);
    layout->addStretch();

    return widget;
}

QMenu* OpenRGBZotac5070TiPlugin::GetTrayMenu()
{
    return nullptr;
}

void OpenRGBZotac5070TiPlugin::Unload()
{
    rm = nullptr;
}
