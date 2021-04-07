#include "StudentWorld.h"
#include <string>
#include "Actor.h"
#include <sstream>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
 
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init()
{
    //Initialize Game status variables
    level = getLevel();
    souls_to_save = (level * 2) + 5;
    bonus = 5000;

    //Makes and adds a Ghost Racer to the game
    racer = new GhostRacer(this);
    addActor(racer);
    
    //allocate yellow lines on left and right edges 
    for (int i = 0; i < VIEW_HEIGHT / SPRITE_HEIGHT; i++) {
        addActor(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, i * SPRITE_HEIGHT, this));
        addActor(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i * SPRITE_HEIGHT, this));
    }

    //allocate white lines of the highway
    for (int i = 0; i < VIEW_HEIGHT / (4*SPRITE_HEIGHT); i++) {
        addActor(new BorderLine(IID_WHITE_BORDER_LINE, LEFT_EDGE + (ROAD_WIDTH / 3), i * 4 * SPRITE_HEIGHT, this));
        addActor(new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_EDGE - (ROAD_WIDTH / 3), i * 4 * SPRITE_HEIGHT, this));
        lastWhiteLine = i * 4 * SPRITE_HEIGHT;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    list<Actor*>::iterator it = actors.begin();
    //All Actors do something for this tick
    for (; it != actors.end(); it++) {
        if ((*it)->getAlive()) {
            (*it)->doSomething();
            if (!getGhost()->getAlive()) {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            else if (souls_to_save == 0){
                increaseScore(bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
        //check if any Actors are dead and remove them
        if (!(*it)->getAlive() && it != actors.begin()) {
            delete* it;
            it = actors.erase(it);
            it--;
        }
    }   
  
    //Add new Actors

    //Border Lines
    //Records the last white line added to the screen
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    lastWhiteLine += -4 - getGhost()->getVerSpeed();

    int delta_y = new_border_y - lastWhiteLine;
    if (delta_y >= SPRITE_HEIGHT) {
        addActor(new BorderLine(IID_YELLOW_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2.0, new_border_y, this));
        addActor(new BorderLine(IID_YELLOW_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2.0, new_border_y, this));
    }

    //if new white line above last white line,
    //update last white line to the one above it
    if (delta_y >= 4 * SPRITE_HEIGHT) {
        lastWhiteLine = new_border_y;
        addActor(new BorderLine(IID_WHITE_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2.0 + ROAD_WIDTH / 3, new_border_y, this));
        addActor(new BorderLine(IID_WHITE_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2.0 - ROAD_WIDTH / 3, new_border_y, this));
    }

    //Oil Slicks
    int ChanceOilSlick = max(150 - level * 10, 40);
    if (randInt(0, ChanceOilSlick - 1) == 0) {
        addActor(new OilSlick(randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, this));
    }

    //Zombie Pedestrians
    int ChanceZombiePed = max(100 - level * 10, 20);
    if (randInt(0, ChanceZombiePed - 1) == 0) {
        addActor(new ZombiePed(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this));
    }

    //Human Pedestrians
    int ChanceHumanPed = max(200 - level * 10, 30);
    if (randInt(0, ChanceHumanPed - 1) == 0) {
        addActor(new HumanPed(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this));
    }
    
    //Holy Water Refills
    int ChanceOfHolyWater = 100 + 10 * level;
    if (randInt(0, ChanceOfHolyWater - 1) == 0) {
        addActor(new HolyWaterGoodie(randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, this));
    }

    //Lost Soul
    int ChanceOfLostSoul = 100;
    if (randInt(0, ChanceOfLostSoul - 1) == 0) {
        addActor(new SoulGoodie(randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, this));
    }
    
    //Zombie Cab
    int ChanceVehicle = max(100 - level * 10, 20);
    if (randInt(0, ChanceVehicle) == 0) {
        
        double min = VIEW_HEIGHT;
        double max = 0;
        double curr = 0;
        double cab_y = 0;
        double verSpeed = 0;
        Actor* closest = nullptr;
        double chosen_lane = 0;

        //Used to pick random lanes for Cab generation
        vector<double> arr = { 1,2,3 };
        double cur_lane = 0;
        double lane = 0;
        for (int i = 0; i < 3; i++) {
            //generate an int between 0 and 2
            //then use that to pick a current lane
            //erase for the next iteration of the loop to pick a different lane
            lane = randInt(0, arr.size() - 1);
            cur_lane = arr[lane];
            arr.erase(arr.begin() + lane);
            //finds closest Collision Avoidance Worthy Actor(CAWA) to bottom of screen
            for (it = actors.begin(); it != actors.end(); it++) {
                if ((*it)->isCollisionAvoidanceWorthy() && cur_lane == (*it)->getLane()) {
                    curr = (*it)->getY();
                    if (min > curr) {
                        min = curr;
                        closest = (*it);
                    }
                }

            }
            //If no CAWA in lane or it is far away enough
            //set variables for cab to be generated
            //such as lane, y coord, and vertical speed
            if (closest == nullptr || closest->getY() > (VIEW_HEIGHT / 3)) {
                chosen_lane = cur_lane;
                cab_y = SPRITE_HEIGHT / 2;
                verSpeed = getGhost()->getVerSpeed() + randInt(2, 4);
                break;
            }

            //finds closest Collision Avoidance worthy Actor to top of screen
            for (it = actors.begin(); it != actors.end(); it++) {
                if ((*it)->isCollisionAvoidanceWorthy() && cur_lane == (*it)->getLane()) {
                    curr = (*it)->getY();
                    if (max < curr) {
                        max = curr;
                        closest = (*it);
                    }
                }

            }
            //If no CAWA in lane or it is far away enough
            //set variables for cab to be generated
            //such as lane, y coord, and vertical speed
            if (closest == nullptr || closest->getY() < (VIEW_HEIGHT * 2 / 3)) {
                chosen_lane = cur_lane;
                cab_y = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                verSpeed = getGhost()->getVerSpeed() - randInt(2, 4);
                break;
            }

        }
        //generates a cab based on lane chosen
        if (chosen_lane == 1) {
            addActor(new ZombieCab(ROAD_CENTER - ROAD_WIDTH / 3, cab_y, verSpeed, this));
        }
        else if (chosen_lane == 2) {
            addActor(new ZombieCab(ROAD_CENTER, cab_y, verSpeed, this));
        }
        else if (chosen_lane == 3) {
            addActor(new ZombieCab(ROAD_CENTER + ROAD_WIDTH / 3, cab_y, verSpeed, this));
        }

    }

    //Update the status of the game
    ostringstream oss;
    
    //Displays Game status on screen
    oss << "Score: " << getScore() << " Lvl: " << getLevel() << " Souls2Save: " << souls_to_save << " Lives: " <<  getLives() <<
        " Health: "<< getGhost()->getHealth() << " Sprays: " << getGhost()->getNumSprays() << " Bonus: " << bonus;

    string status = oss.str();
    setGameStatText(status);

    //Bonus points decrement by 1 per tick
    bonus--;

    //continue on to next tick of the game
    return GWSTATUS_CONTINUE_GAME;
}


//Deletes all dynamically allocated memory (Actors)
void StudentWorld::cleanUp()
{
    list<Actor*>::iterator it;

    it = actors.begin();

    while (it != actors.end()) {
        delete* it;
        it =  actors.erase(it);
    }
    
}

void StudentWorld::addActor(Actor* a) {
    actors.push_back(a);
}

GhostRacer* StudentWorld::getGhost() {
    return racer;
}

//decrements the amount of souls needed to finish a level 
void StudentWorld::recordSoulSaved() {
    souls_to_save--;
}

//Looks for any sprayable actors and sprays the first one
//if holy water projectile overlaps with that actor
bool StudentWorld::sprayFirstAppropriateActor(Actor *a) {
    list<Actor*>::iterator it = actors.begin();
    while (it != actors.end()) {
        if (overlaps((*it), a)) {
            if ((*it)->beSprayedIfAppropriate()) {
                return true;
            }
        }
        it++;
    }
    return false;
}

//checks if Actors are overlapping
bool StudentWorld::overlaps(const Actor* a1, const Actor* a2) const {
    double delta_x = abs(a1->getX() - a2->getX());
    double delta_y = abs(a1->getY() - a2->getY());
    double radius_sum = a1->getRadius() + a2->getRadius();
    if (delta_x < radius_sum * 0.25 && delta_y < radius_sum * 0.6) {
        return true;
    }
    return false;
}

//Checks a cabs lane for CAWAs(Collision Avoidance Worthy Actors) in it
//Changes speed if CAWA is in lane and close enough
bool StudentWorld::checkLane(Actor* a) {
    if (a->getVerSpeed() > getGhost()->getVerSpeed()) {
        //find closest CAWAs ABOVE zombie cab
        list<Actor*>::iterator it = actors.begin();
        double min_y = VIEW_HEIGHT;
        double temp_y;
        //Look through all actors and compare Y coordinates of CAWAs in the same lane
        while (it != actors.end()) {
            if ((*it)->getLane() == a->getLane() && (*it)->isCollisionAvoidanceWorthy() && (*it)->getY() > a->getY()) {
                temp_y = (*it)->getY();
                if (min_y > temp_y) {
                    min_y = temp_y;
                }
            }
            it++;
        }
        //if the closest CAWA is less than 96 pixels away from cab
        //decrease the cab's speed
        if (min_y != VIEW_HEIGHT && abs(min_y - a->getY()) < 96) {
            a->setVerSpeed(a->getVerSpeed() - 0.5);
            return true;
        }
    }
    else if(a->getVerSpeed() <= getGhost()->getVerSpeed()){
        //find closest CAWAs BELOW zombie cab excluding ghost racer
        list<Actor*>::iterator it = actors.begin();
        double max_y = 0;
        double temp_y = 0;
        //Look through all actors and compare Y coordinates of CAWAs in the same lane
        while (it != actors.end()) {
            if ((*it)->getLane() == a->getLane() && (*it)->isCollisionAvoidanceWorthy() && (*it)->getY() < a->getY()) {
                temp_y = (*it)->getY();
                if (max_y < temp_y) {
                    max_y = temp_y;
                }
            }
            it++;
        }
        //if the closest CAWA is less than 96 pixels away from cab
        //increase the cab's speed
        if (max_y != 0 && abs(max_y - a->getY()) < 96 && max_y != getGhost()->getY()) {
            a->setVerSpeed(a->getVerSpeed() + 0.5);

            return true;
        }
    }
    return false;
}
