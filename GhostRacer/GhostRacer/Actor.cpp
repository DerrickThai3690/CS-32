#define _USE_MATH_DEFINES
#include "Actor.h"
#include "StudentWorld.h"
#include <math.h>
#include<iostream>
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//ACTOR
//All actors are alive by default and are not moving
//They also hold a pointer to the StudentWorld used for the game
Actor::Actor(int imageID, double x, double y, int dir, double size, unsigned int depth, StudentWorld* sw)
	:GraphObject(imageID, x, y, dir, size, depth)
{
	setVerSpeed(0);
	setHorSpeed(0);
	studWorld = sw;
	setAlive(true);
}

//Movement used by all Actors
void Actor::doSomething() {
	double vert_speed = getVerSpeed() - getWorld()->getGhost()->getVerSpeed();
	double horiz_speed = getHorSpeed();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;
	moveTo(new_x, new_y);
	return;
}

bool Actor::getAlive() const {
	return isAlive;
}

bool Actor::setAlive(bool state) {
	isAlive = state;
	return isAlive;
}

double Actor::getVerSpeed() const {
	return vSpeed;
}

double Actor::getHorSpeed() const {
	return hSpeed;
}

void Actor::setVerSpeed(double s) {
	vSpeed = s;
}
void Actor::setHorSpeed(double s) {
	hSpeed = s;
}

//Actors by default cannot be sprayed by holy water projectiles
bool Actor::beSprayedIfAppropriate() {
	return false;
}

//Actors by default are not Collision Avoidance Worthy
bool Actor::isCollisionAvoidanceWorthy () {
	return false;
}

//returns one of the 3 lanes of the game that the actor is within
//if actor is not on the highway then return outside the highway
int Actor::getLane() {
	if (getX() >= LEFT_EDGE && getX() < LEFT_EDGE + ROAD_WIDTH / 3) {
		return 1;
	}
	else if (getX() >= LEFT_EDGE + ROAD_WIDTH / 3 && getX() < RIGHT_EDGE - ROAD_WIDTH / 3) {
		return 2;
	}
	else if (getX() >= RIGHT_EDGE - ROAD_WIDTH / 3 && getX() < RIGHT_EDGE) {
		return 3;
	}
	else {
		//not in any lane
		return 0;
	}
}

StudentWorld* Actor::getWorld()  {
	return studWorld;
}


//BORDERLINE
BorderLine::BorderLine(int imageID, double x, double y, StudentWorld* sw)
	:Actor(imageID, x, y, 0, 2.0, 2, sw)
{
	setVerSpeed(-4);
	setHorSpeed(0);
}

void BorderLine::doSomething() {
	Actor::doSomething();
	//if Borderlines are outside of the screen set them not alive and deleted
	if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
		setAlive(false);
		return;
	}

	return;
}

//AGENT
Agent::Agent(int imageID, double x, double y, int dir, double size, unsigned int depth, StudentWorld* sw)
	:Actor(imageID, x, y, dir, size, depth, sw)
{
	setVerSpeed(-4);
	setHorSpeed(0);
	setHealth(100);
}

int Agent::getHealth()const {
	return health;
}

//Health of any agents cannot be increased to be greater than 100
void Agent::setHealth(int num) {
	if (health <= 100) {
		health = num;
	}
}

//decrement an Agent's health by the input
//if the Agent dies play their death sound
//return true if Agent has died
bool Agent::takeDamageAndPossiblyDie(int hp) {
	setHealth(getHealth() - hp);
	if (getHealth() <= 0) {
		setAlive(false);
		getWorld()->playSound(soundWhenDie());
		return true; // return if dead
	}
	else {
		return false; // retrn if alive
	}
}

//All agents are Collision Avoidance Worthy by default
//(Ghost Racer, All pedestrians, Zombie Cab)
bool Agent::isCollisionAvoidanceWorthy() {
	return true;
}

//Plays Pedestrians(derived from Agent) sounds by default
int Agent::soundWhenHurt() {
	return SOUND_PED_HURT;
}

