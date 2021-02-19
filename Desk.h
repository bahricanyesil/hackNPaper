#include <string>
using namespace std;

#ifndef DESK_H
#define DESK_H


class Desk {
private: 
    static int ID;

public:
	int currentID;
    float serviceTime;
    string type;
    bool isAvailable = true;

    Desk(float _serviceTime, string _type);
    Desk(const Desk& desk);
    ~Desk();
};


#endif //DESK_H