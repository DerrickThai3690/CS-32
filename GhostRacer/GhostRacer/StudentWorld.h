#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>


const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class GhostRacer;

class StudentWorld : public GameWorld
{
public:
   StudentWorld(std::string assetPath);
   ~StudentWorld();
   virtual int init();
   virtual int move();
   virtual void cleanUp();
   
   //Adds and Actor to the Actor container
   void addActor(Actor* a);

   //return a pointer to this world's GhostRacer
   GhostRacer* getGhost();

   //decrement souls needed to save by 1
   void recordSoulSaved();

   // If actor a overlaps some live actor that is affected by a holy water
   // projectile, inflict a holy water spray on that actor and return true;
   // otherwise, return false.  (See Actor::beSprayedIfAppropriate.)
   bool sprayFirstAppropriateActor(Actor* a);

   // Return true if actor a1 overlaps actor a2, otherwise false.
   bool overlaps(const Actor* a1, const Actor* a2) const;

   //Check Lane foe CAWAs (Collision Avoidance Worthy Actors)
   bool checkLane(Actor* a);

private:
    std::list<Actor*> actors;
    GhostRacer* racer;
    int lastWhiteLine;
    int level;
    int bonus;
    int souls_to_save;
};

#endif // STUDENTWORLD_H_
