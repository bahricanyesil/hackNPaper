#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <string>
#include "Hacker.h"
#include "Desk.h"

using namespace std;

// Struct to store the events according to their types and time
struct Event {
    float timestamp;
    string type;
    int lineChanges;
    int hackerId;
    int desk;
    bool operator>(const Event& other) const
    {
        if(abs(timestamp - other.timestamp) < 0.00001) {
            return hackerId > other.hackerId;
        } else {
            return ((timestamp - other.timestamp) > 0.00001);
        }
    }
};

// Struct to store hackers with their IDs, commit numbers and timestamp in the hoodie queue
struct Hoodie {
    float timestamp;
    int hackerId;
    int commitNumber;
    bool operator>(const Hoodie& other) const
    {
        if(commitNumber == other.commitNumber) {
            if(abs(timestamp - other.timestamp) < 0.00001) {
                return hackerId > other.hackerId;
            } else {
                return ((timestamp - other.timestamp) > 0.00001);
            }
        } else {
            return commitNumber < other.commitNumber;
        }
    }
};

// Firstly, it looks all sticker desks and tries to find an available sticker desk
void findDeskSticker(string type, vector<Hacker>* hackers, vector<Desk>* desk, Event* event, queue<int>* queue, priority_queue<Event, vector<Event>, greater<Event>>* eventQueue) {
    for(int i=0; i<desk->size(); i++){
        // If it finds an available sticker desk, creates an event for the hacker to leave from the sticker desk after.
        if(desk->at(i).isAvailable) {
            hackers->at(event->hackerId-1).enteredStickerAt = event->timestamp;
            float time = event->timestamp + desk->at(i).serviceTime;
            Event newEvent = { time, type, -1, event->hackerId, i };
            eventQueue->push(newEvent);
            desk->at(i).isAvailable = false;
            return;
        }
    }
    // If it could not find an available sticker desk, it pushes the hacker to the sticker queue.
    queue->push(event->hackerId);
    hackers->at(event->hackerId-1).enteredStickerAt = event->timestamp;    
}

// Firstly, it looks all hoodie desks and tries to find an available hoodie desk
void findDeskHoodie(string type, vector<Hacker>* hackers, vector<Desk>* desk, Event* event, priority_queue<Hoodie, vector<Hoodie>, greater<Hoodie>>* hoodieQueue, priority_queue<Event, vector<Event>, greater<Event>>* eventQueue) {
    for(int i=0; i<desk->size(); i++){
        // If it finds an available hoodie desk, creates an event for the hacker to leave from the hoodie desk after.
        if(desk->at(i).isAvailable) {
            float time = event->timestamp + desk->at(i).serviceTime;
            hackers->at(event->hackerId-1).enteredHoodieAt = event->timestamp;
            Event newEvent = { time, type, -1, event->hackerId, i };
            eventQueue->push(newEvent);
            desk->at(i).isAvailable = false;
            return;
        }
    }
    // If it could not find an available hoodie desk, it pushes the hacker to the hoodie queue.
    int commitNumber = hackers->at(event->hackerId-1).validCommits;
    Hoodie newHoodie = { event->timestamp, event->hackerId, commitNumber };
    hoodieQueue->push(newHoodie);
    hackers->at(event->hackerId-1).enteredHoodieAt = event->timestamp;
}

// It takes a hacker from the sticker queue and put it to the sticker desk immediately when the other hacker leaves.
void changeInDeskSticker(string type, vector<Hacker>* hackers, Event* event, vector<Desk>* desk, queue<int>* queue, priority_queue<Event, vector<Event>, greater<Event>>* eventQueue) {
    float time = event->timestamp + desk->at(event->desk).serviceTime;
    int newHackerID;
    newHackerID = queue->front();
    queue->pop();
    hackers->at(newHackerID-1).totalWaitingSticker += (event->timestamp - hackers->at(newHackerID-1).enteredStickerAt);
    Event newEvent = { time, type, -1, newHackerID, event->desk};
    eventQueue->push(newEvent);
}

// It takes a hacker from the hoodie queue and put it to the hoodie desk immediately when the other hacker leaves.
void changeInDeskHoodie(string type, vector<Hacker>* hackers, Event* event, vector<Desk>* desk, priority_queue<Hoodie, vector<Hoodie>, greater<Hoodie>>* hoodieQueue, priority_queue<Event, vector<Event>, greater<Event>>* eventQueue) {
    float time = event->timestamp + desk->at(event->desk).serviceTime;
    int newHackerID; 
    newHackerID = hoodieQueue->top().hackerId;
    hoodieQueue->pop();
    hackers->at(newHackerID-1).totalWaitingHoodie += (event->timestamp - hackers->at(newHackerID-1).enteredHoodieAt);
    Event newEvent = { time, type, -1, newHackerID, event->desk};
    eventQueue->push(newEvent);
}

