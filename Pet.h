#ifndef PET_H
#define PET_H

#include <string>

class Pet {
public:
    explicit Pet(const std::string& name);
    virtual ~Pet();

    const std::string& getName() const;
    int getHunger() const;
    int getHappiness() const;
    int getEnergy() const;
    int getCleanliness() const;
    int getHealth() const;
    int getAgeDays() const;

    void setState(int hunger, int happiness, int energy, int cleanliness, int health, int ageDays);
    void feed();
    void play();
    void rest();
    void clean();
    void visitDoctor();
    void advanceDay();
    bool isAlive() const;
    std::string statusLine() const;

    virtual std::string getType() const = 0;
    virtual std::string specialAction() = 0;

protected:
    void adjustStats(int hungerDelta, int happinessDelta, int energyDelta, int cleanlinessDelta, int healthDelta);

private:
    int clampStat(int value) const;

    std::string name_;
    int hunger_;
    int happiness_;
    int energy_;
    int cleanliness_;
    int health_;
    int ageDays_;
};

class Dog : public Pet {
public:
    explicit Dog(const std::string& name);
    std::string getType() const;
    std::string specialAction();
};

class Cat : public Pet {
public:
    explicit Cat(const std::string& name);
    std::string getType() const;
    std::string specialAction();
};

class Rabbit : public Pet {
public:
    explicit Rabbit(const std::string& name);
    std::string getType() const;
    std::string specialAction();
};

#endif
