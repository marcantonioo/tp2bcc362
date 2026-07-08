#include "LogEntrySerializer.hpp"
#include <string>

std::vector<char> LogEntrySerializer::serialize(LogEntry& entry)
{
    std::vector<char> buffer;

    int term = entry.getTerm();

    buffer.insert(buffer.end(), (char*)&term, (char*)&term + sizeof(term));

    ClientCommand cmd = entry.getOperation();

    int clientID = cmd.getClientID();
    Operation op = cmd.getOperation();

    std::string key = cmd.getKey();
    std::string value = cmd.getValue();

    int keySize = key.size();
    int valueSize = value.size();

    buffer.insert(buffer.end(), (char*)&clientID, (char*)&clientID + sizeof(clientID));
    buffer.insert(buffer.end(), (char*)&op, (char*)&op + sizeof(op));

    buffer.insert(buffer.end(), (char*)&keySize, (char*)&keySize + sizeof(keySize));
    buffer.insert(buffer.end(), key.begin(), key.end());

    buffer.insert(buffer.end(), (char*)&valueSize, (char*)&valueSize + sizeof(valueSize));
    buffer.insert(buffer.end(), value.begin(), value.end());

    return buffer;
}

LogEntry LogEntrySerializer::deserialize(std::vector<char>& buffer)
{
    const char* ptr = buffer.data();

    int term;
    memcpy(&term, ptr, sizeof(term));
    ptr += sizeof(term);

    int clientID;
    memcpy(&clientID, ptr, sizeof(clientID));
    ptr += sizeof(clientID);

    Operation op;
    memcpy(&op, ptr, sizeof(op));
    ptr += sizeof(op);

    int keySize;
    memcpy(&keySize, ptr, sizeof(keySize));
    ptr += sizeof(keySize);

    std::string key(ptr, keySize);
    ptr += keySize;

    int valueSize;
    memcpy(&valueSize, ptr, sizeof(valueSize));
    ptr += sizeof(valueSize);

    std::string value(ptr, valueSize);

    ClientCommand cmd(clientID, op, key, value);

    return LogEntry(cmd, term);
}