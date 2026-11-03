#include "IntegrationTestUtil.hpp"
#include "LibReser/Json/Backend.hpp"
#include "LibReser/Toml/Parser.hpp"
#include "LibReser/Toml/Printer.hpp"
#include "LibReser/Toml/Backend.hpp"
#include "LibReser/Xml/Backend.hpp"
#include "gtest/gtest.h"

using namespace reser;

TEST(Integration, TomlJsonXmlRoundTrip) {
  static const std::string_view input = R"(
    [package]
    edition      = "2024"
    name         = "esp32-led-controller"
    rust-version = "1.88"
    version      = "0.1.0"
    
    [[bin]]
    name = "esp32-led-controller"
    path = "./src/main.rs"
    
    [dependencies]
    esp-hal = { version = "~1.1.0", features = ["defmt", "esp32c3", "unstable"] }
    heapless = { version = "0.9.3", features = ["serde"] }
    picoserve = { version = "0.18.0", features = ["embassy", "defmt", "ws"] }
    serde = { version = "=1.0.228", default-features = false, features = [ "alloc", "derive" ] }
    serde-json-core = "0.6.0"
    esp-hal-smartled = { git = "https://github.com/aelsi2/esp-hal-community" , features = [ "esp32c3" ] }
    smart-leds = "0.4.0"
    thiserror = { version = "2.0.18", default-features = false }
    async-trait = "0.1.89"
    esp-rtos = { version = "0.3.0", features = [
      "defmt",
      "embassy",
      "esp-alloc",
      "esp-radio",
      "esp32c3",
    ] }
    esp-storage = { version = "0.9.0", features = ["esp32c3", "defmt"] }
    littlefs2 = { version = "0.7.1", features = ["c-stubs"] }
    delog = { version = "0.1.8", features = ["portable-atomic"] }
    embedded-storage = "0.3.1"
    embedded-graphics = "0.8.2"
    ssd1306 = "0.10.0"
    
    defmt                  = { version = "1.0.1", features = ["alloc"] }
    esp-bootloader-esp-idf = { version = "0.5.0", features = ["defmt", "esp32c3"] }
    
    embassy-net = { version = "0.9.1", features = [
      "defmt",
      "dhcpv4",
      "dhcpv4-hostname",
      "medium-ethernet",
      "tcp",
      "udp",
    ] }
    embedded-io = { version = "0.7.1", features = ["defmt"] }
    embedded-io-async = { version = "0.7.0", features = ["defmt"] }
    esp-alloc = { version = "0.10.0", features = ["defmt"] }
    esp-backtrace = { version = "0.19.0", features = [
      "defmt",
      "esp32c3",
      "panic-handler",
    ] }
    esp-println = { version = "0.17.0", features = ["defmt-espflash", "esp32c3"] }
    # for more networking protocol support see https://crates.io/crates/edge-net
    embassy-executor = { version = "0.10.0", features = ["defmt"] }
    embassy-sync = { version = "0.8.0", features = ["defmt"] }
    embassy-futures = "0.1.2"
    embassy-time = { version = "0.5.0", features = ["defmt"] }
    esp-radio = { version = "0.18.0", features = [
      "defmt",
      "esp-alloc",
      "esp32c3",
      "unstable",
      "wifi",
    ] }
    smoltcp = { version = "0.13.0", default-features = false, features = [
      "defmt",
      "medium-ethernet",
      "multicast",
      "proto-dhcpv4",
      "proto-dns",
      "proto-ipv4",
      "socket-dns",
      "socket-icmp",
      "socket-raw",
      "socket-tcp",
      "socket-udp",
    ] }
    
    critical-section = "1.2.0"
    static_cell      = "2.1.1"
    
    
    # For fine tuning these settings, please refer to https://doc.rust-lang.org/cargo/reference/profiles.html
    [profile.dev]
    # The default debug profile is too slow and too big for resource-constrained devices.
    # Always build with some optimizations enabled.
    opt-level = 2
    
    [profile.release]
    codegen-units = 1     # LLVM can perform better optimizations using a single thread
    debug         = 2     # prefer slower builds but better debugging experience
    lto           = 'fat'
    opt-level     = 2
  )";
  testRoundTrip<toml::Parser, toml::Printer>(input, toml::Backend, json::Backend, xml::Backend);
}
