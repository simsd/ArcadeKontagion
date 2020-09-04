#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
//Actor stuff
Actor::Actor(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* world):
	 GraphObject(ID, sX, sY, dir, depth, size), m_alive(true), m_sworld(world) {
}
bool Actor::getAlive() {
	return m_alive;
}
void Actor::setAlive(bool x) {
	m_alive = x;
}

StudentWorld* Actor::getWorld() {
	return m_sworld;
}

 Actor::~Actor() {

}

 //Agent
 Agent::Agent(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw, int hitPoints)
		: Actor(ID, sX, sY, dir, depth, size, sw){
	 m_hitpoints = hitPoints;
 }

 bool Agent::edible() {
	 return false;
 }

 Agent::~Agent() {}
 bool  Agent::hitable() { return true; }
 bool  Agent::hasHitpoints() { return true; }
 bool  Agent::blockable() { return false; }
 int  Agent::getHitPoints() { return m_hitpoints; }
 void  Agent::setHitPoints(int x) { m_hitpoints = x; };

//DIRT
Dirt::Dirt(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw)
		: Actor(ID, sX, sY, dir, depth, size, sw) {}
void Dirt::doSomething() {
	//doesnt do anything its dirt
}

Dirt::~Dirt() {

}

//Damage for a dir just kills it 
void Dirt::damage(int x) {
	x = 0;
	x++;
	this->setAlive(false);
}

//Dirt is not edible so false
bool Dirt::edible() {
	return false;
}
//Dirt is hitable so true
bool Dirt::hitable() {
	return true;
}

//Dirt does not have hitpoints
bool Dirt::hasHitpoints() {
	return false;
}

//Dirt is blockable
bool Dirt::blockable() {
	return true;
}

//PIT
//where the bacteria spawn from
//can not be damaged from sprays or flames, is not edible or blockable
Pit::~Pit() {}
Pit::Pit(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw)
	:Actor(ID, sX, sY, dir, depth, size, sw)
{	
	//start with no bacteria emitted
	m_numAggressiveSal = 0;
	m_numRegularSal = 0;
	m_numEcoli = 0;
}
void Pit::damage(int x) {
	x = 0;
	x++;
}
bool Pit::edible() {
	return false;
}

void Pit::doSomething() {
	//check if all the bacteria have been spawned, if so set it to false
	if (!this->getAlive())
	{
		return;
	}	
	if (m_numAggressiveSal == 3 && m_numRegularSal == 5 && m_numEcoli == 2)
	{
		this->setAlive(false); 
		return;
	}
	int rand = randInt(0, 49);
	if (rand == 0) {//insert a new bacteria
		int randBac = randInt(0, 2); 
		bool bacMade = false;
		Actor* bacToAdd = nullptr;
		while (!bacMade) {
			if (randBac == 0 && m_numRegularSal < 5) {
				m_numRegularSal++;
				bacMade = true;
				bacToAdd = new RegularSalmonella(IID_SALMONELLA, this->getX(), this->getY(), 90, 0, 1, this->getWorld(), 4);
			}
			else if (randBac == 1 && m_numAggressiveSal < 3)
			{
				m_numAggressiveSal++;
				bacToAdd = new AggressiveSalmonella(IID_SALMONELLA, this->getX(), this->getY(), 90, 0, 1, this->getWorld(), 10);
				bacMade = true;
			}
			else if (randBac == 2 && m_numEcoli < 2) {
				m_numEcoli++;
				bacToAdd = new Ecoli(IID_ECOLI, this->getX(), this->getY(), 90, 0, 1, this->getWorld(), 5);
				bacMade = true;
			}
			if (!bacMade)
			{
				randBac = randInt(0, 2);
			}
			else {
				this->getWorld()->playSound(SOUND_BACTERIUM_BORN);
				this->getWorld()->addToList(bacToAdd);
			}
		}
	}
}

