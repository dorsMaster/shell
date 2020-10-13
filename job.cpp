#include <iomanip>
#include "includes.h"

class Job {
public:
    pid_t pid;
    string status = "";
    string dur = "";
    string command = "";

    Job(pid_t lpid, string lstat, string ldur, string lcmd) {
        pid = lpid;
        status = lstat;
        dur = ldur;
        command = lcmd;
    }

    Job(pid_t lpid, string lcmd) {
        pid = lpid;
        command = lcmd;
    }

    void printJob() {
        cout << pid << " | " << status << " | " << dur << " | " << command << endl;
    }
};