int Agent::soundWhenDie() {
	return SOUND_PED_DIE;
}

//GHOST RACER
GhostRacer::GhostRacer(StudentWorld* sw)
	:Agent(IID_GHOST_RACER, 128, 32, 90, 4.0, 0, sw)
{
	setVerSpeed(0);
	holyWater = 10;
}


void GhostRacer::doSomething() {
	if (!getAlive()) {
		return;
	}
	
	//if Ghost Racer hits the either edges of the highway
	//take damage and redirect ghost racer away from the edge then play a crashing sound
	if (getX() <= LEFT_EDGE) {
		if (getDirection() > 90) {
			takeDamageAndPossiblyDie(10);
			setDirection(82);
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
		}
	}
	if (getX() >= RIGHT_EDGE) {
		if (getDirection() < 90) {
			takeDamageAndPossiblyDie(10);
			setDirection(98);
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
		}
	}

	//read keystroke if any
	int key;

	//keyboard inputs that control Ghost Racer movement and actions
	if (getWorld()->getKey(key)) {
		switch (key) {
			//if SPACEBAR is pressed shoot a holy water projectile from Ghost Racer
		case KEY_PRESS_SPACE:
			if (getNumSprays() > 0) {
				double x;
				double y;
				getPositionInThisDirection(getDirection(), SPRITE_HEIGHT, x, y);
				getWorld()->addActor(new Spray(x, y + SPRITE_HEIGHT, getDirection(), getWorld()));
				getWorld()->playSound(SOUND_PLAYER_SPRAY);
				holyWater--;
			}
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() < 114) {
				setDirection(getDirection() + 8);
			}
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() > 66) {
				setDirection(getDirection() - 8);
			}
			break;
		case KEY_PRESS_UP:
			if (getVerSpeed() < 5) {
				setVerSpeed(getVerSpeed() + 1);
			}
			break;
		case KEY_PRESS_DOWN:
			if (getVerSpeed() > -1) {
				setVerSpeed(getVerSpeed() - 1);
			}
			break;
		}

	}
	//Moves Ghost Racer
	double max_shift_per_tick = 4.0;
	double direction = getDirection() ;
	double delta_x = cos(direction * M_PI / 180) * max_shift_per_tick;
	double cur_x = getX();
	double cur_y = getY();
	moveTo(cur_x + delta_x, cur_y);

	return;
}

int GhostRacer::soundWhenDie() {
	return SOUND_PLAYER_DIE;
}

int GhostRacer::getNumSprays() const {
	return holyWater;
}

void GhostRacer::increaseSprays(int amt) {
	holyWater += amt;
}

//makes Ghost Racer change direction clockwise and counter-clockwise randomly
//Ghost Racer's direction cannot spin below 60 or above 120
void GhostRacer::spin() {
	int randomNum  = randInt(0, 1);
	if (getDirection() >= 60 && getDirection() <= 120) {
		if (randomNum == 0) {
			setDirection(getDirection() - randInt(5, 20));
		}
		else if(randomNum ==1){
			setDirection(getDirection() + randInt(5, 20));
		}
	}
	
}

//PEDESTRIANS
Ped::Ped(int imageID, double x, double y, double size, StudentWorld* sw)
	:Agent(imageID, x, y, 0, size, 0, sw)
{
	setMovePlan(0);
	setHealth(2);
}

int Ped::getMovePlan() {
	return mPlan;
}

void Ped::setMovePlan(int num) {
	mPlan = num;
}

//Human Pedestrian
HumanPed::HumanPed(double x, double y, StudentWorld* sw)
	: Ped(IID_HUMAN_PED, x, y, 2.0, sw)
{

}

