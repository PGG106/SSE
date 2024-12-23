#include "shims.h"

void StartUciThread();
void RunMainThread();
void SpinLock(volatile bool* condition);