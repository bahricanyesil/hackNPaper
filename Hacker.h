using namespace std;

#ifndef HACKER_H
#define HACKER_H


class Hacker {
private: 
    static int ID;

public:
    int currentID;
    float arrivalTime;
    int numberOfGifts = 0;
    float enteredStickerAt = 0;
    float enteredHoodieAt = 0;
    float totalWaitingSticker = 0;
    float totalWaitingHoodie = 0;
    int totalLineChanges = 0;
    int totalCommits = 0;
    int validCommits = 0;

    Hacker(float _arrivalTime);
    Hacker(const Hacker& hacker);
    ~Hacker();
};


#endif //HACKER_H