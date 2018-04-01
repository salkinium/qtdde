# glient needs Python 2 !!!

git clone --depth=1 https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH="$(pwd)/depot_tools":$PATH
export GYP_GENERATORS="ninja"

git clone --depth=1 https://pdfium.googlesource.com/pdfium.git
gclient config --unmanaged --name=pdfium https://pdfium.googlesource.com/pdfium.git

# Get rid of unused and big dependencies
# UNIX tools are too stupid for multiline find/replace
gsed -i 's/^.*skia".*$//' pdfium/DEPS
gsed -i 's/^.*skia\.git.*$//' pdfium/DEPS
gsed -i 's/^.*corpus".*$//' pdfium/DEPS
gsed -i 's/^.*pdfium_tests@.*$//' pdfium/DEPS
gsed -i 's/^.*v8".*$//' pdfium/DEPS
gsed -i 's/^.*v8\.git.*$//' pdfium/DEPS
gsed -i 's/^.*v8.*$//g' pdfium/.gn

gclient sync --no-history

cd pdfium
mkdir -p out/release
mkdir -p out/debug
cat <<EOS > out/release/args.gn
# Build arguments go here.
# See "gn args <out_dir> --list" for available build arguments.
use_goma=false
pdf_use_skia=false
pdf_use_skia_paths=false
pdf_enable_xfa=false
pdf_enable_v8=false
pdf_is_standalone=true
pdf_is_complete_lib=true
is_component_build=false
clang_use_chrome_plugins=false
mac_deployment_target="10.12.0"
EOS
cat <<EOS > out/debug/args.gn
# Build arguments go here.
# See "gn args <out_dir> --list" for available build arguments.
use_goma=false
pdf_use_skia=false
pdf_use_skia_paths=false
pdf_enable_xfa=false
pdf_enable_v8=false
pdf_is_standalone=true
pdf_is_complete_lib=true
is_component_build=false
clang_use_chrome_plugins=false
mac_deployment_target="10.12.0"
is_debug=true
symbol_level=2
strip_absolute_paths_from_debug_symbols=false
strip_debug_info=false
EOS

gn gen out/debug
gn gen out/release

ninja -C out/debug pdfium_all
ninja -C out/release pdfium_all

cd ..

cp pdfium/out/debug/obj/libpdfium.a ../lib/libpdfium_debug.a
cp pdfium/out/release/obj/libpdfium.a ../lib/libpdfium_release.a