bool Pit::hitable() {
	return false;
}
bool Pit::hasHitpoints() {
	return false;
}

bool Pit::blockable() {
	return false;
}

//Bacterium
//this implements regular salmonella, aggressive salmonella, and ecoli
Bacterium::~Bacterium(){}
Bacterium::Bacterium(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw, int hitPoints)
	:Agent(ID, sX, sY, dir, depth, size, sw, hitPoints) {
	m_movementPlanDistance = 0; //start with zero move distance plan and no food eaten
	m_foodEaten = 0;
}
//damage bacteria by a certain amount, and kill if it damaged severly
void Bacterium::damage(int x) {
	if (this->getAlive() == true) {
		if ((this->getHitPoints() - x) <= 0) {
			this->die();
		}
		else {
			this->setHitPoints(this->getHitPoints() - x);
		}
		this->cry();
	}
}
//return the movementplandistance
int Bacterium::getMovementPlanDistance() {
	return m_movementPlanDistance;
}
//set the movementplandistance
void Bacterium::setMovementPlantDistance(int x) {
	m_movementPlanDistance = x;
}
//return the foodeaten
int Bacterium::getFoodEaten() {
	return m_foodEaten;
}
//set the food eaten
void Bacterium::setFoodEaten(int x) {
	m_foodEaten = x;
}

//Socrates
Socrates::Socrates(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw, int hitPoints)
	:Agent(ID, sX, sY, dir, depth, size, sw, hitPoints){
	m_sprayCharges = 20;
	m_flameThrowerCharges = 5;
}

void Socrates::damage(int x) {
	if (this->getHitPoints() <= x) {
		this->getWorld()->playSound(SOUND_PLAYER_DIE);
		this->setAlive(false);
	}
	else {
		this->getWorld()->playSound(SOUND_PLAYER_HURT);
		this->setHitPoints(this->getHitPoints() - x);
	}
}

void Socrates::doSomething() {
	double dx = 0;
	double dy = 0;
	if (this->getAlive() == false) //if socrates is dead move on
		return;
	int ch;
	if (this->getWorld()->getKey(ch)) { //get key entry	
		if (ch == KEY_PRESS_SPACE) {
			if (m_sprayCharges > 0) {
				getPositionInThisDirection(this->getDirection(), 1, dx, dy);
				Actor* spray = new DisinfectantSpray(IID_SPRAY, dx, dy, this->getDirection(), 1, 1, this->getWorld());
				this->getWorld()->addToList(spray);
				m_sprayCharges -= 1;
				this->getWorld()->playSound(SOUND_PLAYER_SPRAY);
			}
			return;
		}
		if (ch == KEY_PRESS_ENTER) {
			if (m_flameThrowerCharges > 0) {
				//need 16 flame objects
				int i = 0;
				while (i < 16) {
					getPositionInThisDirection(this->getDirection() + (22 * i), SPRITE_WIDTH, dx, dy);
					Actor* flame = new Flame(IID_FLAME, dx, dy, this->getDirection() + (22 * i), 1, 1, this->getWorld());
					this->getWorld()->addToList(flame);
					i++;
				}
				m_flameThrowerCharges--;
				this->getWorld()->playSound(SOUND_PLAYER_FIRE);
			}
			return;
		}
		if (ch == KEY_PRESS_LEFT){
			//adjust position angle by 5 degrees
			//ajust coordinates around the parmeter based on new pos angle
			//ajust coordinates around the parmeter based on new pos angle
			//set direction to pos angle + 180
			//calculate new angle
			double angle = atan2(this->getY() - (VIEW_HEIGHT/2), this->getX() - (VIEW_WIDTH/2)) * (180 / (3.14159));
			angle += 5;
			//move to new angle
			moveTo((VIEW_WIDTH/2) + ((VIEW_WIDTH / 2) * cos(angle * 1.0 / 360 * 2 * (4 * atan(1)))), (VIEW_HEIGHT / 2) + ((VIEW_HEIGHT / 2) * sin(angle * 1.0 / 360 * 2 * (4 * atan(1)))));
			int direction = int(180 + angle);
			direction = direction % 360;
			this->setDirection(direction);
			return;
		}
		if (ch == KEY_PRESS_RIGHT) {
			//m_pos -= 5;    //decrease pos angle by by
			//if (m_pos == 0)
			//	m_pos = 360;
			//calculate new angle
			double angle = atan2(this->getY() - (VIEW_HEIGHT / 2), this->getX() - (VIEW_WIDTH / 2)) * (180 / (3.14159));
			angle -= 5;
			//move to new angle
			moveTo((VIEW_WIDTH / 2) + ((VIEW_WIDTH / 2) * cos(angle * 1.0 / 360 * 2 * (4 * atan(1)))), (VIEW_HEIGHT / 2) + ((VIEW_HEIGHT / 2) * sin(angle * 1.0 / 360 * 2 * (4 * atan(1)))));
			int direction = int(180 + angle);
			direction = direction % 360;
			this->setDirection(direction);
			return;
		}
	}
	//if no other key was selected, increase t
	if (m_sprayCharges < 20) {
		m_sprayCharges += 1;
	}
	
}
Socrates::~Socrates() {

}