int main(int argc, char* argv[]) {
    ifstream infile;
    infile.open(argv[1]);
    ofstream outfile;
    outfile.open(argv[2]);
    outfile << fixed << setprecision(3);

    priority_queue<Event, vector<Event>, greater<Event>> eventQueue; 

    // Hacker vector to store all of the hackers and it pushes the creation of the hacker to the event queue.
    int numberOfHackers;
    infile >> numberOfHackers;
    vector<Hacker> hackers;
    hackers.reserve(numberOfHackers);
    float arrivalTime;
    for(int i=0; i<numberOfHackers; i++) {
        infile >> arrivalTime;
        Event event = { arrivalTime, "hacker", -1, -1, -1 };
        eventQueue.push(event);
    }

    // It pushes the commit event to the event queue.
    int numberOfCodeCommits;
    infile >> numberOfCodeCommits;
    int id;
    int lineChanges;
    float time;
    for(int i=0; i<numberOfCodeCommits; i++) {
        infile >> id >> lineChanges >> time;
        Event event = { time, "commit", lineChanges, id, -1 };
        eventQueue.push(event);
    }

    // It pushes the attempt event to the event queue.
    int numberOfAttempts;
    infile >> numberOfAttempts;
    int idOfHacker;
    float attemptSecond;
    for(int i=0; i<numberOfAttempts; i++) {
        infile >> idOfHacker >> attemptSecond;
        Event event = { attemptSecond, "attempt", -1, idOfHacker, -1 };
        eventQueue.push(event);
    }

    // Sticker desk vector to store all of the sticker desks
    int numberOfStickerDesks;
    infile >> numberOfStickerDesks;
    vector<Desk> stickerDesks;
    stickerDesks.reserve(numberOfStickerDesks);
    float serviceTimeOfDesk;
    for(int i=0; i<numberOfStickerDesks; i++) {
        infile >> serviceTimeOfDesk;
        stickerDesks.push_back(Desk(serviceTimeOfDesk, "sticker"));
    }

    // Hoodie desk vector to store all of the hoodie desks
    int numberOfHoodieDesks;
    infile >> numberOfHoodieDesks;
    vector<Desk> hoodieDesks;
    hoodieDesks.reserve(numberOfHoodieDesks);
    float serviceTimeOfHoodieDesk;
    for(int i=0; i<numberOfHoodieDesks; i++) {
        infile >> serviceTimeOfHoodieDesk;
        hoodieDesks.push_back(Desk(serviceTimeOfHoodieDesk, "hoodie"));
    }

    // Sticker queue to put the hackers when all of the sticker desks are full.
    queue<int> stickerQueue;
    // Hoodie queue to put the hackers when all of the hoodie desks are full.
    priority_queue<Hoodie, vector<Hoodie>, greater<Hoodie>> hoodieQueue; 
    int maxLengthOfSticker = 0;
    int maxLengthOfHoodie = 0;
    int totalInvalidGift = 0;
    int totalInvalidCommit = 0;
    float lastTimeStamp;
    float turnaroundTime = 0;
    while (eventQueue.empty() == false)
    {
        Event event = eventQueue.top();
        int hackerId = event.hackerId;
        // Hacker event type represents the arrival of a hacker.
        if(event.type == "hacker") {
            hackers.push_back(Hacker(event.timestamp));
        } else if(event.type == "commit") {
            // Commit event type represents the commit action of the hackers.
            hackers[hackerId-1].totalLineChanges += event.lineChanges;
            hackers[hackerId-1].totalCommits++;
            // If the line change is more than 20, it increments the number of valid commits.
            if(event.lineChanges >= 20) {
                hackers[hackerId-1].validCommits++;
            }
        } else if(event.type == "attempt") {
            // Attempt event type represents the attempt of the hacker to the sticker queue.
            // If the valid commit number is less than 3, invalid attempt due to commit.
            if(hackers[hackerId-1].validCommits < 3) {
                totalInvalidCommit++;
            } else if(hackers[hackerId-1].numberOfGifts > 2) {
                // If the number of gifts is more than 2, invalid attempt due to the number of gift.
                totalInvalidGift++;
            } else {
                // If the hacker fulfills all the requirements, tries to find a sticker desk for the hacker.
                findDeskSticker("leaveSticker", &hackers, &stickerDesks, &event, &stickerQueue, &eventQueue);
            }
        } else if(event.type == "leaveSticker") {
            // Leave sticker event type represents the quit of the hacker from the sticker desk.
            if(stickerQueue.empty()) {
                // If the sticker queue is empty, only set the desk to available, no one comes in.
                stickerDesks[event.desk].isAvailable = true;
            } else {
                // If the sticker queue is not empty, takes a hacker from there and put into the desk via this method.
                changeInDeskSticker("leaveSticker", &hackers, &event, &stickerDesks, &stickerQueue, &eventQueue);
            }
            // Tries to find an available hoodie desk for the hacker that leaves from the sticker queue.
            findDeskHoodie("leaveHoodie", &hackers, &hoodieDesks, &event, &hoodieQueue, &eventQueue);
        } else if(event.type == "leaveHoodie") {
            // Leave hoodie event type represents the quit of the hacker from the hoodie desk.
            // Increments the turnaround time since a hacker finishes the whole process.
            turnaroundTime += (event.timestamp - hackers[hackerId-1].enteredStickerAt);
            if(hoodieQueue.empty()) {
                // If the hoodie queue is empty, only set the desk to available, no one comes in.
                hoodieDesks[event.desk].isAvailable = true;
            } else {
                // If the hoodie queue is not empty, takes a hacker from there and put into the desk via this method.
                changeInDeskHoodie("leaveHoodie", &hackers, &event, &hoodieDesks, &hoodieQueue, &eventQueue);
            }
            // Since the hacker takes both sticker and hoodie, increments the number of total gifts for that user.
            hackers[hackerId-1].numberOfGifts++;
        }
        lastTimeStamp = event.timestamp;
        eventQueue.pop();
        // If the maxlength for sticker queue is less than current length of the sticker queue, sets it.
        if(maxLengthOfSticker < stickerQueue.size()) {
            maxLengthOfSticker = stickerQueue.size();
        }
        // If the maxlength for hoodie queue is less than current length of the hoodie queue, sets it.
        if(maxLengthOfHoodie < hoodieQueue.size()) {
            maxLengthOfHoodie = hoodieQueue.size();
        }
    }

    outfile << maxLengthOfSticker << endl;
    outfile << maxLengthOfHoodie << endl;

    int totalGifts = 0;
    int totalCommits = 0;
    int totalLineChanges = 0;
    float totalWaitingSticker = 0;
    float totalWaitingHoodie = 0;
    float maxWaiting = -1;
    int idOfMaxWaiting = -1;
    float minWaiting = -1;
    int idOfMinWaiting = -1;
    // A for loop to iterate over all hackers to find the total numbers that is required for the output.
    for(int m=0; m<numberOfHackers; m++) {
        totalGifts += hackers[m].numberOfGifts;
        totalCommits += hackers[m].totalCommits;
        totalLineChanges += hackers[m].totalLineChanges;
        
        totalWaitingSticker += hackers[m].totalWaitingSticker;
        totalWaitingHoodie += hackers[m].totalWaitingHoodie;
        float totalHackerWaiting = hackers[m].totalWaitingSticker + hackers[m].totalWaitingHoodie;

        // If the number of gifts is 3, controls the total waiting of the hacker and if it is less than the min waiting, sets it.
        if(hackers[m].numberOfGifts == 3){
            if(minWaiting < 0) {
                minWaiting = totalHackerWaiting;
                idOfMinWaiting = hackers[m].currentID; 
            } else {
                if((minWaiting - totalHackerWaiting) > 0.00001) {
                    minWaiting = totalHackerWaiting;
                    idOfMinWaiting = hackers[m].currentID;
                } else if(abs(totalHackerWaiting - minWaiting) < 0.00001 && hackers[m].currentID < idOfMinWaiting) {
                    idOfMinWaiting = hackers[m].currentID;
                }
            }
        }
        
        // If the total waiting time of the hacker is more than the maximum waiting time, sets it.
        if((totalHackerWaiting - maxWaiting) > 0.00001) {
            maxWaiting = totalHackerWaiting;
            idOfMaxWaiting = hackers[m].currentID;
        } else if(abs(totalHackerWaiting - maxWaiting) < 0.00001 && hackers[m].currentID < idOfMaxWaiting) {
            idOfMaxWaiting = hackers[m].currentID;
        }
    }
    
    outfile << ((float) totalGifts) / numberOfHackers << endl;
    outfile << ((float) totalWaitingSticker) / totalGifts << endl;
    outfile << ((float) totalWaitingHoodie) / totalGifts << endl;
    outfile << ((float) totalCommits) / numberOfHackers << endl;
    outfile << ((float) totalLineChanges) / totalCommits << endl;
    outfile << ((float) turnaroundTime) / totalGifts << endl;
    outfile << totalInvalidCommit << endl;
    outfile << totalInvalidGift << endl;
    outfile << idOfMaxWaiting << " " << maxWaiting << endl;
    outfile << idOfMinWaiting << " " << minWaiting << endl;
    outfile << lastTimeStamp;

    infile.close();
    outfile.close();
    return 0;
}