# esp32-ble-mqtt-demo

## First run

When pulling project, run (to pull dependent submodules - libraries):
```bash
git submodule add -b release/1.3 https://github.com/h2zero/esp-nimble-cpp.git components/nimBLE
```

Run `idf.py menuconfig` and check Bluetooth (BLE - NimBLE only) is selected (Component config -> Bluetooth (check) -> Bluetooth host -> NimBLE).


## Useful tips

If ESP32 (or JTAG adapter) is not recognized by computer on first run, modify udev rules:
```bash
sudo gedit /etc/udev/rules.d/esp32-local.rules
```
`
SUBSYSTEM=="usb", ATTRS{idVendor}=="0c45", ATTRS{idProduct}=="6366", OWNER="username", GROUP="plugdev", TAG+="uaccess"
`
```bash
sudo adduser username plugdev
sudo udevadm control --reload-rules
```