double Socrates::getFlameThrowerCharges() {
	return m_flameThrowerCharges;
}
void Socrates::setFlameThrowerCharges(double x) {
	m_flameThrowerCharges = x;
}
double Socrates::getSprayCharges() {
	return m_sprayCharges;
}
void Socrates::setSprayCharges(double x) {
	m_sprayCharges = x;
}

//PROJECTILE CLASS
//class to implement spray and flame
//Spray does not block other actors, nor can it be damaged
Projectile::~Projectile() {}
Projectile::Projectile(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw)
	:Actor(ID, sX, sY, dir, depth, size, sw) {};
bool Projectile::hitable() {
	return false;
}
bool Projectile::hasHitpoints() {
	return false;
}
void Projectile::doSomething() {
	if (!this->getAlive()) //return if it is dead
		return;
	this->specialization(); //call the subclass's specialization functin
	return;
}
bool Projectile::blockable() {
	return false;
}
bool Projectile::edible() {
	return false;
}
void Projectile::damage(int x) {
	x = 0;
	x++;
	this->setAlive(false);
}
int Projectile::getTravelDistance() {
	return m_travelDistance;
}
void Projectile::setTravelDistance(int increaseAmount) {
	if (increaseAmount <= 0) //do not change travel distance if it is less than 1
		return;
	m_travelDistance += increaseAmount;
}

//SPRAY CLASS
//inherits from Projectile, contains specialization function
void DisinfectantSpray::specialization() {
	//implement if hits target
	//if the spray collides with something with hit points, it will kill itself and 
	//collision function will other actor
	if (this->getWorld()->collisions(this, SPRITE_WIDTH, 2)) {
		this->setAlive(false);
		return;
	}
	
	//moveAngle moves sprite by two in it's direction
	moveAngle(this->getDirection(), SPRITE_WIDTH);
	this->setTravelDistance((SPRITE_WIDTH));
	//if travel distance is 112 skill the spray
	if (this->getTravelDistance() > 112) {
		this->setAlive(false);
	}
}
DisinfectantSpray::~DisinfectantSpray() {
}
DisinfectantSpray::DisinfectantSpray(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw):
				Projectile(ID, sX, sY, dir, depth, size, sw){}

//FLAME CLASS
//inherits from projectile, pratically same as spray
Flame::~Flame() {

}

Flame::Flame(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw)
	:Projectile(ID, sX, sY, dir, depth, size, sw){}

