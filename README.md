Make sure the mingw-w64-toolchain is installed.

To build
```
  mkdir build_windows && cd build_windows
  cmake -DCMAKE_TOOLCHAIN_FILE=../mingw-w64-toolchain.cmake ..
  make -j${ncores}
```
