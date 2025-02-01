// CodeGenerator.h
#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <string>
#include <random>

class CodeGenerator {
public:
    CodeGenerator();

    // Generates a unique short code
    std::string generateShortCode();

private:
    static const std::string Chars;

    std::mt19937 generator;
    std::uniform_int_distribution<> distribution;
};

#endif // CODEGENERATOR_H