void Flame::specialization() {
	if (this->getWorld()->collisions(this, SPRITE_WIDTH, 5)) {
		this->setAlive(false);
		return;
	}

	//move sprite width by sprite_width
	moveAngle(this->getDirection(), SPRITE_WIDTH);
	this->setTravelDistance((SPRITE_WIDTH));
	//if the flame has already traveled 32 pixels delete it 
	if (this->getTravelDistance() > 32) {
		this->setAlive(false);
	}
}

//GOODIE
//will help implement all three kinds of goodies and fungus
//nonblockable, and nonedible
void Goodie::damage(int x) {
	x = 0;
	x++;
	this->setAlive(false);
}
bool Goodie::edible() {
	return false;
}
Goodie::~Goodie() {

}
void Goodie::doSomething() {
	if (!this->getAlive()) { //check if goodie is alive
		return;
	}
	//check if the goodie overlaps wit socrates
	if (this->getWorld()->socratesOverlap(this)) { //if true it overlaps
		this->specialization(); //call each specific goodie/fungus specialization function
		return;
	}
	if (getLife() == 0)//out of life
		this->setAlive(false);
	else
		decLife(); //decrement life by one tick
}
Goodie::Goodie(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw):
		Actor(ID, sX, sY, dir, depth, size, sw) {
	//every goodie/fungus has a random amount of lfie
	m_life = max(randInt(0, 300-10*this->getWorld()->getLevel()-1), 50);
}
bool Goodie::hitable() {
	return true;
}
bool Goodie::hasHitpoints() {
	return false;
}
//return the current life value of goodie
int Goodie::getLife() {
	return m_life;
}

bool Goodie::blockable() {
	return false;
}
//decrement life just reduces m_life by a value of one
void Goodie::decLife() {
	--m_life;
}
//Restore Health Goodie
RestoreHealthGoodie::~RestoreHealthGoodie() {	
}
RestoreHealthGoodie::RestoreHealthGoodie(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw):
	Goodie(ID, sX, sY, dir, depth, size, sw) {}
void RestoreHealthGoodie::specialization() {
	this->getWorld()->playSound(SOUND_GOT_GOODIE);
	this->getWorld()->increaseScore(250); //increase score by 250
	this->setAlive(false); //kill the goodie
	this->getWorld()->setSocratesHealth(); //reset socrates health
}

//Flamethrower goodie
FlameThrowerGoodie::~FlameThrowerGoodie() {
}
FlameThrowerGoodie::FlameThrowerGoodie(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw) :
	Goodie(ID, sX, sY, dir, depth, size, sw) {}
void FlameThrowerGoodie::specialization() {
	this->getWorld()->playSound(SOUND_GOT_GOODIE);
	this->getWorld()->increaseScore(300); //increase score by 300 points
	this->setAlive(false); //set life to false
	this->getWorld()->setSocratesFlameThrowerCharges(); //increment flame thrower charges by 
}

//Extra Life Goodie
ExtraLifeGoodie::~ExtraLifeGoodie(){}
ExtraLifeGoodie::ExtraLifeGoodie(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw):
		Goodie(ID, sX, sY, dir, depth, size, sw) {}
void ExtraLifeGoodie::specialization() {
	this->getWorld()->playSound(SOUND_GOT_GOODIE);
	this->getWorld()->increaseScore(500); //increase score by 500
	this->setAlive(false);	//set its alive to false
	this->getWorld()->incLives(); //increment flame thrower charges by 5
}

//FUNGUS
Fungus::~Fungus() {}
Fungus::Fungus(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw):
	Goodie(ID, sX, sY, dir, depth, size, sw) {}
void Fungus::specialization() {
 //if true it overlaps
	this->getWorld()->increaseScore(-50); //decrease score by 50
	this->setAlive(false); //kill the fungus
	this->getWorld()->decreaseSocratesHealth(20); //decrease socrates health by 20 hitpoints
	return;
}

//FOOD
//Food does nothing, does not block other things 
//Can not be damaged
Food::~Food(){}
Food::Food(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw)
	:Actor(ID, sX, sY, dir, depth, size, sw) {}
