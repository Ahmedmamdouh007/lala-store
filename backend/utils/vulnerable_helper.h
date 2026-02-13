#ifndef VULNERABLE_HELPER_H
#define VULNERABLE_HELPER_H

#include <string>

// Simulated "beginner" helper - no bounds checking, fixed-size buffer.
// Used in lab routes to demonstrate buffer overflow vulnerabilities.
namespace VulnerableHelper {
    // Copies input into 32-byte buffer (no length check) - buffer overflow
    void copyToBuffer(const std::string& input, char* out);
    // Builds a "greeting" string into 40-byte buffer using sprintf - overflow possible
    void formatGreeting(const char* name, char* out);
}

#endif
