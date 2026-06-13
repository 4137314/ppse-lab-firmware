#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <Arduino.h>

typedef void (*TaskCallback)(void);

typedef struct {
    TaskCallback callback;
    uint32_t interval;
    uint32_t last_run;
} Task;

inline void run_task(Task* t) {
    uint32_t now = millis();
    if (now - t->last_run >= t->interval) {
        t->callback();
        t->last_run = now;
    }
}
#endif
