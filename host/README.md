# LED Wristwatch Host Software

## Build Instructions (Linux)

Prerequisites:

 - cmake
 - g++ compatible with C++11
 - gtkmm-3.0
 - hidapi-hidraw

To build the first time:

```
$ mkdir build
$ cd build
$ cmake ../
$ make
```

To build the Nth time:

```
$ cd build && make
```

To ensure source updates occur after creating a new file:

```
$ touch CMakeLists.txt
$ cd build && make
```

To run:

```
$ ./build/wristwatch
```

## Troubleshooting

Not able to find device, even though it is plugged in and working properly:

 - Did you remember to run the program with sudo?
 - Did you remember to make a udev rule for hidraw to avoid sudo (add user to
   plugdev group):

```
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0664", GROUP="plugdev"
```

