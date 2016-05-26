Compiles debug mode by default.

To build and run unit tests:

```
cd ~/aoc-2016
mkdir build
cd build
cmake ..
make
```

To build an optimized version:
```
cd ~/aoc-2016
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

Binary is located in the build directory under the version you build:
release or debug.