void HumanPed::doSomething() {
	if (!getAlive()) {

		return;
	}
	//if humanPed overlaps with ghostRacer
	//Ghost Racer dies and game ends
	if (getWorld()->overlaps(this, getWorld()->getGhost())) {
		getWorld()->getGhost()->setAlive(false);
	}

	//Movement
	Actor::doSomething();

	//If Human Ped moves off screen it is set to not alive and deleted
	if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
		setAlive(false);
		return;
	}
	//move plan is decremented by 1 per tick
	//if move plan is 0 or less, make a new move plan
	setMovePlan(getMovePlan() - 1);
	if (getMovePlan() > 0) {
		return;
	}
	else {
		while (getHorSpeed() == 0) {
			setHorSpeed(randInt(-3, 3));
		}
		setMovePlan(randInt(4, 32));
		//Human Ped turns around 
		if (getHorSpeed() < 0) {
			setDirection(180);
		}
		else {
			setDirection(0);
		}
	}
	return;
}
//If Human Ped is sprayed by holy water
//it turns around and plays a hurt sound
bool HumanPed::beSprayedIfAppropriate() {
	setHorSpeed(getHorSpeed() * -1);
	if (getHorSpeed() < 0) {
		setDirection(0);
	}
	else {
		setDirection(180);
	}
	getWorld()->playSound(soundWhenHurt());

	return true;
 }

//Zombie Pedestrian
ZombiePed::ZombiePed(double x, double y, StudentWorld* sw)
	: Ped(IID_ZOMBIE_PED, x, y, 3.0, sw)
{
	grunt = 0;
}

void ZombiePed::doSomething() {
	if (!getAlive()) {
		return;
	}

	//if ZombiePed overlaps with ghostRacer then ZombiePed dies
	//Ghost Racer takes 5 points of damage
	if (getWorld()->overlaps(this, getWorld()->getGhost())) {
		getWorld()->getGhost()->takeDamageAndPossiblyDie(5);
		this->takeDamageAndPossiblyDie(2);
		return;
	}

	//if ZombiePed within 30 pixels of Ghost Racer and ZombiePed is in front of Ghost Racer
	//Zombie Ped will turn towards the Ghost Racer and go towards it
	if (abs(getX() - getWorld()->getGhost()->getX()) <= 30 && getY() > getWorld()->getGhost()->getY()){
		setDirection(270);
		if (getX() < getWorld()->getGhost()->getX()) {
			setHorSpeed(1);
		}
		else if(getX() > getWorld()->getGhost()->getX()){
			setHorSpeed(-1);
		}
		else {
			setHorSpeed(0);
		}
	}

	//ZombiePed makes a grunt sound periodically
	grunt--;
	if (grunt <= 0) {
		getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
		grunt = 20;
	}

	//Movement
	Actor::doSomething();
	//If ZombiePed goes off screen it is set not alive and deleted
	if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
		setAlive(false);
		return;
	}

	//if there are still plans to move then move
	//for every tick decrement move plan by 1
	if (getMovePlan() > 0) {
		setMovePlan(getMovePlan() - 1);
		return;
	}
	//if movement plan is 0 make a new one
	else {
		while (getHorSpeed() == 0) {
			setHorSpeed(randInt(-3, 3));
		}
		setMovePlan(randInt(4, 32));
		if (getHorSpeed() < 0) {
			setDirection(180);
		}
		else {
			setDirection(0);
		}
	}
	return;
}


/*
When ZombiePed is sprayed by holy water projectile
it takes 1 point of damage
if ZombiePed dies it has 1 out of 5 chance to drop a Healing Goodie and increases score
else if ZombiePed survives it plays a hurt sound
*/
bool ZombiePed::beSprayedIfAppropriate() {
	takeDamageAndPossiblyDie(1);
	if (!getAlive()) {
		if (!getWorld()->overlaps(this, getWorld()->getGhost())) {
			if (randInt(0, 4) == 0) {
				getWorld()->addActor(new HealingGoodie(getX(), getY(), getWorld()));
			}
		}
		getWorld()->increaseScore(150);
	}
	else {
		getWorld()->playSound(SOUND_PED_HURT);
	}
	return true;
}


