#ifndef SYNTH_RANDOMGENERATOR_H
#define SYNTH_RANDOMGENERATOR_H

#include <random>

class RandomGenerator {
public:
    RandomGenerator();
    float RandFloat(int min, int max);
    float RandFloat(float min, float max);
private:
    std::default_random_engine engine;
};


#endif //SYNTH_RANDOMGENERATOR_H