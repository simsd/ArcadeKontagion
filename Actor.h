#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
//GraphObject(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0)
class StudentWorld;

class Actor : public GraphObject //Actor class that every other class inherits from 
{
public:
	Actor( int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* world); //constructor to make an actor
	virtual void doSomething() = 0; //pure virtual function for doSomething, each derived class will need to do something 
	virtual ~Actor(); //destructor for actor
	bool getAlive(); //will return if the actor is alive or not
	void setAlive(bool x); //will set actor to be alive or dead
	StudentWorld* getWorld(); //get world pointer for each actor
	virtual bool hitable() = 0; //returns true if the actor can be damaged or hit with projectile
	virtual bool hasHitpoints() = 0; //returns true if the actor has hitpoints ex. socrates, bacteria
	virtual bool blockable() = 0; //if it can impede movement of bacteria then true
	virtual bool edible() = 0;  //if an actor can be consumed, set it to true. otherwise, return false
	virtual void damage(int x) = 0; //if actor can be damaged, this function will damage that actor by x
private:
	bool m_alive = true;   //boolean that represents if a actor is alive or not
	StudentWorld* m_sworld; //the studentworld that the actor is in
};
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Agent : public Actor { //class for agent, aka actors that have hitpoints
public:
	Agent(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw, int hitPoints);
	virtual ~Agent();
	bool hitable();
	bool hasHitpoints();
	bool blockable();
	int getHitPoints();			//returns that amount of hitpoints an actor has
	void setHitPoints(int x);	//sets the amount of hitpoints for an actor
	bool edible();
	virtual void damage(int x) = 0;
private:
	int m_hitpoints;           //amount of hitpoints aka. health an agent has
};

//Class declaration for Dirt class which directly inherents from Dirt
//this class doesn't really do anything except block bacteria movement, so 
//there is not anything special to state about it
class Dirt : public Actor
{
public:
	Dirt(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw);
	virtual ~Dirt();
	void doSomething();
	bool hitable();
	bool hasHitpoints();
	bool blockable();
	bool edible();
	void damage(int x);
private:
};

//Declaration for Pit class, it inherits from the Actor 
class Pit : public Actor {
public:
	virtual ~Pit();
	Pit(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw);
	void doSomething();
	bool hitable();
	bool hasHitpoints();
	bool blockable();
	bool edible();
	void damage(int x);
private:
	int m_numRegularSal;   //this is the number of regularSalomenlla bacteria this pit has spawned
	int m_numAggressiveSal;//this is the number of aggressiveSalmenola bacteria this pit has spawned
	int m_numEcoli;			//this is the number of Ecoli the pit has spawned
};

//Socrates class delcaration
//this class inherits from Agent so it has hitpoints
class Socrates : public Agent {
public:
	Socrates(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw, int hitPoints);
	void doSomething();
	virtual ~Socrates();
	double getFlameThrowerCharges();      //gets the amount of flame thrower charges
	void setFlameThrowerCharges(double x); //sets the amount of flame thrower charges
	double getSprayCharges();				//gets amount of spray charges 
	void setSprayCharges(double x);			//sets amount of spray charges
	void damage(int x);						
private:
	double m_sprayCharges;       //the number spray charges that Socrates has
	double m_flameThrowerCharges; //number of flame thrower charges that Socrates has
};

//Class declaration for Projectile, which is all the actors socrates can shoot
class Projectile : public Actor {
public:
	virtual ~Projectile();
	Projectile(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw);
	bool hitable();
	bool hasHitpoints();
	void doSomething();
	bool blockable();
	bool edible(); 
	void damage(int x);
	int getTravelDistance();
	void setTravelDistance(int increaseAmount);
	virtual void specialization() = 0;
private:
	int m_travelDistance = 0; //distance projectile has traveled
};

//Class declaration of disinfectantSpray projectile, it inherits directly from actor
class DisinfectantSpray : public Projectile {
public:
	virtual ~DisinfectantSpray();
	DisinfectantSpray(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw);
	void specialization();

private:
};

//Class declaration of Flame class it inherits directly from Actor
class Flame : public Projectile {
public:
	virtual ~Flame();
	Flame(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw);
	void specialization();
private:
};

//Goodie class that inherits from Actor and is based class for all three goodie types
class Goodie : public Actor {
public:
	virtual ~Goodie();
	Goodie(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw);
	void doSomething();
	bool hitable();
	bool hasHitpoints();
	int getLife();	//returns how many ticks the goodie has been alive
	void decLife(); //reduce m_life by one
	bool blockable();
	bool edible();
	void damage(int x);
	virtual void specialization() = 0; //since all the goodies have similar behavior, they share a doSomething
private:							   //all goodies implement specialization which does there specific behavior
	int m_life;                        //this is the lifespan of a goodie
};

//RestoreHealthGoodie class declaration
class RestoreHealthGoodie : public Goodie {
public:
	virtual ~RestoreHealthGoodie();
	RestoreHealthGoodie(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw);
	void specialization();
};
//FlameThrowerGoodie class declaration
class FlameThrowerGoodie : public Goodie {
public:
	virtual ~FlameThrowerGoodie();
	FlameThrowerGoodie(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw);
	void specialization();
};
//ExtraLifeGoodie class declaration 
class ExtraLifeGoodie : public Goodie {
public:
	virtual ~ExtraLifeGoodie();
	ExtraLifeGoodie(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw);
	void specialization();
};
//Fungus class declaration
class Fungus : public Goodie {
public:
	virtual ~Fungus();
	Fungus(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw);
	void specialization();
};
//Declaration for Food class, inherits directly from Actor
class Food : public Actor {
public:
	virtual ~Food();
	Food(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw);
	void doSomething();
	bool hitable();
	bool hasHitpoints();
	bool blockable();
	bool edible();
	void damage(int x);
private:

};

//Declaration for Bacterium class, this is an abstract class that inherits from Agent
class Bacterium : public Agent {
public:
	virtual ~Bacterium();
	Bacterium(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw, int hitPoints);
	virtual void doSomething() = 0;
	int getMovementPlanDistance();
	void setMovementPlantDistance(int x);
	int getFoodEaten();
	void setFoodEaten(int x);
	virtual void die() = 0;  //all bacteria need to implement a die function that will delete it 
	void damage(int x); //all bacteria need to implement their own damage function
	virtual void cry() = 0; //all bacteria need to implement a function that will play their proper sound
private:
	int m_movementPlanDistance;  //amount of ticks that bacterium can move
	int m_foodEaten;			 //amount of food that bacteria 
};

//Class declaration for RegularSalmonella 
class RegularSalmonella : public Bacterium {
private:
public:
	virtual ~RegularSalmonella();
	RegularSalmonella(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw, int hitPoints);
	virtual void doSomething();
	void cry(); //plays salmonella hurt or death sound
	void die();

};
//Class declaration for AggressiveSalmonella 
class AggressiveSalmonella : public RegularSalmonella {
public:
	virtual ~AggressiveSalmonella();
	AggressiveSalmonella(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw, int hitPoints);
	void doSomething();
};
//Class declaration for Ecoli
class Ecoli : public Bacterium {
public:
	virtual ~Ecoli();
	Ecoli(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw, int hitPoints);
	void cry(); //plays ecoli hurt or death sound
	void doSomething();
	void die();
};
#endif // ACTOR_H_
