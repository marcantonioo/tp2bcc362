#include "ClientCommandSerializer.h"

#include <cstring>

std::vector<char> ClientCommandSerializer::serialize(const ClientCommand& cmd)
{
    std::vector<char> buffer;

    auto append = [&](const void* data, size_t size)
    {
        const char* ptr = static_cast<const char*>(data);
        buffer.insert(buffer.end(), ptr, ptr + size);
    };

    int clientID = cmd.getClientID();
    append(&clientID, sizeof(clientID));

    int operation = static_cast<int>(cmd.getOperation());
    append(&operation, sizeof(operation));

    int keySize = cmd.getKey().size();
    append(&keySize, sizeof(keySize));
    append(cmd.getKey().data(), keySize);

    int valueSize = cmd.getValue().size();
    append(&valueSize, sizeof(valueSize));
    append(cmd.getValue().data(), valueSize);

    return buffer;
}

ClientCommand ClientCommandSerializer::deserialize(const std::vector<char>& buffer)
{
    size_t offset = 0;

    auto read = [&](void* dest, size_t size)
    {
        std::memcpy(dest, buffer.data() + offset, size);
        offset += size;
    };

    int clientID;
    read(&clientID, sizeof(clientID));

    int operation;
    read(&operation, sizeof(operation));

    int keySize;
    read(&keySize, sizeof(keySize));

    std::string key(buffer.data() + offset, keySize);
    offset += keySize;

    int valueSize;
    read(&valueSize, sizeof(valueSize));

    std::string value(buffer.data() + offset, valueSize);
    offset += valueSize;

    return ClientCommand(
        clientID,
        static_cast<Operation>(operation),
        key,
        value
    );
}