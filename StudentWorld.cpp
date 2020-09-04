#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <cstdlib>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

//Destructor for this class calls cleanUp which will delete allocated data
StudentWorld::~StudentWorld() {
    cleanUp();//call clean up to delete all dynamically a
}

int StudentWorld::init()
{
    double x, y;
    m_so = new Socrates(IID_PLAYER, 0, VIEW_HEIGHT / 2, 0, 0, 1, this, 100);
    int numDirt = max(180 - 20 * getLevel(), 20);
    int numFood = min(5*getLevel(), 25);
     //Generate amount of Pits
    for (int i = 0; i < getLevel(); i++) {
        randomXY(x, y); //get random location for pit, making sure it doesn't overlap
        while (overLap(x, y)) {
            randomXY(x, y);
        }
        //making pit and adding it to actors
        Actor* pit = new Pit(IID_PIT, x, y, 0, 1, 1, this);
        actors.push_back(pit);
    }
    //Generating Food
    for (int i = 0; i < numFood; i++) {
        //finding it a random location and adding a food 
        //object there
        randomXY(x, y);
        while (overLap(x, y)) {
            randomXY(x, y);
        }
        Actor* food = new Food(IID_FOOD, x, y, 90, 1, 1, this);
        actors.push_back(food);
    }

    //Generating Dirt
    for (int i = 0; i < numDirt; i++) {
        randomXY(x, y);
        while (overLap(x, y)) {
            randomXY(x, y);
        }
        Actor* food = new Dirt(IID_DIRT, x, y, 0, 1, 1, this);
        actors.push_back(food);
    }

    return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::overLap(double x, double y) {//not meant for actors in the vector
    int size = actors.size();
    for (int i = 0; i < size; i++) {
        if (distance(x, actors[i]->getX(), y, actors[i]->getY()) <= SPRITE_WIDTH) {
            if (!actors[i]->blockable()) {
                return true; //there is potentially overlap
            }
        }
    }

    return false;
}

//targetAngle returns the angle between an actor and socrates
double StudentWorld::targetAngle(Actor* a) {
    double angle = (atan2(m_so->getY() - a->getY(), m_so->getX() - a->getX()))* (180 / 3.14159);
    if (angle < 0)
    {
        angle += 360;
 
    }
    return angle;
}

//returns the distance between socrates and the passed in actor
double StudentWorld::socratesDistance(Actor* a) {
    return distance(m_so->getX(), a->getX(), m_so->getY(), a->getY());
}

//Finds random XY coordinates that are around the perimeter
void StudentWorld::randomXY(double& x, double& y) {
    x = randInt(0, VIEW_WIDTH); //find a random x and y
    y = randInt(0, VIEW_HEIGHT);
    while (distance(VIEW_WIDTH / 2, x, VIEW_HEIGHT / 2, y) >= 120) { //keep looking for randomvalues as long as
        x = randInt(0, VIEW_WIDTH);                                 //long as they are out of bounds
        y = randInt(0, VIEW_HEIGHT);
    }
}
int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    m_so->doSomething();            //give socrates chance do to something
    int size = actors.size();
    for (int i = 0; i < size; i++) //do something for every actor
    {
        actors[i]->doSomething();
        if (m_so->getAlive() == false) { //if something causes
            this->decLives();            //socrates to die
            return GWSTATUS_PLAYER_DIED;
        }
        if (levelFinished()) {//if level has been completed return finished level
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }

    vector<Actor*>::iterator it;
    //delete dead actors
    for (it = actors.begin(); it != actors.end();) { 
        if ((*it)->getAlive() == false) {
            delete* it;
            it = actors.erase(it);
        }
        else {
            it++;
        }
    }
    //adding new goodies
    int chanceFungus = max(510 - getLevel() * 10, 200); //chance if fungus will be added
    int randomFun = randInt(0, chanceFungus - 1); //chance if goodie will be added
    if (randomFun == 0) { //if randomFun is zero, add a fungus somewhere on the perimeter
        double x, y;
        moveRandom(x, y);
        Actor* fungus = new Fungus(IID_FUNGUS, x, y, 0, 1, 1, this);
        actors.push_back(fungus); //put new goodie in the list
    }

    //int randGood = 45;
    int chanceGoodie = max(510 - getLevel() * 10, 250); 
    int randGood = randInt(0, chanceGoodie - 1);
    if (randGood == 0) {//if randGood is 0, spawn a goodie
        int whichGoodie = randInt(1,10); //will give random number between 0-9
        Actor* goodie;
        double x, y; 
        moveRandom(x, y); //where on the perimeter the goodie will be
        if (whichGoodie == 1) {//add a extra-life
            goodie = new ExtraLifeGoodie(IID_EXTRA_LIFE_GOODIE,x,
                y, 0, 1, 1, this);
        }
        else if (whichGoodie > 1 && whichGoodie <= 4) { //add a flame thrower
            goodie = new FlameThrowerGoodie(IID_FLAME_THROWER_GOODIE, x,
               y, 0, 1, 1, this);
        }
        else {//add a restore health
            goodie = new RestoreHealthGoodie(IID_RESTORE_HEALTH_GOODIE, x,
                y, 0, 1, 1, this);
        }
        actors.push_back(goodie); //push the new goodie in the lsit
    }

    //updating game statistics
    //to_string brings an int to a string
    setGameStatText("Score: " + calculateScore(this->getScore()) + "  Level: " + to_string(int(this->getLevel()))
    + "  Lives: " + to_string(int(this->getLives())) + "  Health: " + 
    to_string(int(m_so->getHitPoints())) + "  Sprays: " + to_string(int(m_so->getSprayCharges())) + "  Flames: " + to_string(int(m_so->getFlameThrowerCharges())));
    
    if (m_so->getAlive() == false) { //if player had died dec the lives and return died value
        this->decLives();
        return GWSTATUS_PLAYER_DIED;
    }
 
    return GWSTATUS_CONTINUE_GAME;
}

//levelFinished makes sure there are not actors that can spawn eneimes or enemies left
bool StudentWorld::levelFinished() {
    int size = actors.size();
    for (int i = 0; i < size; i++) {
        //if an actor that is not edible, not blockable, and not hitable is present level not finished
        if (!actors[i]->blockable() && !actors[i]->edible() && !actors[i]->hitable() && actors[i]->getAlive())
            return false;
        //if an actor that has hitpoints is still in the actors vector return false
        if (actors[i]->hasHitpoints())
            return false;
    }
    return true; //level is over
}

//collisions is used by projectiles in order to inflict damage to actors that they overlap with
bool StudentWorld::collisions(Actor* a, int testDistance, int amount) {
    int size = actors.size();
    for (int i = 0; i < size; i++) {
        if (actors[i]->hitable()) {//make sure it doesn't compare to itself, want to damage hitable actors
            if (distance(a->getX(), actors[i]->getX(), a->getY(), actors[i]->getY()) <= testDistance) {
                if (actors[i]->hitable()) {
                    actors[i]->damage(amount); //that actors damage function will be called, this can lower hitpoints by amount
                    return true;
                }
            }
        }
    }

    return false;
}

//Simply add actors the lsit
void StudentWorld::addToList(Actor* a) {
    actors.push_back(a);
}


//Deletes all the actors in the vector
void StudentWorld::cleanUp()
{
    delete m_so; //need to delete socrates as well
    for (unsigned int i = 0; i < actors.size(); i++) {
        delete actors[i];
    }
    actors.clear();
}

//This function is vital to ensure that the score is properly displaued
string StudentWorld::calculateScore(int score) {
    string scorestr = to_string(score); //turn score into a string
    if (score < 0) {//if score is less than zero make it zero
        increaseScore(0 - score);
        scorestr = "0"; //if the score is less than zero, change it to zero
    }
    //if score is above 6 digits, change it to max 6 digit value
    if (scorestr.size() > 6) {
        scorestr = "999999";
        return scorestr;
    }
    int leadingZeros = 6 - scorestr.size(); //calculate how many leading zeros the score should have
    for (int i = 0; i < leadingZeros; i++) //append that number of leading zeros to scorestr
        scorestr = "0" + scorestr;
    return scorestr;


}

//Move somewhere random around the perimeter of the petri dish
void StudentWorld::moveRandom(double &x, double &y) {
    //calculates X and Y value based upon a random angle
    x = (VIEW_WIDTH/2) + ((VIEW_WIDTH / 2) * cos(double(randInt(0, 359)) * 1.0 / 360 * 2 * (4 * atan(1))));
    y = (VIEW_HEIGHT/2) + ((VIEW_WIDTH / 2) * sin(double(randInt(0, 359)) * 1.0 / 360 * 2 * (4 * atan(1))));
    //recalculate if the random X,Y is greater than 120 pixels from center
    while (distance(x, VIEW_HEIGHT / 2, y, VIEW_WIDTH / 2) != double(VIEW_RADIUS)) {
        x = (VIEW_WIDTH / 2) + ((VIEW_WIDTH / 2) * cos(double(randInt(0, 359)) * 1.0 / 360 * 2 * (4 * atan(1))));
        y = (VIEW_HEIGHT / 2) + ((VIEW_HEIGHT / 2) * sin(double(randInt(0, 359)) * 1.0 / 360 * 2 * (4 * atan(1))));
    }
}

//Set socrates health to 100
void StudentWorld::setSocratesHealth() {
    m_so->setHitPoints(100); //set health points to full
}

//Increase socrates flame thrower charhes by 5
void StudentWorld::setSocratesFlameThrowerCharges() {
    m_so->setFlameThrowerCharges(5 + m_so->getFlameThrowerCharges());
}

//Check if an given actor overlaps with socrates
bool StudentWorld::socratesOverlap(Actor* a) {
    //calculate distance beween socrates and actor
    double dis = distance(m_so->getX(), a->getX(), m_so->getY(), a->getY());
    if (dis <= 2.0 * SPRITE_RADIUS)
        return true;
    return false;
}

//Checks if a given actor overlaps with an edible actor
bool StudentWorld::edibleOverlap(Actor* a) {
    //for every actor in the vector, check if it's edible
    //if it is, find distance between it and passed in actor
    //if it is overlaping kill that actor
    int size = actors.size();
    for (int i = 0; i < size; i++) {
        if ((actors[i]->edible() == true && actors[i]->getAlive() == true)) {
            double dis = distance(actors[i]->getX(), a->getX(), actors[i]->getY(), a->getY());
                if (dis <= SPRITE_WIDTH) {
                    actors[i]->setAlive(false);
                        return true;
                }
        }
    }
   
    return false;
}

//Check if (x,y) coordinate will be blocked by a blockable actor
bool StudentWorld::blockChecking(double x, double y) {
    //for every element in vector, if it is blockable
    //find distance between it and (x,y) and if it's overlaps
    //return that it does overlap(true_
    int size = actors.size();
    for (int i = 0; i < size; i++) { //check dirt
        if (actors[i]->blockable()) {
            double dis = distance(actors[i]->getX(), x, actors[i]->getY(), y);
            if (dis <= SPRITE_WIDTH/2){
                return true;
            }
        }
    }

    //checking bounds as well to make sure x and y are withing bounds of petri dish
    if (distance(x, VIEW_WIDTH / 2, y, VIEW_HEIGHT / 2) >= VIEW_RADIUS) {
        return true;
    }

    return false;
}



//Find closest edible actor in respect to a specifc actor, if edible actor if found x and y will have it's coordiantes
//otherwrise newx and newy will be -1
void StudentWorld::getCloseEdible(const double  x, const double  y, double & newX, double & newY) {
    //go through every actor in vector, if it is edible, check if its less than 128 pixels away
    //if so return that edible actors address, otherwise return nullptr
    int size = actors.size();
    for (int i = 0; i < size; i++) {
        if (actors[i]->edible() && actors[i]->getAlive() == true) {
            if (distance(actors[i]->getX(), x, actors[i]->getY(), y) < 128) {
                newX = actors[i]->getX();
                newY = actors[i]->getY();
                return;
            }
        }
    }
    newX = -1;
    newY = -1;
}

//decreaseSocratesHealth will decrease socrates health by a certain amount
void StudentWorld::decreaseSocratesHealth(int decreaseAmount) {
    m_so->damage(decreaseAmount);
}

//finds the distance beween to different coordinates 
double StudentWorld::distance(double x2, double x1, double y2, double y1) {
    return sqrt(pow(x2 - x1,2) + pow(y2 - y1,2));
}