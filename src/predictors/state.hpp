#ifndef STATE_HPP
#define STATE_HPP

#include <string>

enum State {
        STRONG_NOT_TAKEN, // 00
        WEAK_NOT_TAKEN, // 01
        WEAK_TAKEN, // 10
        STRONG_TAKEN // 11
    };

std::string getStateName(State state) {
    switch (state) {
        case STRONG_NOT_TAKEN:
            return std::string("STRONG_NOT_TAKEN");
        case WEAK_NOT_TAKEN:
            return std::string("WEAK_NOT_TAKEN");
        case WEAK_TAKEN:
            return std::string("WEAK_TAKEN");
        case STRONG_TAKEN:
            return std::string("STRONG_TAKEN");
        default:
            return std::string();
    }
}


#endif /* STATE_HPP */