# mumlib2 - simple Mumble client library

Fairly simple Mumble library written in C++.


## Build

* Dependencies
  * asio
  * Protobuf
  * OpenSSL
  * Opus

The library uses CMake build system:

```
mkdir ~build && cd ~build
cmake ..
make
```


## Usage

Sample usage is covered in *src_example/mumlib2_example.cpp* file. 

Basically, you should extend *mumlib2::Callback* class to implement your own handlers.


## TODO

* login via private key
* positional audio


## Authors

* Michał Słomkowski (original author)
* Mikhail Paulyshka (maintainer of this repository)
* Auzan Muhammad
* Benedikt Wildenhain
* HeroCC
* Hiroshi Takey F
* Hunter N. Morgan
* Matthias Larisch
* Patrik Dahlström
* Scott Hardin

The library contains code from following 3rd party projects:

* mumble: https://github.com/mumble-voip/mumble
* libmumble: https://github.com/cornejo/libmumble
