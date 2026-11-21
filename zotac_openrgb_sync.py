# Zero-compile OpenRGB 1.1 SDK -> ZOTAC RTX 5070 Ti NVAPI Bridge
# Requires: pip install openrgb-python
# Run OpenRGB 1.1 with Settings -> SDK Server enabled (Start Server), then run this script as Administrator.

import ctypes
import sys
import time
from openrgb import OpenRGBClient

I2C_ADDR_7BIT = 0x49
POLL_INTERVAL_SEC = 0.05

# NVAPI constants (Windows x64 nvapi64.dll)
NVAPI_QUERY_INTERFACE_ID_ENUM_GPUS = 0xE5AC921F
NVAPI_QUERY_INTERFACE_ID_I2C_WRITE_EX = 0x283AC65A


class NvI2CInfoV3(ctypes.Structure):
    _fields_ = [
        ("version", ctypes.c_uint32),
        ("displayMask", ctypes.c_uint32),
        ("bIsDDCPort", ctypes.c_uint8),
        ("i2cDevAddress", ctypes.c_uint8),
        ("pbI2cRegAddress", ctypes.POINTER(ctypes.c_uint8)),
        ("regAddrSize", ctypes.c_uint32),
        ("pbData", ctypes.POINTER(ctypes.c_uint8)),
        ("cbSize", ctypes.c_uint32),
        ("i2cSpeed", ctypes.c_uint32),
        ("i2cSpeedKhz", ctypes.c_uint32),
        ("portId", ctypes.c_uint8),
        ("bIsPortIdSet", ctypes.c_uint32),
    ]


def init_nvapi():
    nvapi = ctypes.WinDLL("nvapi64.dll")
    query_iface = nvapi.nvapi_QueryInterface
    query_iface.restype = ctypes.c_void_p
    query_iface.argtypes = [ctypes.c_uint32]

    init_ptr = query_iface(0x0150E828)
    enum_ptr = query_iface(NVAPI_QUERY_INTERFACE_ID_ENUM_GPUS)
    write_ptr = query_iface(NVAPI_QUERY_INTERFACE_ID_I2C_WRITE_EX)

    init_fn = ctypes.CFUNCTYPE(ctypes.c_int)(init_ptr)
    if init_fn() != 0:
        raise RuntimeError("NvAPI_Initialize failed.")

    enum_fn = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.POINTER(ctypes.c_void_p * 64), ctypes.POINTER(ctypes.c_uint32))(enum_ptr)
    handles = (ctypes.c_void_p * 64)()
    count = ctypes.c_uint32(0)
    enum_fn(ctypes.byref(handles), ctypes.byref(count))
    if count.value == 0:
        raise RuntimeError("No NVIDIA GPU found via NVAPI.")

    write_fn = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p, ctypes.POINTER(NvI2CInfoV3), ctypes.POINTER(ctypes.c_uint32))(write_ptr)
    return handles[0], write_fn


def write_zotac_color(gpu_handle, write_fn, r, g, b):
    payload = (ctypes.c_uint8 * 10)(0x0F, 0x00, 0x00, 0x00, r, g, b, 0x05, 0xFF, 0x01)
    reg = (ctypes.c_uint8 * 1)(0x0F)
    info = NvI2CInfoV3()
    info.version = ctypes.sizeof(NvI2CInfoV3) | (3 << 16)
    info.displayMask = 0
    info.bIsDDCPort = 0
    info.i2cDevAddress = (I2C_ADDR_7BIT << 1)
    info.pbI2cRegAddress = reg
    info.regAddrSize = 1
    info.pbData = payload
    info.cbSize = 10
    info.i2cSpeed = 0xFFFF
    info.i2cSpeedKhz = 100
    info.portId = 1
    info.bIsPortIdSet = 1
    unknown = ctypes.c_uint32(0)
    return write_fn(gpu_handle, ctypes.byref(info), ctypes.byref(unknown))


def main():
    if sys.platform != "win32":
        print("nvapi64.dll sync bridge is Windows-only.")
        return
    gpu_handle, write_fn = init_nvapi()
    client = OpenRGBClient()
    if not client.devices:
        print("No OpenRGB devices found to mirror.")
        return

    source_dev = client.devices[0]
    print(f"Mirroring OpenRGB device '{source_dev.name}' -> ZOTAC GPU I2C 0x{I2C_ADDR_7BIT:02X}")
    last_rgb = None
    while True:
        source_dev.update()
        if source_dev.colors:
            c = source_dev.colors[0]
            rgb = (c.red, c.green, c.blue)
            if rgb != last_rgb:
                write_zotac_color(gpu_handle, write_fn, *rgb)
                last_rgb = rgb
        time.sleep(POLL_INTERVAL_SEC)


if __name__ == "__main__":
    main()
