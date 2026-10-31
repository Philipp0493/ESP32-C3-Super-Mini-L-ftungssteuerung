#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
export PLATFORMIO_CORE_DIR=/workspace/ambientika-pio
PIO=/workspace/ambientika-tools/bin/pio
PY=/workspace/ambientika-tools/bin/python
"$PIO" run -j 2
mkdir -p dist
"$PY" "$PLATFORMIO_CORE_DIR/packages/tool-esptoolpy/esptool.py" --chip esp32c3 merge_bin \
  -o dist/ambientika-c3-single-fan.bin --flash_mode dio --flash_freq 40m --flash_size 4MB \
  0x0000 .pio/build/esp32c3/bootloader.bin \
  0x8000 .pio/build/esp32c3/partitions.bin \
  0xe000 "$PLATFORMIO_CORE_DIR/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin" \
  0x10000 .pio/build/esp32c3/firmware.bin
"$PY" "$PLATFORMIO_CORE_DIR/packages/tool-esptoolpy/esptool.py" --chip esp32c3 image_info .pio/build/esp32c3/firmware.bin
"$PY" - <<'PY'
from pathlib import Path
import hashlib
root=Path('.')
image=(root/'dist/ambientika-c3-single-fan.bin').read_bytes()
for offset,p in [(0,'.pio/build/esp32c3/bootloader.bin'),(0x8000,'.pio/build/esp32c3/partitions.bin'),(0xe000,'/workspace/ambientika-pio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin'),(0x10000,'.pio/build/esp32c3/firmware.bin')]:
    data=Path(p).read_bytes()
    assert image[offset:offset+len(data)]==data, p
assert len(image)<=4*1024*1024
checksum=hashlib.sha256(image).hexdigest()
(root/'dist/SHA256SUMS').write_text(checksum+'  ambientika-c3-single-fan.bin\n')
print('Merged image: all four source images verified at their flash offsets; SHA256:',checksum)
PY
