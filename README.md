# Introduction

A simple Vehicle Signal Specification (VSS) signal to MQTT proxy, using the KUKSA.val databroker as VSS source.  The MQTT payloads are protobufs loosely derived from the update entries in the KUKSA.val gRPC VAL API.  See `protos/vss-notification.proto` for the protobuf definition.

# Support

Support options include:
- #automotive IRC channel on Libera.chat, see https://libera.chat/guides/ for guidance on using IRC. 
- AGL Vehicle to Cloud (V2C) expert group (EG) Zoom call every second Monday, see calendar at https://lists.automotivelinux.org/g/agl-dev-community/calendar.
- AGL community mailing list, see https://lists.automotivelinux.org/g/agl-dev-community for subscription information and archives.

# Building

Required dependencies:
- g++
- protoc
- grpc
- grpc++
- glib 2.0
- openssl
- libmosquitto
- yaml-cpp

The development packages for the above need to be available in your build environment.

Compilation also requires the KUKSA.val VAL API protobuf definitions.  To get them, clone the KUKSA.val repository from https://github.com/eclipse/kuksa.val.git.  A build can then be configued by running:
```
meson -Dprotos=<kuksa.val/proto> [build directory]
```
where `kuksa.val` is the path to your clone of the kuksa.val repository, and `build directory` is an optional build directory that meson will configure (default is `build` in the current directory).  The proxy can then be built by running `ninja` in the created build directory.

# Configuration

The proxy requires a configuation file in YAML syntax.  If run with no arguments, it will look for it in `/etc/agl-vss-proxy/config.yaml`.  Alternatively, a different file can be specified on the command-line with the `--config-file` option.  Specifying signals to proxy is the only mandatory requirement in the configuration file.  An example of a minimal configuration file is:
```
signals:
  - Vehicle.Speed
  ```
  This will result in attempting to use local instances of the KUKSA.val databroker and a MQTT broker with no TLS and default values for the payload client ID, MQTT ID, and MQTT topic.  See `src/config.yaml.example` for a full example configuration file that documents the various options.

# Source Provenance

Some source code from previous AGL projects has been adapted for the proxy, it is documented below to clarify copyrights.

From agl-service-hvac (https://git.automotivelinux.org/apps/agl-service-hvac) as of commit 6ebb449:
- meson.build
- meson_options.txt
- src/meson.build
- src/main.cpp
- src/HvacService.* -> src/ProxyService.*
- src/KuksaClient.*
- src/KuksaConfig.*

From agl-telematics-demo-recorder (https://git.automotivelinux.org/apps/agl-telematics-demo-recorder) as of commit a69d2e0b:
- app/mqttclient.* -> src/MqttClient.*

# TODOs

Planned features that are currently not implemented:
- Signal rate-limiting with frequency/period option
- Per-signal conditions to control when signals are proxied.
- Capturing/snapshotting signal state when a condition is triggered.

Other potential features:
- MQTT v5 message properties support
- Pre-shared key support
- VIS client support to support other VSS server implementations