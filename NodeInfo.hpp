#ifndef NODEINFO_H
#define NODEINFO_H

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
    void setid(int ID){id = ID;}
    void setport(int Port){port = Port;}
    void setaddress(std::string addr){address = addr;}
    int getid(){return id;}
    int getport(){return port;}
    std::string getaddress(){return address;}

};

#endif