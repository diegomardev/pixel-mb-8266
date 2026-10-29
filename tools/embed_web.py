"""Incrusta la interfaz sin CDN ni carga de archivos web adicional."""
from pathlib import Path
import zlib
try:
    Import('env')  # PlatformIO/SCons
    root = Path(env.subst('$PROJECT_DIR'))
except NameError:
    root = Path(__file__).resolve().parents[1]
html = (root / 'web/index.html').read_text(encoding='utf-8')
assert ')AWXHTML"' not in html
header = '#pragma once\n#include <Arduino.h>\n// Generado desde web/index.html por tools/embed_web.py\nstatic const char WEB_UI[] PROGMEM = R"AWXHTML(' + html + ')AWXHTML";\n'
target = root / 'include/WebUI.h'
if not target.exists() or target.read_text(encoding='utf-8') != header:
    target.write_text(header, encoding='utf-8')

# A 1 KiB DEFLATE window keeps first-boot RAM bounded on ESP8266.
# Build the bundled SDK inflater instead of downloading another dependency.
try:
    sdk = Path(env.PioPlatform().get_package_dir('framework-arduinoespressif8266')) / 'tools/sdk/uzlib/src'
    env.Append(CPPPATH=[str(sdk)])
    env.BuildSources('$BUILD_DIR/pixelmb_uzlib', str(sdk), src_filter='-<*> +<tinflate.c> +<tinfzlib.c> +<adler32.c> +<crc32.c>')
except NameError:
    pass
# Install the complete default library once; existing installations are preserved.
parts = ['#pragma once\n#include <Arduino.h>\n']
entries = []
for index, source in enumerate(sorted((root / 'data/animations').glob('*.awx'))):
    symbol = f'DEFAULT_ANIMATION_{index}'
    raw = source.read_bytes()
    compressor = zlib.compressobj(9, zlib.DEFLATED, 10)
    data = compressor.compress(raw) + compressor.flush()
    parts.append(f'static const uint8_t {symbol}[] PROGMEM = {{\n')
    for start in range(0, len(data), 24):
        parts.append(','.join(f'0x{b:02x}' for b in data[start:start+24]) + ',\n')
    parts.append('};\n')
    pack = 4
    entries.append(f'{{"{source.stem}", {symbol}, sizeof({symbol}), {len(raw)}, {pack}}}')
parts.append('struct DefaultAnimation { const char *name; const uint8_t *data; size_t size; size_t rawSize; uint8_t pack; };\n')
parts.append('static const DefaultAnimation DEFAULT_ANIMATIONS[] = {\n' + ',\n'.join(entries) + '\n};\n')
target = root / 'include/DefaultAnimations.h'
content = ''.join(parts)
if not target.exists() or target.read_text() != content:
    target.write_text(content)

# Interfaz del actualizador, protegida por la autenticación Digest existente.
update = (root / "web/update.html").read_text(encoding="utf-8")
assert ")UPDATEHTML\"" not in update
content = '#pragma once\n#include <Arduino.h>\nstatic const char UPDATE_UI[] PROGMEM = R"UPDATEHTML(' + update + ')UPDATEHTML";\n'
target = root / "include/UpdateUI.h"
if not target.exists() or target.read_text() != content:
    target.write_text(content)
