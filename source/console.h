#pragma once
#include <wut.h>

#ifdef __cplusplus
extern "C" {
#endif

void ConsoleInit();
void ConsoleFree();
BOOL ConsoleDrawStart();
void ConsoleDrawEnd();

#ifdef __cplusplus
}
#endif
