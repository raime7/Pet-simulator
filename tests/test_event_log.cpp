#include "../src/EventLog.h"

#include <cassert>
#include <sstream>
#include <string>
#include <vector>

int main() {
    EventLog emptyLog;
    assert(emptyLog.empty());
    assert(emptyLog.size() == 0);

    EventLog mochiLog;
    mochiLog.add("Fed Mochi.");
    mochiLog.add("Played with Mochi.");
    mochiLog.add("Mochi rested.");
    assert(mochiLog.size() == 3);

    const std::vector<std::string> mochiEvents = mochiLog.toVector();
    assert(mochiEvents.size() == 3);
    assert(mochiEvents[0] == "Mochi rested.");
    assert(mochiEvents[1] == "Played with Mochi.");
    assert(mochiEvents[2] == "Fed Mochi.");

    EventLog recentLog;
    recentLog.add("First");
    recentLog.add("Second");
    recentLog.add("Third");
    std::ostringstream recentOut;
    recentLog.printRecent(2, recentOut);
    assert(recentOut.str() == "1. Third\n2. Second\n");

    std::ostringstream zeroLimitOut;
    recentLog.printRecent(0, zeroLimitOut);
    assert(zeroLimitOut.str().empty());

    std::ostringstream negativeLimitOut;
    recentLog.printRecent(-1, negativeLimitOut);
    assert(negativeLimitOut.str().empty());

    std::ostringstream allRecentOut;
    recentLog.printRecent(10, allRecentOut);
    assert(allRecentOut.str() == "1. Third\n2. Second\n3. First\n");

    EventLog original;
    original.add("One");
    original.add("Two");
    EventLog copied(original);
    original.add("Three");
    const std::vector<std::string> copiedEvents = copied.toVector();
    assert(copiedEvents.size() == 2);
    assert(copiedEvents[0] == "Two");
    assert(copiedEvents[1] == "One");

    EventLog first;
    first.add("Alpha");
    first.add("Beta");
    EventLog second;
    second.add("Old");
    second = first;
    first.clear();
    assert(first.empty());
    assert(first.size() == 0);
    const std::vector<std::string> secondEvents = second.toVector();
    assert(secondEvents.size() == 2);
    assert(secondEvents[0] == "Beta");
    assert(secondEvents[1] == "Alpha");

    EventLog selfAssigned;
    selfAssigned.add("Alpha");
    selfAssigned.add("Beta");
    selfAssigned = selfAssigned;
    const std::vector<std::string> selfAssignedEvents = selfAssigned.toVector();
    assert(selfAssigned.size() == 2);
    assert(selfAssignedEvents.size() == 2);
    assert(selfAssignedEvents[0] == "Beta");
    assert(selfAssignedEvents[1] == "Alpha");

    EventLog swapLeft;
    swapLeft.add("Left old");
    swapLeft.add("Left new");
    EventLog swapRight;
    swapRight.add("Right only");
    swapLeft.swap(swapRight);
    const std::vector<std::string> swappedLeftEvents = swapLeft.toVector();
    const std::vector<std::string> swappedRightEvents = swapRight.toVector();
    assert(swapLeft.size() == 1);
    assert(swappedLeftEvents.size() == 1);
    assert(swappedLeftEvents[0] == "Right only");
    assert(swapRight.size() == 2);
    assert(swappedRightEvents.size() == 2);
    assert(swappedRightEvents[0] == "Left new");
    assert(swappedRightEvents[1] == "Left old");

    EventLog printEmpty;
    std::ostringstream emptyOut;
    printEmpty.printRecent(2, emptyOut);
    assert(emptyOut.str() == "No events.\n");

    return 0;
}
