//
// Created by Dorsa Nahid on 9/19/20.
//

#ifndef DORSA_INCLUDES_H
#define DORSA_INCLUDES_H

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <ctime>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <map>
#include <fcntl.h>
#include <regex>

using namespace std;
static const string DELIMITER = " ";
static const int PATH_MAX = 1024;
const int nameWidth     = 7;
const int numWidth     = 4;
const int commandWidth = 100;
const char separator    = ' ';

#endif
