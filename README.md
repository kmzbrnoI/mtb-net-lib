hJOP MTB Network RCS Library
============================

This repository contains library intended to be compiled into Windows `dll`
file and linked to [hJOPserver](https://github.com/kmzbrnoI/hJOPserver) as [RCS
library](https://hjop.kmz-brno.cz/rcs). It connects to hJOP on one side and
to [MTB daemon JSON server](https://github.com/kmzbrnoI/mtb-daemon) on the
other side. The library allows to connect hJOP with MTBbus.

## Building & toolkit

This library was developed in `vim` using `qmake` & `make`. For `clang-tidy` to
run, it is suggested to use `clang` as a compiler. Library is assumed to be
compiled on Windows to produce `dll` file.

### Prerequisities

 * Qt 5
 * [Bear](https://github.com/rizsotto/Bear)

### Toolchain setup on debian

```bash
$ apt install qt5-default
$ apt install bear
$ apt install clang-7 clang-tools-7 clang-tidy-7 clang-format-7
```

### Build

Clone this repository:

```
$ git clone https://github.com/kmzbrnoI/mtb-net-lib
```

And then build:

```
$ mkdir build
$ cd build
$ qmake ..
$ # qmake CONFIG+=debug ..
$ bear make
```

## Style checking

```bash
$ cd src
$ clang-tidy-7 -p ../build -extra-arg-before=-x -extra-arg-before=c++ -extra-arg=-std=c++17 -header-filter=src/ *.cpp
$ clang-format-7 *.cpp *.h
$ clang-include-fixer-7 -p ../build *.cpp
```

For `clang-include-fixer` to work, it is necessary to [build `yaml` symbols
database](https://clang.llvm.org/extra/clang-include-fixer.html#creating-a-symbol-index-from-a-compilation-database).
You can do it this way:

 1. Download `run-find-all-symbols.py` from [github repo](https://github.com/microsoft/clang-tools-extra/blob/master/include-fixer/find-all-symbols/tool/run-find-all-symbols.py).
 2. Execute it in `build` directory.

## Authors

This library was created by:

 * Jan Horacek ([jan.horacek@kmz-brno.cz](mailto:jan.horacek@kmz-brno.cz))

Do not hesitate to contact author in case of any troubles!

## License

This application is released under the [Apache License v2.0
](https://www.apache.org/licenses/LICENSE-2.0).
