#include "includes.h"
#include "commands.cpp"
#include "job.cpp"
#include "utils.cpp"


vector<Job> jobs;

/**
 * Checks whether the PID given is currently in the tasks
 * @param cmdPID
 * @param ljobs
 */
bool hasPid(string cmdPID, vector<Job> ljobs) {
    for (int i = 0; i < ljobs.size(); i++) {
        if (ljobs[i].pid == stoi(cmdPID)) {
            return true;
        }
    }
    return false;
}

/**
 * Adds a job to the vector of jobs
 * @param job
 */
void addJob(Job job) {
    if (jobs.size() < 32) {
        jobs.push_back(job);
    } else {
        cout << "Max entries in the Process Table has been reached" << endl;
    }
}

/**
 * Updates the jobs vector to reflect their new status and time using the ps command
 */
void update() {
    vector<Job> oldJobs;
    copy(jobs.begin(), jobs.end(), back_inserter(oldJobs));
    jobs.clear();
    for (int i = 0; i < oldJobs.size(); i++) {
        pid_t pid = oldJobs[i].pid;
        FILE *fp;
        int status;
        char path[PATH_MAX];

        string statcmd = "ps -o stat -o etime ";
        statcmd.append(to_string((int) pid));
        const char *c_stat_cmd = statcmd.c_str();
        fp = popen(c_stat_cmd, "r");
        if (fp == NULL)
            /* Handle error */;
        vector<string> lines;
        while (fgets(path, PATH_MAX, fp) != NULL) {
            if (path != " " && path != "\n") {
                lines.push_back(path);
            }
        }

        if (lines.size() == 2) {
            string line = regex_replace(lines[1], regex("^ +| +$|( ) +"), "$1");
            vector<string> jobStat = tokenizer(line, " ");
            oldJobs[i].status = jobStat[0];
            jobStat[1].erase(std::remove(jobStat[1].begin(), jobStat[1].end(), '\n'), jobStat[1].end());
            oldJobs[i].dur = jobStat[1];

            Job job = Job(oldJobs[i].pid, oldJobs[i].status, oldJobs[i].dur, oldJobs[i].command);
            addJob(job);
        }


        status = pclose(fp);
        if (status == -1) {
            /* Error reported by pclose() */
        } else {
            /* Use macros described under wait() to inspect `status' in order
            to determine success/failure of command executed by popen() */
        }

    }

}

/**
 * prints the format for all the jobs
 */
void printJobs() {
    update();
    cout << "Running processes:" << endl;
    if (jobs.size() == 0) {
        cout << "Processes = ";
        printElement(jobs.size(), nameWidth);
        cout << " active" << endl;
    } else {
        printElement("#", numWidth);
        printElement("PID", nameWidth);
        printElement("S", numWidth);
        printElement("SEC", nameWidth);
        printElement("Command", commandWidth);
        cout << endl;
	int activeJobs = 0;
        for (int i = 0; i < jobs.size(); i++) {
            if (jobs[i].status[0]=='R'){
                activeJobs = activeJobs + 1;
            }
            printJob(jobs[i], i);
            cout << endl;
        }
        cout << "Processes = ";
        printElement(activeJobs, nameWidth);
        cout << " active" << endl;
    }

    struct rusage usage{};
    getrusage(RUSAGE_CHILDREN, &usage);
    cout << "Competed processes:" << endl;
    cout << "User time = ";
    printElement(usage.ru_utime.tv_sec, nameWidth);
    cout << " seconds" << endl;

    cout << "Sys  time = ";
    printElement(usage.ru_stime.tv_sec, nameWidth);
    cout << " seconds" << endl;
}

/**
 * Kill, resume and suspend
 * @param args
 * @param background
 * @param background
 */
int krs(vector<string> args, int signal, bool background) {
    if (args.size() >= 2) {
        if (hasPid(args[1], jobs)) {
            kill(stoi(args[1]), signal);
            if (background)
                wait(0);
            return 0;
        } else { cout << "Please enter a valid PID" << endl; }

    } else { cout << "Please enter a PID" << endl; }
}

/**
 * handles the input and calls upon different functions according to the user input
 * @param cmdArgs 
 * @param terminalInput
 */
