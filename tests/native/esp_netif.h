#pragma once
struct esp_netif_t {};
inline esp_netif_t fakeSta; inline std::string fakeNetifHostname;
inline esp_netif_t*esp_netif_get_handle_from_ifkey(const char*){return &fakeSta;}
inline int esp_netif_set_hostname(esp_netif_t*,const char*h){fakeNetifHostname=h;return 0;}
