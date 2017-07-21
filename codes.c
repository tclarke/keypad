#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "codes.h"

void loadDb()
{
    FILE* pFile;
    char* pLine;
    char* pDelim;
    size_t cur = 0;

    if ((pFile = fopen("/etc/keypad/codes", "r")) == NULL)
    {
        perror("Open PIN file");
        exit(2);
    }
    while (feof(pFile) == 0 && cur < MAX_ENT)
    {
        static int sz = MAX_NAME;
        if (getline(&pLine, &sz, pFile) == -1)
        {
            if (feof(pFile))
            {
                break;
            }
            perror("Reading PIN file");
            exit(2);
        }
        if ((pDelim = strtok(pLine, ":")) != NULL)
        {
            strncpy(pinEntries[cur].name, pDelim, MAX_NAME);
        }
        if ((pDelim = strtok(NULL, ":")) != NULL)
        {
            char tmp[2] = {0,0};
            for (pinEntries[cur].pin_n = 0;
                 pinEntries[cur].pin_n < MAX_PIN && pDelim[pinEntries[cur].pin_n] != '\n';
                 pinEntries[cur].pin_n++)
            {
                tmp[0] = pDelim[pinEntries[cur].pin_n];
                pinEntries[cur].pin[pinEntries[cur].pin_n] = atoi(tmp);
            }
        }
        ++cur;
    }
    pinEntries_n = cur;
}

size_t codecmp(const int pin[MAX_PIN], size_t pin_n)
{
    for (size_t i = 0; i < pinEntries_n; ++i)
    {
        int cnt = pinEntries[i].pin_n;
        if (cnt != pin_n)
        {
            continue;
        }
        if (memcmp(pin, pinEntries[i].pin, cnt) == 0)
        {
            return i;
        }
    }
    return MAX_ENT + 1;
}
