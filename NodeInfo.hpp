#include <string>

class NodeInfo{
    std::string address;
    int id;
    int port;
public:
    NodeInfo(int id, int port, std::string address){
        this->address = address;
        this->id = id;
        this-> port = port;
    }
};