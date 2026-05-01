#include "Pet.h"

#include <sstream>

Pet::Pet(const std::string& name)
    : name_(name),
      hunger_(30),
      happiness_(70),
      energy_(70),
      cleanliness_(70),
      health_(100),
      ageDays_(0) {}

Pet::~Pet() {}

const std::string& Pet::getName() const {
    return name_;
}

int Pet::getHunger() const {
    return hunger_;
}

int Pet::getHappiness() const {
    return happiness_;
}

int Pet::getEnergy() const {
    return energy_;
}

int Pet::getCleanliness() const {
    return cleanliness_;
}

int Pet::getHealth() const {
    return health_;
}

int Pet::getAgeDays() const {
    return ageDays_;
}

void Pet::setState(int hunger, int happiness, int energy, int cleanliness, int health, int ageDays) {
    hunger_ = clampStat(hunger);
    happiness_ = clampStat(happiness);
    energy_ = clampStat(energy);
    cleanliness_ = clampStat(cleanliness);
    health_ = clampStat(health);
    ageDays_ = ageDays < 0 ? 0 : ageDays;
}

void Pet::feed() {
    adjustStats(-25, 3, 0, 0, 0);
}

void Pet::play() {
    adjustStats(8, 15, -12, -5, 0);
}

void Pet::rest() {
    adjustStats(5, 0, 25, 0, 0);
}

void Pet::clean() {
    adjustStats(0, -3, 0, 30, 0);
}

void Pet::visitDoctor() {
    adjustStats(0, -5, 0, 0, 20);
}

void Pet::advanceDay() {
    ++ageDays_;
    adjustStats(12, -7, -5, -10, 0);

    int healthLoss = 0;
    if (hunger_ > 75) {
        healthLoss += 10;
    }
    if (energy_ < 25) {
        healthLoss += 8;
    }
    if (cleanliness_ < 30) {
        healthLoss += 8;
    }

    adjustStats(0, 0, 0, 0, -healthLoss);
}

bool Pet::isAlive() const {
    return health_ > 0;
}

std::string Pet::statusLine() const {
    std::ostringstream output;
    output << name_ << " (" << getType() << ") "
           << "Hunger: " << hunger_
           << ", Happiness: " << happiness_
           << ", Energy: " << energy_
           << ", Cleanliness: " << cleanliness_
           << ", Health: " << health_
           << ", Age: " << ageDays_;
    return output.str();
}

void Pet::adjustStats(int hungerDelta, int happinessDelta, int energyDelta, int cleanlinessDelta, int healthDelta) {
    hunger_ = clampStat(hunger_ + hungerDelta);
    happiness_ = clampStat(happiness_ + happinessDelta);
    energy_ = clampStat(energy_ + energyDelta);
    cleanliness_ = clampStat(cleanliness_ + cleanlinessDelta);
    health_ = clampStat(health_ + healthDelta);
}

int Pet::clampStat(int value) const {
    if (value < 0) {
        return 0;
    }
    if (value > 100) {
        return 100;
    }
    return value;
}

Dog::Dog(const std::string& name) : Pet(name) {}

std::string Dog::getType() const {
    return "Dog";
}

std::string Dog::specialAction() {
    adjustStats(5, 10, -10, -8, 0);
    return getName() + " went for a walk.";
}

Cat::Cat(const std::string& name) : Pet(name) {}

std::string Cat::getType() const {
    return "Cat";
}

std::string Cat::specialAction() {
    adjustStats(3, 8, 15, 0, 0);
    return getName() + " napped in the sun.";
}

Rabbit::Rabbit(const std::string& name) : Pet(name) {}

std::string Rabbit::getType() const {
    return "Rabbit";
}

std::string Rabbit::specialAction() {
    adjustStats(4, 12, -8, -3, 0);
    return getName() + " hopped around happily.";
}
