#include "../src/Pet.h"

#include <cassert>
#include <string>

int main() {
    Dog mochi("Mochi");
    assert(mochi.getName() == "Mochi");
    assert(mochi.getType() == "Dog");
    assert(mochi.getHunger() == 30);
    assert(mochi.getHappiness() == 70);
    assert(mochi.getEnergy() == 70);
    assert(mochi.getCleanliness() == 70);
    assert(mochi.getHealth() == 100);
    assert(mochi.getAgeDays() == 0);
    assert(mochi.isAlive());

    mochi.feed();
    assert(mochi.getHunger() == 5);
    assert(mochi.getHappiness() == 73);

    mochi.play();
    assert(mochi.getHunger() == 13);
    assert(mochi.getHappiness() == 88);
    assert(mochi.getEnergy() == 58);
    assert(mochi.getCleanliness() == 65);

    assert(mochi.specialAction() == "Mochi went for a walk.");
    assert(mochi.getHunger() == 18);
    assert(mochi.getHappiness() == 98);
    assert(mochi.getEnergy() == 48);
    assert(mochi.getCleanliness() == 57);

    mochi.advanceDay();
    assert(mochi.getAgeDays() == 1);
    assert(mochi.getHunger() == 30);
    assert(mochi.getHappiness() == 91);
    assert(mochi.getEnergy() == 43);
    assert(mochi.getCleanliness() == 47);
    assert(mochi.getHealth() == 100);

    Cat luna("Luna");
    assert(luna.specialAction() == "Luna napped in the sun.");
    assert(luna.getHunger() == 33);
    assert(luna.getHappiness() == 78);
    assert(luna.getEnergy() == 85);

    Rabbit clover("Clover");
    assert(clover.specialAction() == "Clover hopped around happily.");
    assert(clover.getHunger() == 34);
    assert(clover.getHappiness() == 82);
    assert(clover.getEnergy() == 62);
    assert(clover.getCleanliness() == 67);

    Dog clampy("Clampy");
    for (int i = 0; i < 10; ++i) {
        clampy.feed();
    }
    assert(clampy.getHunger() == 0);
    for (int i = 0; i < 10; ++i) {
        clampy.rest();
    }
    assert(clampy.getEnergy() == 100);

    Dog rested("Rested");
    rested.rest();
    assert(rested.getHunger() == 35);
    assert(rested.getEnergy() == 95);

    Dog tidy("Tidy");
    tidy.clean();
    assert(tidy.getHappiness() == 67);
    assert(tidy.getCleanliness() == 100);

    Dog patient("Patient");
    patient.setState(30, 70, 70, 70, 60, 0);
    patient.visitDoctor();
    assert(patient.getHappiness() == 65);
    assert(patient.getHealth() == 80);

    Dog bounded("Bounded");
    bounded.setState(-10, 150, 101, -1, 250, -3);
    assert(bounded.getHunger() == 0);
    assert(bounded.getHappiness() == 100);
    assert(bounded.getEnergy() == 100);
    assert(bounded.getCleanliness() == 0);
    assert(bounded.getHealth() == 100);
    assert(bounded.getAgeDays() == 0);

    Dog ghost("Ghost");
    ghost.setState(30, 70, 70, 70, 0, 0);
    assert(!ghost.isAlive());

    Dog fragile("Fragile");
    fragile.setState(90, 50, 10, 10, 100, 2);
    fragile.advanceDay();
    assert(fragile.getAgeDays() == 3);
    assert(fragile.getHunger() == 100);
    assert(fragile.getHealth() == 74);

    Cat pixel("Pixel");
    assert(pixel.getType() == "Cat");
    const std::string status = pixel.statusLine();
    assert(status.find("Pixel") != std::string::npos);
    assert(status.find("Cat") != std::string::npos);
    assert(status.find("Hunger: 30") != std::string::npos);

    Rabbit thumper("Thumper");
    assert(thumper.getType() == "Rabbit");

    return 0;
}
