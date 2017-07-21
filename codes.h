#ifndef CODES_H__
#define CODES_H__

#define MAX_NAME 20
#define MAX_PIN 6
#define MAX_ENT 20
struct {
    char name[MAX_NAME];
    int pin[MAX_PIN];
    size_t pin_n;
} pinEntries[MAX_ENT];

size_t pinEntries_n;

extern void loadDb();
extern size_t codecmp(const int pin[MAX_PIN], size_t pin_n);

#endif
