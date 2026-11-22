#!/usr/bin/env bash
# Baut die Firmware und erzeugt drei Dateien in dist/v2:
#   …-komplett-0x0.bin     Erstinstallation per USB an 0x0 (löscht alle Einstellungen)
#   …-update-0xE000.bin    Update per USB an 0xE000 (Einstellungen, WLAN und API-Schlüssel bleiben)
#   …-ota.bin              Update über die Weboberfläche (Abschnitt „Firmware-Update“)
set -euo pipefail
cd "$(dirname "$0")/.."
TOOLS=${AMBIENTIKA_TOOLS:-/workspace/ambientika-tools}
export PLATFORMIO_CORE_DIR=${PLATFORMIO_CORE_DIR:-/workspace/ambientika-pio}
PIO="$TOOLS/bin/pio"
PY="$TOOLS/bin/python"
VERSION=v2
OUT=dist/$VERSION
NAME=ambientika-c3-$VERSION
BUILD=.pio/build/esp32c3
ESPTOOL="$PLATFORMIO_CORE_DIR/packages/tool-esptoolpy/esptool.py"
BOOT_APP0="$PLATFORMIO_CORE_DIR/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin"

"$PIO" run -j 2
mkdir -p "$OUT"
"$PY" "$ESPTOOL" --chip esp32c3 merge_bin -o "$OUT/$NAME-komplett-0x0.bin" \
  --flash_mode dio --flash_freq 40m --flash_size 4MB \
  0x0000 "$BUILD/bootloader.bin" 0x8000 "$BUILD/partitions.bin" 0xe000 "$BOOT_APP0" 0x10000 "$BUILD/firmware.bin"
# Ab 0xE000: boot_app0 (startet wieder ota_0) + Anwendung. Der NVS-Bereich (0x9000–0xDFFF) bleibt unberührt.
"$PY" "$ESPTOOL" --chip esp32c3 merge_bin -o "$OUT/$NAME-update-0xE000.bin" --target-offset 0xe000 \
  0xe000 "$BOOT_APP0" 0x10000 "$BUILD/firmware.bin"
cp "$BUILD/firmware.bin" "$OUT/$NAME-ota.bin"
"$PY" "$ESPTOOL" --chip esp32c3 image_info "$BUILD/firmware.bin"

"$PY" - "$OUT" "$NAME" "$BUILD" "$BOOT_APP0" <<'PY'
import hashlib, sys
from pathlib import Path
out, name, build, boot_app0 = Path(sys.argv[1]), sys.argv[2], Path(sys.argv[3]), Path(sys.argv[4])
full = (out / f'{name}-komplett-0x0.bin').read_bytes()
upd = (out / f'{name}-update-0xE000.bin').read_bytes()
parts = [(0, build / 'bootloader.bin'), (0x8000, build / 'partitions.bin'), (0xe000, boot_app0), (0x10000, build / 'firmware.bin')]
for offset, p in parts:
    data = p.read_bytes()
    assert full[offset:offset + len(data)] == data, p
assert full[0xe000:] == upd, 'update image must equal complete image from 0xE000'
app = (build / 'firmware.bin').read_bytes()
assert len(app) <= 0x140000, 'firmware larger than OTA partition'
lines = [f'{hashlib.sha256(f.read_bytes()).hexdigest()}  {f.name}' for f in sorted(out.glob('*.bin'))]
(out / 'SHA256SUMS').write_text('\n'.join(lines) + '\n')
print('Images verified; app size', len(app), 'of', 0x140000)
print('\n'.join(lines))
PY
