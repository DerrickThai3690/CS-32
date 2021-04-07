#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;


class Actor: public GraphObject { 
public:
	Actor(int imageID, double x, double y, int dir, double size, unsigned int depth, StudentWorld* sw);

	// Action to perform for each tick.
	virtual void doSomething();
	
	// Get this actor's alive status
	bool getAlive() const;

	// Set this actor's alive status
	bool setAlive(bool state);

	// Get this actor's vertical speed.
	double getVerSpeed() const;

	// Get this actor's horizontal speed
	double getHorSpeed() const;
	
	// Set this actor's vertical speed.
	void setVerSpeed(double s);

	// Set this actor's horizontal speed
	void setHorSpeed(double s);

	// If this actor is affected by holy water projectiles, then inflict that
	// effect on it and return true; otherwise, return false.
	virtual bool beSprayedIfAppropriate();

	// check if this actor is Collision Avoidance Worthy
	virtual bool isCollisionAvoidanceWorthy();

	// gets lane of objects
	int getLane();

	// Get this actor's world
	StudentWorld* getWorld();

private:
	double vSpeed;
	double hSpeed;
	bool isAlive;
	StudentWorld* studWorld;

};

class BorderLine : public Actor {
public:
	BorderLine(int imageID, double x, double y, StudentWorld* sw);
	virtual void doSomething();
};

class Agent : public Actor {
public:
	Agent(int imageID, double x, double y, int dir, double size, unsigned int depth, StudentWorld* sw);
	virtual void doSomething() = 0;
	
	// Get an Agent's Health
	int getHealth() const;

	// Increase HP by num
	void setHealth(int num);

	// Do what the spec says happens when hp units of damage is inflicted.
	// Return true if this agent dies as a result, otherwise false.
	bool takeDamageAndPossiblyDie(int hp);

	// checks if agents are Collision Avoidance Worthy
	virtual bool isCollisionAvoidanceWorthy();

protected:
	// Plays an Pedestrians(derived from Agent) sound when hurt but not dead
	virtual int soundWhenHurt();

	// Plays an Pedestrians(derived from Agent) sound when dead
	virtual int soundWhenDie();

private:
	int health;
};

class GhostRacer : public Agent {
public:	
	GhostRacer(StudentWorld* sw);
	virtual void doSomething();
	
	// Plays ghost Racer's sound when dead
	virtual int soundWhenDie();
	
	// Gets the amount of holy water left
	int getNumSprays() const;
	
	// Increase the number of holy water projectiles of Ghost Racer
	void increaseSprays(int amt);
	
	// Spin as a result of hitting an oil slick.
	void spin();

private:
	int holyWater;
};

class Ped : public Agent {
public:
	Ped(int imageID, double x, double y, double size, StudentWorld* sw);

protected:
	// Get a Pedestrian's Move Plan
	int getMovePlan();
	
	// Set a Pedestrian's move plan
	void setMovePlan(int num);
private:
	int mPlan;
};

class HumanPed : public Ped {
public:
	HumanPed(double x, double y, StudentWorld* sw);
	virtual void doSomething();
	virtual bool beSprayedIfAppropriate();
};

class ZombiePed : public Ped {
public:
	ZombiePed(double x, double y, StudentWorld* sw);
	virtual void doSomething();
	virtual bool beSprayedIfAppropriate();
private:
	int grunt;
};

class ZombieCab : public Agent {
public:
	ZombieCab(double x, double y, int verSpeed, StudentWorld* sw);
	virtual void doSomething();
	virtual bool beSprayedIfAppropriate();

	virtual int soundWhenHurt();
	virtual int soundWhenDie();

private:
	bool damageGR;
	double planLength;
};

class Spray : public Actor {
public:
	Spray(double x, double y, int dir, StudentWorld* sw);
	virtual void doSomething();
private:
	double maxDistance;
};

class GhostRacerActivatedObject : public Actor
{
public:
	GhostRacerActivatedObject(int imageID, double x, double y, int dir, double size, StudentWorld* sw);
	//Common actions done by all GhostRacerActivatedObjects
	virtual void doSomething();
	
	virtual bool beSprayedIfAppropriate();

protected:
	// Return the object's increase to the score when activated.
	virtual int getScoreIncrease() const;

	// Return the sound to be played when the object is activated.
	virtual int getSound() const;

};

class OilSlick : public GhostRacerActivatedObject
{
public:
	OilSlick(double x, double y, StudentWorld* sw);
	virtual void doSomething();
	virtual int getSound() const;

};

class HealingGoodie : public GhostRacerActivatedObject
{
public:
	HealingGoodie(double x, double y, StudentWorld* sw);
	virtual bool beSprayedIfAppropriate();
	virtual void doSomething();
	virtual int getScoreIncrease() const;
};

class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
	HolyWaterGoodie(double x, double y, StudentWorld* sw);
	virtual bool beSprayedIfAppropriate();
	virtual void doSomething();
	virtual int getScoreIncrease() const;
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
	SoulGoodie(double x, double y, StudentWorld* sw);
	virtual void doSomething();
	virtual int getScoreIncrease() const;
	virtual int getSound() const;
};

#endif // ACTOR_H_
