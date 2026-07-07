#ifndef APPENDENTRIESMESSAGE_H
#define APPENDENTRIESMESSAGE_H
#include "Log.h"
#include <vector>
class AppendEntriesMessage
{
    std::vector<Log> logs;
    int nodeID;
};
#endif