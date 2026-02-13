#include "vulnerable_helper.h"
#include <cstring>
#include <cstdio>

namespace VulnerableHelper {
    // BUG-022 [Buffer overflow] strcpy, no bounds check
    void copyToBuffer(const std::string& input, char* out) {
        strcpy(out, input.c_str());
    }

    // BUG-023 [Buffer overflow] sprintf, no length limit
    void formatGreeting(const char* name, char* out) {
        sprintf(out, "Hello, %s!", name);
    }
}
