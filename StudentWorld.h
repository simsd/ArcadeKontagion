#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "Actor.h"
#include "GameWorld.h"
#include <vector>
#include <string>

using namespace std;

class Actor;
class Socrates;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void addToList(Actor*); //adds an actor the vector of actors
    bool collisions(Actor* a, int dsitance, int amount); //will find collison between projectile and actor
    bool overLap(double x, double y);  
    bool socratesOverlap(Actor* a); //returns true if actor overlaps with scorates
    void setSocratesHealth(); //resets socrates health to the maximum value
    void decreaseSocratesHealth(int decreaseAmount); //decreases socrates health by the  indicated amount
    void setSocratesFlameThrowerCharges(); //increases the amount of flamethrower charges that socrates has
    bool edibleOverlap(Actor* a); //see if passed in actor overlaps with edible actor, sets overlapping edible actor to dead as well
    bool blockChecking(double x, double y); //check if moving to (x,y) position will block movement of actor
    void getCloseEdible(const double x, const double y, double& newX, double& newY); //x and y of closest edible actor
    double socratesDistance(Actor* a); //find the distance between an passed in actor and socrates
    double targetAngle(Actor* a); //find the angle an actor will have to move to reach socrates
private:
    string calculateScore(int score);  //function that calculates the score to display it for the user
    void moveRandom(double& x, double& y);  //find a random x and y coordinate in the playable world that an actor can inhabit
    void randomXY(double& x, double& y); //get random x and y on the perimeter
    double distance(double x2, double x1, double y2, double y1); //function finds distance between two x and y points
    bool levelFinished();  //checks if the level has been completed
    vector<Actor*> actors; //this vector holds every actor in the game but socrate
    Socrates* m_so; //the reference to the socrates character
};

#endif // STUDENTWORLD_H_
