#include "RandomGenerator.h"

RandomGenerator::RandomGenerator() : engine(time(nullptr)) {
}

float RandomGenerator::RandFloat(int min, int max) {
    std::uniform_real_distribution<> distribution(min, max);
    return distribution(engine);
}

float RandomGenerator::RandFloat(float min, float max) {
    std::uniform_real_distribution<> distribution(min, max);
    return distribution(engine);
}