int inputHandler(vector<string> cmdArgs, string terminalInput) {

    bool backgroundFlag = isBackgroundCommand(cmdArgs);
    int outIndex = findIndex('>', cmdArgs);
    int inIndex = findIndex('<', cmdArgs);
    if (cmdArgs.size() > 8 || cmdArgs.empty() || terminalInput.empty()) {
        return 0;
    }

    switch (getEnumCommand(cmdArgs[0])) {
        case JOBS:
            printJobs();
            break;
        case KILL:
            krs(cmdArgs, SIGKILL, backgroundFlag);
            break;
        case RESUME:
            krs(cmdArgs, SIGCONT, backgroundFlag);
            break;
        case SUSPEND:
            krs(cmdArgs, SIGSTOP, backgroundFlag);
            break;
        case WAIT:
            if (cmdArgs.size() >= 2 && hasPid(cmdArgs[1], jobs)) {
                int status;
                pid_t lpid;
                signal(SIGCHLD, SIG_DFL);
                while ((lpid = wait(&status)) > 0);
//                waitpid(stoi(cmdArgs[1]), &status, WNOHANG);
                return 0;
            } else { cout << "Please enter a PID" << endl; }
            break;
        case NONE:
        default:
            int fdOut;
            int fdIn;
            pid_t pid, wpid;
            int status;
            if ((pid = fork()) < 0) {
                fprintf(stderr, "Fork Failed");
                exit(-1);
            } else if (pid == 0) {
                if (outIndex != -1 || backgroundFlag) {
                    string fn;
                    if (outIndex == -1)
                        fn = "/dev/null";
                    else {
                        fn = cmdArgs[outIndex];
                        fn = fn.substr(1, fn.size());
                    }
                    fdOut = open(fn.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                    dup2(fdOut, 1);   // make stdout go to file
                    close(fdOut);     // fd no longer needed - the dup'ed handles are sufficient
                }
                if (inIndex != -1) {
                    string fn = cmdArgs[inIndex];
                    fn = fn.substr(1, fn.size());
                    fdIn = open(fn.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                    dup2(fdIn, 0);   // make stdin go to file
                    close(fdIn);     // fd no longer needed - the dup'ed handles are sufficient
                }

                string statcmd = outputInterpreter(cmdArgs, 0, ((inIndex == -1) ? outIndex : inIndex));
                vector<string> cmd = tokenizer(statcmd, DELIMITER);

                int cmdSize = ((backgroundFlag && outIndex == -1) ? cmd.size() - 1 : cmd.size());
                char *argv[cmdSize + 1];
                for (int i = 0; i < cmdSize; i++)
                    argv[i] = const_cast<char *>(cmd[i].c_str());
                argv[cmdSize] = (char *) nullptr;
                execvp(argv[0], argv);
                exit(1);
            } else {
                Job j = Job(pid, terminalInput);
                addJob(j);
                if ((backgroundFlag && outIndex != -1) || outIndex != -1) {
                    signal(SIGCHLD, SIG_IGN);
                    //Nothing!
                } else if (backgroundFlag) {
                    signal(SIGCHLD, SIG_IGN);
                    return 0;
                } else {
                    signal(SIGCHLD, SIG_DFL);
                    while ((wpid = wait(&status)) > 0);
//                    waitpid(pid, &status, WNOHANG);
//                    WEXITSTATUS(status);
                }
            }
            break;
    }
}

/**
 * exits the program and waits on the functions to finish running
 */
void exit() {
    for (auto &job : jobs) {
        int status;
        waitpid(job.pid, &status, WNOHANG);
    }
    struct rusage usage{};
    int res = getrusage(RUSAGE_CHILDREN, &usage);
    if (res == 0) {
        cout << "Resources used" << endl;
        cout << "User time = " << usage.ru_utime.tv_sec << endl;
        cout << "Sys  time = " << usage.ru_stime.tv_sec << endl;
    }
}

int main() {
    string cmd;
    while (cmd != "exit") {
        cout << "SHELL379: ";
        getline(cin, cmd);
        if (cmd.size() < 100) {
            if (cmd != "exit") {
                cmd = regex_replace(cmd, regex("^ +| +$|( ) +"), "$1");
                vector<string> cmdArgs = tokenizer(cmd, DELIMITER);
                if(tokenLimiter(cmdArgs))
                    inputHandler(cmdArgs, cmd);
                else
                    cout << "Max # of characters in a token reached!" << endl;
            }
        } else {
            cout << "Max # of characters in an input line has been reached!" << endl;
        }
    }
    exit();
    return 0;
}
