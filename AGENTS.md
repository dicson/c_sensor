# Agent Instructions

## Build and Run
- Use PlatformIO commands: `pio run` to build, `pio monitor` to view output.
- Monitor speed is set to `115200`.

## Architecture & Key Files
- `src/main.cpp`: Main sensor loop, RMS current calculation, and power estimation.
- `src/enow.cpp`: ESP-NOW communication implementation.
- `src/partitions.csv`: Custom partition table for ESP32.

## Configuration & Constants
- `CONTROLLER_ADDRESS` in `src/enow.cpp` is hardcoded; update this for target hardware.
- `CALIBRATION_FACTOR` and `NOISE_THRESHOLD` in `src/main.cpp` require tuning for specific sensors.
