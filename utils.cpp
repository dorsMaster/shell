#include "includes.h"

/**
 * Using the delim is able to create tokens from a given input
 * @param input
 * @param delimiter
 */
vector<string> tokenizer(string input, string delimiter) {
    vector<string> tokens;
    size_t pos = 0;
    string token;
    while ((pos = input.find(delimiter)) != string::npos) {
        token = input.substr(0, pos);
        //TODO token size max len = 20 chars
        tokens.push_back(token);
        input.erase(0, pos + delimiter.length());
    }
    if (input.size() >= 2 && input.back() == '&') {
        input.pop_back();
        tokens.push_back(input);
        tokens.emplace_back("&");
    } else {
        tokens.push_back(input);
    }
    return tokens;
}

/**
 * finds the index of a given character in the vector used for figuring out the input/output file name
 * @param c
 * @param input
 */
int findIndex(char c, vector<string> input) {
    for (int i = 0; i < input.size(); i++) {
        if (input[i][0] == c) {
            return i;
        }
    }
    return -1;
}

/**
 * Restructures a command so exec() can understand it
 * @param cmdArgs
 * @param sIndex
 * @param eIndex
 */
string outputInterpreter(vector<string> cmdArgs, int sIndex, int eIndex) {
    string out;
    for (int i = ((sIndex == -1) ? 0 : sIndex); i < ((eIndex == -1) ? cmdArgs.size() : eIndex); i++) {
        out.append(cmdArgs[i]);
        out.append(DELIMITER);
    }
    out.pop_back();
    return out;
}

/**
 * Checks whether the task need to be run in the background or not
 * @param args
 */
bool isBackgroundCommand(vector<string> args) {
    return (args[args.size() - 1] == "&");
}

/**
 * Converts a time string into number of seconds
 * @param in
 */
int strToTime(string in){
    tm t;
    istringstream ss(in);
    ss >> get_time(&t, "%H:%M:%S");
//    https://stackoverflow.com/questions/24270236/how-to-convert-formatted-string-hhmmss-to-seconds-in-c
    return (t.tm_hour * 60 + t.tm_min);
}

/**
 * Uses a template to assign width to table for when it prints
 * https://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
 * @param t
 * @param width
 */
template<typename T> void printElement(T t, const int& width) {
    cout << left << setw(width) << setfill(separator) << t;
}

/**
 * Prints a single job using the function printElement()
 * @param cJob
 * @param i
 */
void printJob(Job cJob, int i) {
    printElement(i, numWidth);
    printElement(cJob.pid, nameWidth);
    printElement(cJob.status, numWidth);
    printElement(strToTime(cJob.dur), nameWidth);
    printElement(cJob.command, commandWidth);
}

/**
 * Checks to see whether the token is less than 20 characters long
 * @param tokens
 */
bool tokenLimiter(vector<string> tokens){
    bool res = true;
    for(int i=0; i<tokens.size();i++){
        if((tokens[i]).length()>20){
            res = false;
            break;
        }
    }
    return res;
}