ZombieCab::ZombieCab(double x, double y, int verSpeed, StudentWorld* sw)
	:Agent(IID_ZOMBIE_CAB, x, y, 90, 4.0, 0, sw)
{
	setVerSpeed(verSpeed);
	setHealth(3);
	setHorSpeed(0);
	planLength = 0;
	damageGR = false;
}

void ZombieCab::doSomething() {
	if (!getAlive()) {
		return;
	}

	/*
	if cab overlaps with GhostRacer
	cab veers towards one direction and Ghost racer takes 20 damage
	and cab will mark that it has damaged Ghost Racer so it will not hurt GhostRacer again
	if still overlapping in the next tick
	*/
	if (getWorld()->overlaps(this, getWorld()->getGhost())) {
		if (!damageGR) {
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
			getWorld()->getGhost()->takeDamageAndPossiblyDie(20);
			//If cab left of Ghost Racer it veers left
			if (getX() <= getWorld()->getGhost()->getX()) {
				setHorSpeed(-5);
				setDirection(120 + randInt(0, 19));
			}
			//If cab right of Ghost Racer it veers right
			if (getX() > getWorld()->getGhost()->getX()) {
				setHorSpeed(5);
				setDirection(60 - randInt(0, 19));
			}
			damageGR = true;
		}
	}

	//Movement
	Actor::doSomething();
	if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
		setAlive(false);
		return;
	}

	//Check Lane for CAWA
	if (getWorld()->checkLane(this)) {
		return;
	}

	//decrement plan and if plan is empty then get a new plan
	planLength--;
	if (planLength > 0) {
		return;
	}
	else {
		planLength = randInt(4, 32);
		setVerSpeed(getVerSpeed() + randInt(-2, 2));
	}
	return;
}

//If cab is sprayed by holy water it takes 1 damage
//If cab dies it has 1 out of 5 chance to drop an Oil Slick where it died
//and Player gets 200 points
//else if cab doesnt die cab plays hurt sounds
bool ZombieCab::beSprayedIfAppropriate() {
	takeDamageAndPossiblyDie(1);
	if (!getAlive()) {
		if (randInt(0, 4) == 0) {
			getWorld()->addActor(new OilSlick(getX(), getY(), getWorld()));
		}
		getWorld()->increaseScore(200);
		return true;
	}
	else {
		getWorld()->playSound(soundWhenHurt());
	}
	return true;
}

int ZombieCab::soundWhenHurt() {
	return  SOUND_VEHICLE_HURT;
}

int ZombieCab::soundWhenDie() {
	return  SOUND_VEHICLE_DIE;
}

Spray::Spray(double x, double y, int dir, StudentWorld* sw)
	:Actor(IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1, sw)
{
	maxDistance = 160;
}

void Spray::doSomething() {
	if (!getAlive()) {
		return;
	}
	//checks all sprayable actors and sprays the first available one
	//then the holy water projectile is set not alive
	if (getWorld()->sprayFirstAppropriateActor(this)) {
		setAlive(false);
		return;
	}
	//if no sprayable actors then holy water continues moving forwards
	else {
		moveForward(SPRITE_HEIGHT);
		maxDistance -= SPRITE_HEIGHT;
	}
	//if holy water goes off screen it is set not alive and deleted
	if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
		setAlive(false);
		return;
	}

	//if Holy water has traveled more than 160 pixels it is set not alive and deleted
	if (maxDistance <= 0) {
		setAlive(false);
	}


}

GhostRacerActivatedObject::GhostRacerActivatedObject(int imageID, double x, double y, int dir, double size, StudentWorld* sw) 
	:Actor(imageID, x, y, dir, size, 2, sw)
{
	setVerSpeed(-4);
	setHorSpeed(0);
}

//Common code for all GhostRacerActivatedObject
//GhostRacerActivatedObject are set not alive and their sounds are played
//and Players score is increased respectively
void GhostRacerActivatedObject::doSomething() {
	setAlive(false);
	getWorld()->playSound(getSound());
	getWorld()->increaseScore(getScoreIncrease());
}

