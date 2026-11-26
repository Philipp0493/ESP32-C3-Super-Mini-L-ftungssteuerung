#pragma once
inline int fakeWdtResets=0,fakeWdtInit=0;
inline int esp_task_wdt_init(uint32_t,bool){fakeWdtInit++;return 0;}
inline int esp_task_wdt_add(void*){return 0;}
inline int esp_task_wdt_reset(){fakeWdtResets++;return 0;}
