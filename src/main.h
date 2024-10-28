#include <string>
#include <unordered_map>
#define APP_VERSION "1.2.0"

#include "RtMidi.h"
#include <iostream>
#include <pwd.h>
#include <signal.h>
#include <thread>
#include <toml++/toml.hpp>

#ifdef __unix__

#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>

#endif

#ifdef __APPLE__

#include <ApplicationServices/ApplicationServices.h>

#endif

static void finish(int ignore);
int listMidiIO();
int listenToMidiPort(int port);

#ifdef __unix__

void emit(int fd, int type, int code, int val);
int keyPress(int fd, std::vector<int> keyCodes);
int listenAndMap(std::string configLocation);

#endif

#ifdef __APPLE__

void keyPressDown(int key);
void keyPressUp(int key);
void keyPress(std::vector<int> keys);

int listenAndMap(std::string configLocation);

#endif

void helpMessage();

std::string GetEnv(const std::string &var);
