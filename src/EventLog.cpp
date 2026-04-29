#include "EventLog.h"

#include <ostream>
#include <utility>

EventNode::EventNode(const std::string& eventMessage, EventNode* nextNode)
    : message(eventMessage), next(nextNode) {
}

EventLog::EventLog()
    : head_(0), size_(0) {
}

EventLog::EventLog(const EventLog& other)
    : head_(0), size_(0) {
    copyFrom(other);
}

EventLog::~EventLog() {
    clear();
}

EventLog& EventLog::operator=(const EventLog& other) {
    if (this != &other) {
        clear();
        copyFrom(other);
    }

    return *this;
}

void EventLog::add(const std::string& message) {
    head_ = new EventNode(message, head_);
    ++size_;
}

void EventLog::clear() {
    deleteRecursive(head_);
    head_ = 0;
    size_ = 0;
}

bool EventLog::empty() const {
    return head_ == 0;
}

int EventLog::size() const {
    return size_;
}

void EventLog::swap(EventLog& other) {
    std::swap(head_, other.head_);
    std::swap(size_, other.size_);
}

std::vector<std::string> EventLog::toVector() const {
    std::vector<std::string> events;

    for (EventNode* current = head_; current != 0; current = current->next) {
        events.push_back(current->message);
    }

    return events;
}

void EventLog::printRecent(int limit, std::ostream& out) const {
    if (empty()) {
        out << "No events.\n";
        return;
    }

    int index = 1;
    for (EventNode* current = head_; current != 0 && index <= limit; current = current->next) {
        out << index << ". " << current->message << '\n';
        ++index;
    }
}

void EventLog::copyFrom(const EventLog& other) {
    std::vector<std::string> events = other.toVector();

    for (std::vector<std::string>::reverse_iterator it = events.rbegin(); it != events.rend(); ++it) {
        add(*it);
    }
}

void EventLog::deleteRecursive(EventNode* node) {
    if (node == 0) {
        return;
    }

    deleteRecursive(node->next);
    delete node;
}