//Food doSomething does not do anthing
void Food::doSomething() {}
bool Food::hitable() {
	return false;
}
bool Food::hasHitpoints() {
	return false;
}
bool Food::blockable() {
	return false;
}
bool Food::edible() {
	return true;
}
void Food::damage(int x) {
	x = 0;
	x++;
	this->setAlive(false);
}

//Agressive Salmonella
AggressiveSalmonella::~AggressiveSalmonella(){}
AggressiveSalmonella::AggressiveSalmonella(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw, int hitPoints)
	: RegularSalmonella(ID, sX, sY, dir, depth, size, sw, hitPoints) {}
void AggressiveSalmonella::doSomething() {
	bool skip = false;
	if (!this->getAlive())
		return;
	if (this->getWorld()->socratesDistance(this) <= 72) {
		double newX, newY;
		double theta = this->getWorld()->targetAngle(this);
		if (theta < 0) {
			theta += 360;
		}
		getPositionInThisDirection(int(theta), 3, newX, newY);
		this->setDirection(int(theta));
		if (!this->getWorld()->blockChecking(newX, newY)) {
			this->moveTo(newX, newY);
		}
		skip = true;
	}
	if (this->getWorld()->socratesOverlap(this)) {//overlap with socrates
		this->getWorld()->decreaseSocratesHealth(2); //socrates takes 1 point of damage
		if (skip)
			return;
	}
	if (getFoodEaten() == 3) {//sal. has eaten 3 pieces of food
		double newX = this->getX();
		if (newX < VIEW_WIDTH / 2)
			newX += SPRITE_WIDTH / 2;
		else if (newX > VIEW_WIDTH / 2)
			newX -= SPRITE_WIDTH / 2;
		else
			newX = VIEW_WIDTH / 2;
		double newY = this->getY();
		if (newY < VIEW_HEIGHT / 2)
			newY += SPRITE_WIDTH / 2;
		else if (newY > VIEW_HEIGHT / 2)
			newY -= SPRITE_WIDTH / 2;
		else
			newY = VIEW_HEIGHT / 2;
		Actor* reg = new AggressiveSalmonella(IID_SALMONELLA, newX, newY, 90, 0, 1, this->getWorld(), 10);
		this->getWorld()->addToList(reg);
		this->setFoodEaten(0);
		if (skip)
			return;
	}
	else if (this->getWorld()->edibleOverlap(this)) {
		//set the food to be dead in the the function above
		this->setFoodEaten(this->getFoodEaten() + 1);
	}
	if (skip)
		return;
	//step 6 and beyond
	if (getMovementPlanDistance() > 0)
	{
		setMovementPlantDistance(getMovementPlanDistance() - 1);
		double newX, newY;
		getPositionInThisDirection(this->getDirection(), 3, newX, newY);
		if (!this->getWorld()->blockChecking(newX, newY)) {
			this->moveTo(newX, newY);
		}
		else {
			int randDir = randInt(0, 359);
			this->setDirection(randDir);
			this->setMovementPlantDistance(10);
		}
		return;
	}
	else {
		double foodX = 0.0, foodY = 0.0;
		this->getWorld()->getCloseEdible(this->getX(), this->getY(), foodX, foodY);
		if (foodX == -1 && foodY == -1) {
			int randDir = randInt(0, 359);
			this->setDirection(randDir);
			this->setMovementPlantDistance(10);
			return;
		}
		else {
			int posAngle = int((atan2(foodY - this->getY(), foodX - this->getX())) * (180 / 3.14159));
			if (posAngle < 0) {
				posAngle += 360;
			}
			this->setDirection(posAngle);
			double newX, newY;
			getPositionInThisDirection(this->getDirection(), 3, newX, newY);
			if (!this->getWorld()->blockChecking(newX, newY)) {
				this->moveTo(newX, newY);
			}
			else {
				int randDir = randInt(0, 359);
				this->setDirection(randDir);
				this->setMovementPlantDistance(10);
				return;
			}
		}
	}
}

