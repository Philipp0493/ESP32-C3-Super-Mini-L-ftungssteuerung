#pragma once
// Minimaler Ersatz für die ESP-IDF-mDNS-Abfrage.
#define MDNS_TYPE_PTR 0x000C
#define ESP_IPADDR_TYPE_V4 0
struct esp_ip4_addr_t {uint32_t addr;};
struct esp_ip_addr_t {union {esp_ip4_addr_t ip4;} u_addr; uint8_t type;};
struct mdns_ip_addr_t {esp_ip_addr_t addr; mdns_ip_addr_t*next;};
struct mdns_txt_item_t {const char*key;const char*value;};
struct mdns_result_t {mdns_result_t*next;char*hostname;mdns_txt_item_t*txt;size_t txt_count;mdns_ip_addr_t*addr;};
struct mdns_search_once_t {int polls;};
inline mdns_result_t*fakeMdnsResults=nullptr;inline int fakeMdnsSearches=0,fakeMdnsPollsNeeded=1;
inline mdns_search_once_t fakeSearch;
inline mdns_search_once_t*mdns_query_async_new(const char*,const char*,const char*,uint16_t,uint32_t,size_t,void*){fakeMdnsSearches++;fakeSearch.polls=0;return &fakeSearch;}
inline bool mdns_query_async_get_results(mdns_search_once_t*s,uint32_t,mdns_result_t**r){if(++s->polls<fakeMdnsPollsNeeded)return false;*r=fakeMdnsResults;return true;}
inline void mdns_query_results_free(mdns_result_t*){}
inline void mdns_query_async_delete(mdns_search_once_t*){}
