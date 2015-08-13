# build file to generate the distribution binary tarball

make clean
autoreconf

./configure \
  $PKGCFG \
  $CROSSCFG \
  --with-ptw32=$PREFIX/usr/i686-w64-mingw32.static \
  PTW32_LIBS="-lpthread -lpcreposix -lpcre" \
  FLTK_CONFIG=$PREFIX/bin/i686-w64-mingw32.static-fltk-config

make

$PREFIX/bin/i686-w64-mingw32.static-strip src/flnet.exe
make nsisinst
mv src/*setup*exe .

make clean

# build the distribution tarball
./configure
make distcheck
make clean
