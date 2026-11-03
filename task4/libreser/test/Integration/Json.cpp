#include "IntegrationTestUtil.hpp"
#include "LibReser/Json/Backend.hpp"
#include "LibReser/Json/Parser.hpp"
#include "LibReser/Json/Printer.hpp"
#include "LibReser/Toml/Backend.hpp"
#include "LibReser/Xml/Backend.hpp"
#include "gtest/gtest.h"

using namespace reser;

TEST(Integration, JsonXmlTomlRoundTrip) {
  static const std::string_view input = R"(
    {
      "log": {
        "loglevel": "info"
      },
      "api": {
        "services": [
          "HandlerService",
          "LoggerService",
          "StatsService"
        ],
        "tag": "api"
      },
      "stats": {},
      "policy": {
        "levels": {
          "0": {
            "statsUserUplink": true,
            "statsUserDownlink": true
          }
        },
        "system": {
          "statsInboundUplink": true,
          "statsInboundDownlink": true,
          "statsOutboundUplink": true,
          "statsOutboundDownlink": true
        }
      },
      "inbounds": [
        {
          "listen": "127.0.0.1",
          "port": 62789,
          "protocol": "dokodemo-door",
          "settings": {
            "address": "127.0.0.1"
          },
          "tag": "api",
          "sniffing": null
        },
        {
          "tag": "Vless-TCP-XTLS",
          "port": 443, 
          "protocol": "vless",
          "settings": {
            "clients": [
              {
                "email": "general@vless-tcp-xtls", 
                "id": "90e4903e-66a4-45f7-abda-fd5d5ed7f797", 
                "flow": "xtls-rprx-vision",
                "level": 0
              }
            ],
            "decryption": "none",
            "fallbacks": [
              {
                "name": "trh2o.example.com",
                "alpn": "h2",
                "dest": "@trojan-h2"
              },
              {
                "name": "vlh2o.example.com",
                "alpn": "h2",
                "dest": "@vless-h2"
              },
              {
                "name": "vmh2o.example.com",
                "alpn": "h2",
                "dest": "@vmess-h2"
              },
              {
                "name": "ssh2o.example.com",
                "alpn": "h2",
                "dest": 4003
              },
              {
                "path": "/vlws",
                "dest": "@vless-ws",
                "xver": 2 
              },
              {
                "path": "/vmws",
                "dest": "@vmess-ws",
                "xver": 2
              },
              {
                "path": "/trojanws",
                "dest": "@trojan-ws",
                "xver": 2
              },
              {
                "path": "/ssws",
                "dest": 4001
              },
              {
                "path": "/vltc",
                "dest": "@vless-tcp",
                "xver": 2
              },
              {
                "path": "/vmtc",
                "dest": "@vmess-tcp",
                "xver": 2
              },
              {
                "path": "/sstc",
                "dest": 4002
              },
              {
                "alpn": "h2",
                "dest": "@trojan-tcp",
                "xver": 2
              },
              {
                "dest": "/dev/shm/h1.sock",
                "xver": 2
              }
            ]
          },
          "streamSettings": {
            "network": "tcp",
            "security": "tls",
            "tlsSettings": {
              "certificates": [
                {
                  "ocspStapling": 3600, 
                  "certificateFile": "/etc/ssl/example.com/domain.pem", 
                  "keyFile": "/etc/ssl/example.com/domain-key.pem" 
                },
                {
                  "ocspStapling": 3600,
                  "certificateFile": "/etc/ssl/behindcdn.com/domain.pem",
                  "keyFile": "/etc/ssl/behindcdn.com/domain-key.pem"
                }
              ],
              "minVersion": "1.2", 
              "cipherSuites": "TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256:TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256:TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384:TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384:TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256:TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256", 
              "alpn": [
                "h2", 
                "http/1.1" 
              ]
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        },
        {
          "listen": "@vless-ws",
          "protocol": "vless",
          "settings": {
            "clients": [
              {
                "email": "general@vless-ws", 
                "id": "90e4903e-66a4-45f7-abda-fd5d5ed7f797", 
                "level": 0
              }
            ],
            "decryption": "none"
          },
          "streamSettings": {
            "network": "ws",
            "security": "none",
            "wsSettings": {
              "acceptProxyProtocol": true,
              "path": "/vlws"
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        },
        {
          "listen": "@vmess-ws",
          "protocol": "vmess",
          "settings": {
            "clients": [
              {
                "email": "general@vmess-ws",
                "id": "90e4903e-66a4-45f7-abda-fd5d5ed7f797",
                "level": 0
              }
            ]
          },
          "streamSettings": {
            "network": "ws",
            "security": "none",
            "wsSettings": {
              "acceptProxyProtocol": true,
              "path": "/vmws"
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        },
        {
          "listen": "@trojan-ws", 
          "protocol": "trojan",
          "settings": {
            "clients": [
              {
                "email": "general@trojan-ws",
                "password": "desdemona99",
                "level": 0
              }
            ]
          },
          "streamSettings": {
            "network": "ws",
            "security": "none",
            "wsSettings": {
              "acceptProxyProtocol": true, 
              "path": "/trojanws" 
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        },
        {
          "tag": "shadowsocks-ws",
          "listen": "127.0.0.1",
          "port": 4001,
          "protocol": "shadowsocks",
          "settings": {
            "email": "general@shadowsocks-ws",
            "method": "chacha20-ietf-poly1305",
            "password": "desdemona99",
            "level": 0
          },
          "streamSettings": {
            "network": "ws",
            "security": "none",
            "wsSettings": {
              "path": "/ssws"
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        },
        {
          "listen": "@trojan-tcp", 
          "protocol": "trojan",
          "settings": {
            "clients": [
              {
                "email": "general@trojan-tcp",
                "password": "desdemona99",
                "level": 0
              }
            ],
            "fallbacks": [
              {
                "dest": "/dev/shm/h2c.sock",
                "xver": 2 
              }
            ]
          },
          "streamSettings": {
            "network": "tcp",
            "security": "none",
            "tcpSettings": {
              "acceptProxyProtocol": true 
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        },
        {
          "listen": "@vless-tcp",
          "protocol": "vless",
          "settings": {
            "clients": [
              {
                "email": "general@vless-tcp",
                "id": "90e4903e-66a4-45f7-abda-fd5d5ed7f797",
                "level": 0
              }
            ],
            "decryption": "none"
          },
          "streamSettings": {
            "network": "tcp",
            "security": "none",
            "tcpSettings": {
              "acceptProxyProtocol": true,
              "header": {
                "type": "http",
                "request": {
                  "path": [
                    "/vltc"
                  ]
                }
              }
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        },
        {
          "listen": "@vmess-tcp",
          "protocol": "vmess",
          "settings": {
            "clients": [
              {
                "email": "general@vmess-tcp",
                "id": "90e4903e-66a4-45f7-abda-fd5d5ed7f797",
                "level": 0
              }
            ]
          },
          "streamSettings": {
            "network": "tcp",
            "security": "none",
            "tcpSettings": {
              "acceptProxyProtocol": true,
              "header": {
                "type": "http",
                "request": {
                  "path": [
                    "/vmtc"
                  ] 
                }
              }
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        },
        {
          "tag": "shadowsocks-tcp",
          "listen": "127.0.0.1",
          "port": 4002,
          "protocol": "shadowsocks",
          "settings": {
            "method": "chacha20-ietf-poly1305",
            "password": "desdemona99",
            "email": "general@shadowsocks-tcp",
            "level": 0
          },
          "streamSettings": {
            "network": "tcp",
            "security": "none",
            "tcpSettings": {
              "header": {
                "type": "http",
                "request": {
                  "path": [
                    "/sstc"
                  ]
                }
              }
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        },
        {
          "tag": "trojan-grpc",
          "listen": "127.0.0.1", 
          "port": 3001,
          "protocol": "trojan",
          "settings": {
            "clients": [
              {
                "email": "general@trojan-grpc",
                "password": "desdemona99",
                "level": 0
              }
            ]
          },
          "streamSettings": {
            "network": "grpc",
            "security": "none",
            "grpcSettings": {
              "serviceName": "trgrpc"
            }
          }
        },
        {
          "tag": "vless-grpc",
          "listen": "127.0.0.1", 
          "port": 3002,
          "protocol": "vless",
          "settings": {
            "clients": [
              {
                "email": "general@vless-grpc",
                "id": "90e4903e-66a4-45f7-abda-fd5d5ed7f797",
                "level": 0
              }
            ],
            "decryption": "none"
          },
          "streamSettings": {
            "network": "grpc",
            "security": "none",
            "grpcSettings": {
              "serviceName": "vlgrpc"
            }
          }
        },
        {
          "tag": "vmess-grpc",
          "listen": "127.0.0.1", 
          "port": 3003,
          "protocol": "vmess",
          "settings": {
            "clients": [
              {
                "email": "general@vmess-grpc",
                "id": "90e4903e-66a4-45f7-abda-fd5d5ed7f797", 
                "level": 0
              }
            ]
          },
          "streamSettings": {
            "network": "grpc",
            "security": "none",
            "grpcSettings": {
              "serviceName": "vmgrpc"
            }
          }
        },
        {
          "listen": "127.0.0.1", 
          "port": 3004,
          "protocol": "shadowsocks",
          "settings": {
            "method": "chacha20-ietf-poly1305",
            "password": "desdemona99", 
            "email": "2011@gmail.com"
          },
          "streamSettings": {
            "network": "grpc",
            "security": "none",
            "grpcSettings": {
              "serviceName": "ssgrpc" 
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        },
        {
          "listen": "@trojan-h2",
          "protocol": "trojan",
          "settings": {
            "clients": [
              {
                "email": "general@trojan-h2",
                "password": "desdemona99",
                "level": 0
              }
            ]
          },
          "streamSettings": {
            "network": "h2",
            "security": "none",
            "httpSettings": {
              "path": "/trh2"
            }
          }
        },
        {
          "listen": "@vless-h2",
          "protocol": "vless",
          "settings": {
            "clients": [
              {
                "email": "general@vless-h2", 
                "id": "90e4903e-66a4-45f7-abda-fd5d5ed7f797", 
                "level": 0
              }
            ],
            "decryption": "none"
          },
          "streamSettings": {
            "network": "h2",
            "security": "none",
            "httpSettings": {
              "path": "/vlh2"
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        },
        {
          "listen": "@vmess-h2",
          "protocol": "vmess",
          "settings": {
            "clients": [
              {
                "email": "general@vmess-h2",
                "id": "90e4903e-66a4-45f7-abda-fd5d5ed7f797",
                "level": 0
              }
            ]
          },
          "streamSettings": {
            "network": "h2",
            "security": "none",
            "httpSettings": {
              "path": "/vmh2"
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        },
        {
          "tag": "shadowsocks-h2",
          "listen": "127.0.0.1",
          "port": 4003,
          "protocol": "shadowsocks",
          "settings": {
            "method": "chacha20-ietf-poly1305",
            "password": "desdemona99", 
            "email": "general@shadowsocks-h2",
            "level": 0
          },
          "streamSettings": {
            "network": "h2",
            "security": "none",
            "httpSettings": {
              "path": "/ssh2"
            }
          },
          "sniffing": {
            "enabled": true,
            "destOverride": [
              "http",
              "tls"
            ]
          }
        } 
      ],
      "outbounds": [
        {
          "protocol": "freedom",
          "settings": {}
        },
        {
          "protocol": "blackhole",
          "tag": "blocked"
        },
        {
          "tag": "DNS-Internal",
          "protocol": "dns",
          "settings": {
            "address": "127.0.0.53",
            "port": 53
          }
        }
      ],
      "routing": {
        "domainStrategy": "AsIs",
        "rules": [
          {
            "inboundTag": [
              "api"
            ],
            "outboundTag": "api"
          },
          {
            "outboundTag": "blocked",
            "ip": [
              "geoip:private"
            ]
          },
          {
            "outboundTag": "blocked", 
            "protocol": [
              "bittorrent"
            ]
          }
        ]
      }
    }
  )";
  testRoundTrip<json::Parser, json::Printer>(input, json::Backend, xml::Backend, toml::Backend);
}