//REGULAR SALMONELLA
//can be damamged, not blockable and has 50% chance of being turned into
//food when killed
RegularSalmonella::~RegularSalmonella() {

}
RegularSalmonella::RegularSalmonella(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw, int hitPoints)
					: Bacterium(ID, sX, sY, dir, depth, size, sw, hitPoints){}


void RegularSalmonella::cry() {
	if (this->getAlive() == true)
		this->getWorld()->playSound(SOUND_SALMONELLA_HURT);
	else
		this->getWorld()->playSound(SOUND_SALMONELLA_DIE);
}
void RegularSalmonella::die() {
	this->setAlive(false); //set the object to be dead	
	this->getWorld()->increaseScore(100); //increase socrates score by 100
	//find random number 0 - 1, if 1 make a new food object at the same 
	//location
	int chanceForFood = randInt(0, 1);
	if (chanceForFood == 1) {
		this->getWorld()->addToList(new Food(IID_FOOD,this->getX(), this->getY(), 90,1,1,this->getWorld()));
	}
}
void RegularSalmonella::doSomething() {
	if (!this->getAlive())
		return; //if dead return 
	if (this->getWorld()->socratesOverlap(this)) {//if regular Salmonella overlaps with socrates
		this->getWorld()->decreaseSocratesHealth(1); //socrates takes 1 point of damage
	}
	else {
		if (getFoodEaten() == 3) {//sal. has eaten 3 pieces of food
			//set the regular sal. to be newx and newy
			double newX = this->getX();
			if (newX < VIEW_WIDTH / 2)
				newX += SPRITE_WIDTH / 2;
			else if (newX > VIEW_WIDTH / 2)
				newX -= SPRITE_WIDTH / 2;
			else
				newX = VIEW_WIDTH / 2;
			double newY = this->getY();
			if (newY < VIEW_HEIGHT / 2)
				newY += SPRITE_WIDTH / 2;
			else if (newY > VIEW_HEIGHT / 2)
				newY -= SPRITE_WIDTH / 2;
			else
				newY = VIEW_HEIGHT/ 2;
			//create reg. sal at the newx and newy
			Actor* reg = new RegularSalmonella(IID_SALMONELLA, newX, newY, 90, 0,1,this->getWorld(), 4);
			this->getWorld()->addToList(reg); //add the reg sal. to the lsit
			this->setFoodEaten(0); //reset the amount of food back to 0
		}
		else if (this->getWorld()->edibleOverlap(this)) {
			//set the food to be dead in the the function above
			//increase amount of food eaten by one
			this->setFoodEaten(this->getFoodEaten() + 1); 
		}
	}
	//step 5 if movementplan is 1 or above
	if (getMovementPlanDistance() > 0) {
		setMovementPlantDistance(getMovementPlanDistance() - 1);
		double newX, newY; 
		getPositionInThisDirection(this->getDirection(), 3, newX, newY);
		//find newX and newY, and check if the movment here is blocked
		if (!this->getWorld()->blockChecking(newX, newY)) {
			this->moveTo(newX, newY); //if not move here
		}
		else {
			//if blocked pick a random angle and set direction there
			//set movementplandistance to 10
			int randDir = randInt(0, 359);
			this->setDirection(randDir);
			this->setMovementPlantDistance(10);
		}
		return;
	}
	else{//see if there is any food near the sal.
		double foodX = 0.0, foodY = 0.0;
		this->getWorld()->getCloseEdible(this->getX(), this->getY(), foodX, foodY);
		if (foodX == -1 && foodY == -1) { //no found in the distance
			int randDir = randInt(0, 359); //pick random direction and set dir to there
			this->setDirection(randDir);
			this->setMovementPlantDistance(10); //reset movementplan
			return;
		}
		else {
			//find angle between food and sal by atan2 function
			int posAngle = int((atan2(foodY- this->getY(), foodX -this->getX())) * (180 / 3.14159));
			if (posAngle < 0) { //if angle is below 0 add 360 to make it positive
				posAngle += 360;
			}
			//below is to set direction to new food and check if you can move there
			//if so move there, else find random direction and move
			this->setDirection(posAngle);
			double newX, newY;
			getPositionInThisDirection(this->getDirection(), 3, newX, newY);
			if (!this->getWorld()->blockChecking(newX, newY)) {
				this->moveTo(newX, newY);
			}
			else {
				int randDir = randInt(0, 359);
				this->setDirection(randDir);
				this->setMovementPlantDistance(10);
				return;
			}
		}
	}
}


