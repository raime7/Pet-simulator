#ifndef EVENT_LOG_H
#define EVENT_LOG_H

#include <iosfwd>
#include <string>
#include <vector>

struct EventNode {
    std::string message;
    EventNode* next;

    EventNode(const std::string& eventMessage, EventNode* nextNode);
};

class EventLog {
public:
    EventLog();
    EventLog(const EventLog& other);
    ~EventLog();

    EventLog& operator=(const EventLog& other);

    void add(const std::string& message);
    void clear();
    bool empty() const;
    int size() const;
    void swap(EventLog& other);
    std::vector<std::string> toVector() const;
    void printRecent(int limit, std::ostream& out) const;

private:
    void copyFrom(const EventLog& other);
    void deleteRecursive(EventNode* node);

    EventNode* head_;
    int size_;
};

#endif
