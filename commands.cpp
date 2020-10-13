#include "includes.h"

enum Commands {
    JOBS,
    KILL,
    RESUME,
    SUSPEND,
    WAIT,
    NONE
};

/**
 * returns the enum of a string from the map
 * @param input
 */
Commands getEnumCommand(string input) {
    static const map<string, Commands> optionStrings{
            {"jobs",    JOBS},
            {"kill",    KILL},
            {"resume",  RESUME},
            {"suspend", SUSPEND},
            {"wait",    WAIT},
            {"none",    NONE}};

    auto itr = optionStrings.find(input);
    if (itr != optionStrings.end()) {
        return itr->second;
    }
    return NONE;
}
