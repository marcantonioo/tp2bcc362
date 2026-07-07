#include "LogEntry.h"
#include <vector>
class Log{
    int term;
    std::vector<LogEntry> logentry;
public:
    Log(int term, LogEntry message);
    int getTerm(){return term;}
}; 