//ECOLI
Ecoli::~Ecoli() {
}
Ecoli::Ecoli(int ID, double sX, double sY, Direction dir, int depth, double size, StudentWorld* sw, int hitPoints) 
	:Bacterium(ID, sX, sY, dir, depth, size, sw, hitPoints) {}

void Ecoli::doSomething() {
	//return if ecoli is already dead
	if (!this->getAlive()) {
		return;
	}
	//if overlaped with socrates, damage by 4 hitpoints
	if (this->getWorld()->socratesOverlap(this)) { //if overlap with socrates, decrease health by 4
		this->getWorld()->decreaseSocratesHealth(4);
	}
	else {
		//if three peices of food eaten, get ready split ecoli
		if (getFoodEaten() == 3) {//sal. has eaten 3 pieces of food
			//find new location for ecoli
			double newX = this->getX();
			if (newX < VIEW_WIDTH / 2)
				newX += SPRITE_WIDTH / 2;
			else if (newX > VIEW_WIDTH / 2)
				newX -= SPRITE_WIDTH / 2;
			else
				newX = VIEW_WIDTH / 2;
			double newY = this->getY();
			if (newY < VIEW_HEIGHT / 2)
				newY += SPRITE_WIDTH / 2;
			else if (newY > VIEW_HEIGHT / 2)
				newY -= SPRITE_WIDTH / 2;
			else
				newY = VIEW_HEIGHT / 2;
			//make new ecoli and add it to list and set food eaten to zero
			Actor* reg = new Ecoli(IID_ECOLI, newX, newY, 90, 0, 1, this->getWorld(), 5);
			this->getWorld()->addToList(reg);
			this->setFoodEaten(0);
		}
		else if (this->getWorld()->edibleOverlap(this)) {
			//set the food to be dead in the the function above
			this->setFoodEaten(this->getFoodEaten() + 1);
		}
	}
	//step 5
	if (this->getWorld()->socratesDistance(this) <= 256) {
		//find angle between socrates and positon of ecoli
		double theta = this->getWorld()->targetAngle(this);
		if (theta < 0) {
			theta += 360;
		}
		double newX, newY;
		//try to move 2 pixels in the new direction if blocked 
		//rotate around 10 degrees and try until it is able to be moved.
		for (int i = 0; i < 10; i++) {
			getPositionInThisDirection(int(theta), 2, newX, newY);
			if (!this->getWorld()->blockChecking(newX, newY)) {
				this->moveTo(newX, newY);
				break;
			}
			theta += 10;
		}
	}
}

void Ecoli::cry() {
	if (this->getAlive() == true)
		this->getWorld()->playSound(SOUND_ECOLI_HURT);
	else
		this->getWorld()->playSound(SOUND_ECOLI_DIE);
}

void Ecoli::die() {
	this->setAlive(false);
	this->getWorld()->increaseScore(100); //increase score by 100
	int chanceForFood = randInt(0, 1); //see if it will randomly turn into food
	if (chanceForFood == 1) { //if so add food to that location
		this->getWorld()->addToList(new Food(IID_FOOD, this->getX(), this->getY(), 90, 1, 1, this->getWorld()));
	}
}