bool GhostRacerActivatedObject::beSprayedIfAppropriate() {
	return false;
}

//by default GhostRacerActivatedObject like Oil slick will not have a score increase
int GhostRacerActivatedObject::getScoreIncrease() const {
	return 0;
}

int GhostRacerActivatedObject::getSound()const {
	return SOUND_GOT_GOODIE;
}

OilSlick::OilSlick(double x, double y, StudentWorld* sw)
	:GhostRacerActivatedObject(IID_OIL_SLICK, x, y, 0, randInt(2,5), sw)
{
}

void OilSlick::doSomething() {
	Actor::doSomething();
	//if oil slick is off the screen, it is deleted
	if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
		setAlive(false);
		return;
	}

	//If Oil Slick overlaps with Ghost racer, spin the ghost racer
	if (getWorld()->overlaps(this, getWorld()->getGhost())) {
		getWorld()->playSound(getSound());
		getWorld()->getGhost()->spin();
	}
}

int OilSlick::getSound() const{
	return  SOUND_OIL_SLICK;
}

HealingGoodie::HealingGoodie(double x, double y, StudentWorld* sw) 
	:GhostRacerActivatedObject(IID_HEAL_GOODIE, x, y, 0, 1.0, sw)
{

}

void HealingGoodie::doSomething() {
	int curr_health = getWorld()->getGhost()->getHealth();
	Actor::doSomething();
	//if healing goodie goes off the screen, it is deleted
	if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
		setAlive(false);
		return;
	}
	//if healing Goodie overlaps wit hGhost racer, heal the racer by 10 health
	if (getWorld()->overlaps(this, getWorld()->getGhost())) {
		getWorld()->getGhost()->setHealth(curr_health + 10);
		GhostRacerActivatedObject::doSomething();

	}
}

//healingGoodie is deleted if sprayed by holy water
bool HealingGoodie::beSprayedIfAppropriate() {
	setAlive(false);
	return true;
}

int HealingGoodie::getScoreIncrease() const {
	return 250;
}

HolyWaterGoodie::HolyWaterGoodie(double x, double y, StudentWorld* sw)
	:GhostRacerActivatedObject(IID_HOLY_WATER_GOODIE, x, y, 90, 2.0, sw)
{

}

void HolyWaterGoodie::doSomething() {
	//Movement
	Actor::doSomething();
	//if HolyWaterGoodie  goes off screen it is deleted
	if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
		setAlive(false);
		return;
	}
	//if HolyWaterGoodie overlaps with GhostRacer
	//GhostRacer stores 10 additional counts of Holy water 
	if (getWorld()->overlaps(this, getWorld()->getGhost())) {
		getWorld()->getGhost()->increaseSprays(10);
		GhostRacerActivatedObject::doSomething();

	}

}

//HolyWaterGoodie is deleted if sprayed by holy water
bool HolyWaterGoodie::beSprayedIfAppropriate() {
	setAlive(false);
	return true;
}

int HolyWaterGoodie::getScoreIncrease() const {
	return 50;
}

SoulGoodie::SoulGoodie(double x, double y, StudentWorld* sw)
	:GhostRacerActivatedObject(IID_SOUL_GOODIE, x, y, 0, 4.0, sw)
{

}

void SoulGoodie::doSomething() {
	//Movement
	Actor::doSomething();
	//If SoulGoodie goes off Screen it is deleted
	if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
		setAlive(false);
		return;
	}

	//If SoulGoodie Overlaps with Ghost Racer,
	//Decrease the number of souls required to finish the level
	if (getWorld()->overlaps(this, getWorld()->getGhost())) {
		getWorld()->recordSoulSaved();
		GhostRacerActivatedObject::doSomething();
	}
	//Soul Goodie constantly rotates clockwise
	setDirection(getDirection()-10);
}

int SoulGoodie::getScoreIncrease() const {
	return 100;
}

int SoulGoodie::getSound() const {
	return SOUND_GOT_SOUL;
}

