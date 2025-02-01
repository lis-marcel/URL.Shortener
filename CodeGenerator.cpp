// CodeGenerator.cpp
#include "CodeGenerator.h"

const std::string CodeGenerator::Chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

CodeGenerator::CodeGenerator()
    : generator(std::random_device{}()),
      distribution(0, static_cast<int>(Chars.size() - 1))
{
    // Constructor implementation (if needed)
}

std::string CodeGenerator::generateShortCode()
{
    std::string shortCode;
    for (int i = 0; i < 7; ++i) {
        shortCode += Chars[distribution(generator)];
    }
    return shortCode;
}
