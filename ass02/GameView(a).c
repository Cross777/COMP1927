// GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h" // ... if you decide to use the Map ADT
 
#define NUM_HUNTERS 4


// Encounter IDs
#define NONE 0
#define VAMPIRE 1
#define TRAP 2

#define ABBREV_LEN 2

// localConnections()
#define CONNECTED 1
#define NUM_SEAS 10
#define SEA_LOCATIONS { 0, 4, 7, 10, 23, 32, 33, 43, 48, 64 }
#define MAX_CONNECTED 30

// shortestPath()
#define RAIL_MOVE (currentView->roundNum + getCurrentPlayer(currentView)) % 4
#define INF 999
#define VISITED 1
#define LAST 0

typedef struct _hunterData {
    PlayerID hunterID;
    int health;
    LocationID loc;

    // holds the last locations visited
    // the most recently visited location being in element 0
    LocationID trail[TRAIL_SIZE];
} hunterData;

typedef struct _draculaData {
    int health;
    LocationID loc;

    // trail
    // holds the last locations visited
    // the most recently visited location being in element 0
    LocationID trail[TRAIL_SIZE];

    //trapVampTrail encounterTrail[TRAIL_SIZE];
} draculaData;
     

typedef struct _encounterData {
    LocationID loc;
    int encounterType;
} encounterData;


struct gameView {

    int gameScore;
    int roundNum;
    int turnNum;
    Map gameMap;
    
    // hunters data
    hunterData hunters[NUM_HUNTERS];

    // Dracula data
    draculaData drac;

    // traps and vampires
    encounterData encounters[TRAIL_SIZE];
};


typedef int (*compfn)(const void*, const void*);

typedef struct _link 
{
    LocationID loc;
    TransportID type;
    float dist;
} link;

typedef struct _link *Link;

// Function prototypes
static void runPastPlays(GameView g, char * pastPlays, int turnsPlayed);
static void addToHistory (LocationID histArray[TRAIL_SIZE], LocationID loc);
static void addToEncounterHistory (encounterData histArray[TRAIL_SIZE],
                                   encounterData encounter);
static void removeFromEncounterHistory (encounterData histArray[TRAIL_SIZE],
                                        encounterData encounter);
static int isSeaLocation (LocationID loc);
static int isDoubleBack (LocationID loc);
static void assignListToIndex (LocationID *index, LocationID *list, 
                               int s_list);
static void setListNOWHERE (LocationID *list, int size);
static void getRailMoves (GameView currentView, int railMove, 
                          LocationID *connectedIndex, LocationID from);
// shortest path fucntions
static Link processBuffer(GameView currentView, Link pQ, int *pQLen, 
                          LocationID *buffer, int bufferSize, Link prev,
                          LocationID curr);
static void appendLocationsToLink(GameView currentView, Link a, int *aLen,
                                  LocationID *b, int size, LocationID curr);
static Link shift(Link pQ, int *pQLen, LocationID *location);
static void updatePrev (GameView currentView, Link pQ, int *pQLen, 
                        Link prev, LocationID curr);
// qsort function
int cmpDist (Link a, Link b);


// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    // malloc memory to hold the game
    GameView gameView = malloc(sizeof(struct gameView));
    
    // establish map
    gameView->gameMap = newMap();
        
    // get the number of turns made
    int turnsSum = (pastPlays[0] == 0) ? 0 : 1;
    char * currStr = strstr (pastPlays," ");
    while (currStr != NULL) {
        currStr++;
        turnsSum++;
        currStr = strstr (currStr," ");
    }


    gameView->gameScore = GAME_START_SCORE;
    gameView->roundNum = turnsSum/5;
    gameView->turnNum = turnsSum % 5;

    // set up all of the hunters
    int i;
    int j;
    for (i = 0; i < NUM_HUNTERS; i++) {
        gameView->hunters[i].hunterID = i;
        gameView->hunters[i].health = GAME_START_HUNTER_LIFE_POINTS;
        gameView->hunters[i].loc = NOWHERE;

        for (j = 0; j < TRAIL_SIZE; j++)
            gameView->hunters[i].trail[j] = NOWHERE;
    }
    
    // set up dracula
    gameView->drac.health = GAME_START_BLOOD_POINTS;
    gameView->drac.loc = NOWHERE;
    for (j = 0; j < TRAIL_SIZE; j++)
        gameView->drac.trail[j] = NOWHERE;

    for (j = 0; j < TRAIL_SIZE; j++) {
        gameView->encounters[j].loc = NOWHERE;
        gameView->encounters[j].encounterType = NONE;
    }

    // interpret pastPlays string
    runPastPlays(gameView, pastPlays, turnsSum);

    return gameView;
}

// Interprets the past plays string and inputs the appropriate data
// on a turn by turn basis
static void runPastPlays(GameView g, char * pastPlays, int turnsPlayed)
{
    int currChar = 0;
    int currPlayer = PLAYER_LORD_GODALMING;

    char playerIDChars[NUM_PLAYERS] = {'G', 'S', 'H', 'M', 'D'};


    // decipher each turn
    // each turn is 8 chars long (7 + a space)
    while (currChar < turnsPlayed*8 - 1) {
        // assert we are dealing with the correct player
        assert (pastPlays[currChar] == playerIDChars[currPlayer]);
        currChar++;

        // get the location of that player
        char locAbbrev[ABBREV_LEN + 1];
        strncpy(locAbbrev, (pastPlays+currChar), ABBREV_LEN);
        int locID = abbrevToID(locAbbrev);
        currChar += ABBREV_LEN;

        int doubleBackPos = 0;  // stores which history item he doubled
                                // back from (if needed)
        
        
        // deal with special cases for dracula
        if (locID == NOWHERE) {
            if(strcmp(locAbbrev,"C?") == 0)
                locID = CITY_UNKNOWN;
            else if (strcmp(locAbbrev,"S?") == 0)
                locID = SEA_UNKNOWN;
            else if (strcmp(locAbbrev,"HI") == 0)
                locID = HIDE;
            else if (strcmp(locAbbrev,"TP") == 0)
                //locID = TELEPORT;
                locID = CASTLE_DRACULA;
            else if (locAbbrev[0] == 'D')
               if (locAbbrev[1] >= '1' && locAbbrev[1] <= '5') {
                   doubleBackPos = atoi(locAbbrev+1);
                   locID = DOUBLE_BACK_1 - 1 + doubleBackPos;
               }
        }

        // deal with the rest of the info based on whether the player is
        // a hunter or dracula
        // If dracula
        if (currPlayer == PLAYER_DRACULA) {
            // save location that just left the trail
            LocationID locLeftTrail = g->drac.trail[TRAIL_SIZE - 1];

            // Input his current location and add it to his history
            g->drac.loc = locID;
            addToHistory(g->drac.trail, locID);

            // Give dracula extra health if he is in his castle
            if (locID == CASTLE_DRACULA)
                g->drac.health += LIFE_GAIN_CASTLE_DRACULA;

            // check if dracula was at sea and take life of accordingly
            else if (isSeaLocation(locID) ||
            (isDoubleBack(locID) && isSeaLocation(g->drac.trail[doubleBackPos])))
                g->drac.health -= LIFE_LOSS_SEA;

            // the games score is decremented each turn dracula gets
            g->gameScore -= SCORE_LOSS_DRACULA_TURN;

            if (pastPlays[currChar] == 'T') {
                // Trap was Placed
                encounterData placeTrap = {locID, TRAP};
                addToEncounterHistory(g->encounters, placeTrap);

            }
            currChar++;

            if (pastPlays[currChar] == 'V') {
                // immature Vampire was placed
                encounterData placeVamp = {locID, VAMPIRE};
                addToEncounterHistory(g->encounters, placeVamp);
            }
            currChar++;

            if (pastPlays[currChar] == 'M') {
                // a Trap has left the trail
                // Take the trap off the encounter trail 
                LocationID trapLoc = locLeftTrail;
                encounterData removeTrap = {trapLoc, TRAP};
                removeFromEncounterHistory(g->encounters, removeTrap);
            } else if (pastPlays[currChar] == 'V') {
                // a Vampire has matured
                g->gameScore -= SCORE_LOSS_VAMPIRE_MATURES;

                // Take the vampire off the encounter trail
                LocationID vampLoc = locLeftTrail;
                encounterData removeVamp = {vampLoc, VAMPIRE};
                removeFromEncounterHistory(g->encounters, removeVamp);
            }
            currChar+=2;

        // Must be a hunter in else case
        } else {
            // if the new loc is the same as the old, give more life
            if (g->hunters[currPlayer].loc == locID)
                g->hunters[currPlayer].health += LIFE_GAIN_REST;
            // life cannot exceed 9
            if (g->hunters[currPlayer].health > 9)
                g->hunters[currPlayer].health = 9;

            // update location and ad to the hunter's history
            g->hunters[currPlayer].loc = locID;
            addToHistory(g->hunters[currPlayer].trail, locID);


            int j;
            for (j = 0; j < 4; j++) {
                if (pastPlays[currChar] == 'T') {
                    // Trap encountered
                    g->hunters[currPlayer].health -= LIFE_LOSS_TRAP_ENCOUNTER;

                    // remove the trap from the map
                    encounterData removeTrap = {locID, TRAP};
                    removeFromEncounterHistory(g->encounters, removeTrap);

                } else if (pastPlays[currChar] == 'V') {
                    // immature Vampire was encountered 
                    // remove the vampire from the map without a trace
                    encounterData removeVamp = {locID, VAMPIRE};
                    removeFromEncounterHistory(g->encounters, removeVamp);

                } else if (pastPlays[currChar] == 'D') {
                    // Dracula was confronted
                    // TODO
                    //  - Make sure this is the right order
                    g->drac.health -= LIFE_LOSS_HUNTER_ENCOUNTER;
                    g->hunters[currPlayer].health -= LIFE_LOSS_DRACULA_ENCOUNTER;
                }
                currChar++;
            }

            if (g->hunters[currPlayer].health <= 0) {
                g->hunters[currPlayer].loc = ST_JOSEPH_AND_ST_MARYS;
                g->hunters[currPlayer].health = GAME_START_HUNTER_LIFE_POINTS;
            }
        }

        
        // go to the next player and loop around if necessary
        currPlayer++;
        if (currPlayer > PLAYER_DRACULA)
            currPlayer = PLAYER_LORD_GODALMING;

        // skip over the space separating the turns
        currChar++;
    }
}

// updates the specified history array, placing the new item at the
// beginning and removing the last item
// Also updates map as appropriate (i.e. deletes trap, matures vampire) 
static void addToHistory (LocationID histArray[TRAIL_SIZE], LocationID loc)
{
    int i;
    // first check what the last element is
    if (histArray[TRAIL_SIZE - 1] != NOWHERE) {
        // TODO Maybe
    }


    for (i = TRAIL_SIZE - 1; i > 0; i--)
        histArray[i] = histArray[i - 1];
    histArray[0] = loc;
}

// updates the specified encounter history array, placing the new item
// at the beginning and removing the last item
static void addToEncounterHistory (encounterData histArray[TRAIL_SIZE],
encounterData encounter)
{
    int i;
    for (i = TRAIL_SIZE - 1; i > 0; i--) {
        histArray[i].loc = histArray[i - 1].loc;
        histArray[i].encounterType = histArray[i - 1].encounterType;
    }
    histArray[0].loc = encounter.loc;
    histArray[0].encounterType = encounter.encounterType;
}


// look for the specified encounter and remove it from the trail of
// encounters. Startls looking from the oldest encounter;
static void removeFromEncounterHistory (encounterData histArray[TRAIL_SIZE],
encounterData encounter)
{
    int i;
    for (i = 0; i < TRAIL_SIZE ; i++) {
        if (histArray[i].loc == encounter.loc &&
        histArray[i].encounterType == encounter.encounterType) {
            histArray[i].encounterType = NONE;
            return;
        }
    }
}



// returns TRUE if the specified location is considered to be at sea
static int isSeaLocation (LocationID loc)
{
    if (validPlace(loc))
        return isSea(loc);
    else
        return (loc == SEA_UNKNOWN);
}


// returns TRUE if the specified location is one of the double backs
static int isDoubleBack (LocationID loc)
{
    return (loc >= DOUBLE_BACK_1 && loc <= DOUBLE_BACK_5);
}

     
// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    disposeMap(toBeDeleted->gameMap);
    free( toBeDeleted );
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round getRound(GameView currentView)
{
    return currentView->roundNum;
}

// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
    return currentView->turnNum;
}

// Get the current score
int getScore(GameView currentView)
{
    return currentView->gameScore;
}

// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    if (player == PLAYER_DRACULA)
        return currentView->drac.health;
    else
        return currentView->hunters[player].health;
}

// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player)
{ 
    if (player == PLAYER_DRACULA) {
        return currentView->drac.loc;
    } else {
        return currentView->hunters[player].loc;
    }
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player, 
                LocationID trail[TRAIL_SIZE])
{
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        if (player == PLAYER_DRACULA)
            trail[i] = currentView->drac.trail[i];
        else 
            trail[i] = currentView->hunters[player].trail[i];
    }
}

//// Functions that query the map to find information about connectivity

// Returns an array of LocationIDs for all directly connected locations
// NOTE: need to work out how to free malloced space
LocationID *connectedLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    int railMove = ((round + player) % 4);
    int i, c;
    *numLocations = 0;
    
    // create index for connected locations initialised to 0
    LocationID connectedIndex[NUM_MAP_LOCATIONS] = { 0 };
    // from should be included as connected
    connectedIndex[from] = CONNECTED;

    // will be passed into functions that return it holding LocationID's
    LocationID *buffer = malloc(MAX_CONNECTED*sizeof(LocationID));
    setListNOWHERE(buffer, MAX_CONNECTED);
    
    
    if (road) {
        buffer = allAdjacent(currentView->gameMap, from, ROAD, buffer);
        for (i = 0; buffer[i] != NOWHERE && i < MAX_CONNECTED; i++) 
            connectedIndex[buffer[i]] = CONNECTED;
    }
    // Dracula can not travel to hospital
    if (player == PLAYER_DRACULA)
        connectedIndex[ST_JOSEPH_AND_ST_MARYS] = NOWHERE;  
        
    #ifdef DEBUG
        printIndex("connectedIndex: after(road)", connectedIndex, MAX_MAP_LOCATION);
    #endif
    
    if (rail && player != PLAYER_DRACULA) {
        if (railMove > 0) {
            
            setListNOWHERE(buffer, MAX_CONNECTED);
                    
            getRailMoves(currentView, railMove, connectedIndex, from);
                    
        }
    }
    
    #ifdef DEBUG
        printIndex("connectedIndex: after(rail)", connectedIndex, MAX_MAP_LOCATION);
    #endif
    
    if (sea) {
        setListNOWHERE(buffer, MAX_CONNECTED);
        
        // account for all connections sea -> sea || land
        buffer = allAdjacent(currentView->gameMap, from, BOAT, buffer);
        
        assignListToIndex(connectedIndex, buffer, MAX_CONNECTED);
    
        // account for connections land -> sea   
        LocationID seaArray[NUM_SEAS] = SEA_LOCATIONS; 
         
        
        for (i = 0; i < NUM_SEAS; i++) 
            if (haveConnectionByType(currentView->gameMap, seaArray[i], from, BOAT, player))      
                connectedIndex[seaArray[i]] = CONNECTED;
                      
            
    }
    
    #ifdef DEBUG
        printIndex("connectedIndex: after(boat)", connectedIndex, MAX_MAP_LOCATION);
    #endif

    // final array to be loaded with connected locationID's, then returned
    LocationID *connected = malloc(MAX_CONNECTED*sizeof(LocationID));
    setListNOWHERE(connected, MAX_CONNECTED);

    c = 0;
    // traverse index and assign any connected LocationID's to connected 
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        if (connectedIndex[i] == CONNECTED) {
            #ifdef DEBUG
                printf("Assign Index to list:............%d\n", i);
            #endif
            connected[c] = i; 
            (*numLocations)++;
            c++;
        }
    }
    connected = realloc(connected, ((*numLocations)*sizeof(LocationID)));
    free(buffer);
    return connected;

}
// Function 
void getRailMoves (GameView currentView, int railMove, LocationID *connectedIndex, LocationID from)
{
    // index tracks which LocationID's have been checked
    LocationID checked[NUM_MAP_LOCATIONS] = { 0 };
    
    // will maintain a list of all locationID's to be checked
    LocationID toCheck[MAX_MAP_LOCATION];
    setListNOWHERE(toCheck, MAX_MAP_LOCATION);
    
    // will be passed into functions that return it holding LocationID's
    LocationID *buffer = malloc(MAX_CONNECTED*sizeof(LocationID));
    setListNOWHERE(buffer, MAX_CONNECTED);
    
    
    int i, c, s;
    int moveThresh = 0;
    toCheck[0] = from;
    // pos for assigning toCheck and threshold for tracking railMoves 
    s = 1;
    moveThresh = 1;
    for (i = 0; toCheck[i] != NOWHERE && i < MAX_MAP_LOCATION; i++) {
      
        if (i == moveThresh) {
            railMove--; moveThresh = s;
        }
        
        if (railMove == 0) break;
        
        if (!checked[toCheck[i]]) {
            #ifdef DEBUG
                printf("Checking........... %d\n", toCheck[i]);
                printList("toCheck[]:", toCheck, MAX_MAP_LOCATION);
            #endif
            buffer = allAdjacent(currentView->gameMap, toCheck[i], RAIL, buffer);
            
            checked[toCheck[i]] = 1;
            assignListToIndex(connectedIndex, buffer, MAX_CONNECTED);
            
            for (c = 0; buffer[c] != NOWHERE && c < MAX_CONNECTED; c++) {
                if (!checked[buffer[c]]) {
                    toCheck[s] = buffer[c];     s++;
                }                    
            }
            setListNOWHERE(buffer, MAX_CONNECTED);
   
        }
        
    }
    free(buffer);

}

void setListNOWHERE (LocationID *list, int size)
{
    int i;
    for (i = 0; i < size; i++)
        list[i] = NOWHERE;
}

void assignListToIndex (LocationID *index, LocationID *list, int s_list)
{
    int i;
    for (i = 0; list[i] != NOWHERE && i < s_list; i++)
        index[list[i]] = CONNECTED;
        
   
}


void printIndex (char *indexName, LocationID *index, int size)
{
    printf("*********\n%s:\n", indexName);
    int i;
    for (i = 0; i < size+1; i++) {
        if (index[i] == CONNECTED) printf("-- %d --", i); 
    } 
    printf("\n*********\n");
}

void printList (char *listName, LocationID *list, int size)
{
    printf("*********\n%s:\n", listName);
    int i;
    for (i = 0; i < size; i++) {
        if (list[i] == NOWHERE) break;
        printf("%d: %d   ", i, list[i]);   
        if (((i % 9) == 0) && i != 0) printf("\n");
    } 
    printf("\n*********\n");
}


Link shortestPath (GameView currentView, LocationID from, LocationID to, PlayerID player, int road, int rail, int boat)
{
    if (!RAIL_MOVE) rail = 0;
    
    // nodes that have been visited    
    int visited[NUM_MAP_LOCATIONS] = { 0 };
    
    // current shortest distance from the source
    // the previous node for current known shortest path
    Link prev;
    if(!(prev = malloc(NUM_MAP_LOCATIONS*sizeof(link))))
        fprintf(stderr, "GameView.c: shortestPath: malloc failure (prev)");
    int i;
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        prev[i].loc = NOWHERE;
        prev[i].type = NONE;
        if (i != from) {
            prev[i].dist = INF; // arbitrary large distance
        } else {
            prev[i].dist = LAST; 
        }
    }
        
    LocationID *buffer;
        
    // a priority queue that dictates the order LocationID's are checked
    Link pQ = NULL;
    int pQLen = 0; 
    // load initial into queue
    if (!(pQ = malloc((pQLen+1)*sizeof(link))))
        fprintf(stderr, "GameView.c: shortestPath: malloc failure (pQ)");
    pQ[pQLen++].loc = from;
       
    int buffSize;
    i = 0;
    while (!visited[to]) {
        
        LocationID curr = 0;

        if (visited[pQ[0].loc]) { 
            pQ = shift(pQ, &pQLen, &curr);    
            continue;
        } 
        // freeing malloc from connectedLocations()
        if (i != 0) free(buffer); 
        
        // load connected locations into buffer   
        buffer = connectedLocations(currentView, &buffSize, pQ[0].loc, 
                                    player, currentView->roundNum, road, 
                                    rail, boat); 
        // mark current node as visited
        visited[pQ[0].loc] = VISITED;
           
        pQ = shift(pQ, &pQLen, &curr);
           
        pQ = processBuffer(currentView, pQ, &pQLen, buffer, buffSize, prev,
                      curr);
        i++;
        
    }
    
    
    free(buffer);
    free(pQ);
    return prev;
}

static Link processBuffer (GameView currentView, Link pQ, int *pQLen, 
                           LocationID *buffer, int bufferSize, Link prev,
                           LocationID curr)
{
    printf("before appendLocationsToLink\npQ: %p, pQLen: %d, prev: %p\
            curr: %d\n", pQ, *pQLen, prev, curr);
    // calculate distances and add b information
    TransportID type[MAX_TRANSPORT] = { NONE };
    int i, totalConns = 0;  
    for (i = 0; i < bufferSize; i++) {
        totalConns += connections(currentView->gameMap, curr, 
                                  buffer[i], type);
    }
    if(!(pQ = realloc(pQ, ((*pQLen)+totalConns)*sizeof(link))))
        fprintf(stderr, "GameView.c: appendLocationsToLink: realloc failure (pQ)");
    appendLocationsToLink(currentView, pQ, pQLen, buffer, bufferSize, curr);
    
    //LocationID newcur = curr;
    //Link newprev = prev;
    Link newpQ = pQ;
    int *newLen = pQLen;
    printf("after appendLocationsToLink\n");
            
    updatePrev(currentView, newpQ, newLen, prev, curr);   
    
    printLinkIndex("prev", prev, NUM_MAP_LOCATIONS);
    
    qsort((void *) pQ, *pQLen, sizeof(link), (compfn)cmpDist);
    
    // qsort sanity check
    int qsortErr = 0;
    for (i = 0; i < *pQLen-1; i++) 
        if (pQ[i].dist > pQ[i+1].dist) qsortErr = 1;
        
    if (qsortErr) {
        fprintf(stderr, "loadToPQ: qsort did not sort succesfully");
        abort();
    }  
    return pQ;
}

void printLinkIndex (char *name, Link a, int size)
{
    printf("************\n%s\n************\n", name);
    int i;
    for (i = 0; i < size; i++) {
        if (a[i].loc != NOWHERE) {
            printf("%s[%d]:    loc: %d    type: %d    dist: %f\n", name, 
                    i, a[i].loc, a[i].type, a[i].dist);    
        }
    }
    printf("fin\n************\n");
}

void printLinkList (char *name, Link a, int size) 
{
    printf("************\n%s\n************\n", name);
    int i;
    for (i = 0; i < size && a[i].loc != NOWHERE; i++) {
        printf("%s[%d]:    loc: %d    type: %d    dist: %f\n", name, 
               i, a[i].loc, a[i].type, a[i].dist);    
    }
    printf("fin\n************\n");

}

void printLinkLoc (char *name, Link a, int size) 
{
    printf("************\n%s\n************\n%s:", name, name);
    int i;
    for (i = 0; i < size && a[i].loc != NOWHERE; i++) {
        printf(" %d->%d ", i, a[i].loc); 
        if (i % 9 == 0) printf("\n");   
    }
    printf("\nfin\n************\n");


}

static void appendLocationsToLink (GameView currentView, Link a, int *aLen,
                                   LocationID *b, int bLen, LocationID curr)
{
    int i, c, conns;
    TransportID type[MAX_TRANSPORT] = { NONE };     
   
    for (i = 0; i < bLen; i++) {   
        conns = connections(currentView->gameMap, curr, b[i], type);
        for (c = 0; c < conns; c++) {
            /*if (!*aLen) {
                a = malloc(((*aLen)+1)*sizeof(link));
            } else {
                printf("In appendLocationsToLink(): reallocing memory\n");
                newA = realloc(a, ((*aLen)+1)*sizeof(link));
                a = newA; 
            } */
            a[*aLen].loc = b[i];
            a[(*aLen)++].type = type[c];            
        }            
    }
    printLinkLoc("pQ", a, *aLen); 
    printf("cv: %p - pQLen: %d - pQ: %p - curr: %d\n", currentView, *aLen,
            a, curr); 
}

static void updatePrev (GameView currentView, Link pQ, int *pQLen,
                        Link prev, LocationID curr) 
{
    int i;
    // update and pQ prev with curr distance and prev location ID 
    for (i = 0; i < *pQLen; i++) {
        pQ[i].dist = prev[curr].dist + 1; // adding in rail calc
        if (pQ[i].dist < prev[pQ[i].loc].dist) {
            // update 
            prev[pQ[i].loc].dist = pQ[i].dist; 
            prev[pQ[i].loc].loc = curr;
            prev[pQ[i].loc].type = pQ[i].type;
        }
    }
}

static Link shift (Link pQ, int *pQLen, LocationID *location)
{
    int i;
    *location = pQ[0].loc;
    for (i = 0; i < *pQLen-1; i++) {
        pQ[i] = pQ[i+1];
    }
    Link newpQ;
    if (!(newpQ = realloc(pQ, (*pQLen-1)*sizeof(link)))) 
        fprintf(stderr, "GameView.c: shift: realloc failure (pQ)");
    (*pQLen)--;
    //free(pQ); 
    pQ = newpQ;
    
    return pQ;
}

LocationID getLinkLoc (Link a, int index)
{
    return a[index].loc;
}


int cmpDist (Link a, Link b)
{
    float res = a->dist - b->dist;
    if (res < 0.001 && res > -0.001) {
        return 0;
    } else {
        return res;
    }  
}

int locHaveConnection (GameView currentView, PlayerID player, 
                       LocationID from, LocationID to, TransportID type) 
{
    if (!validPlace(from) || !validPlace(to)) {
        fprintf(stderr, "GameView.c: locHaveConnection(): Invalid location input: %d ----> %d", from, to);
        abort();
    }
    // RAIL CASE
    int railMove = RAIL_MOVE;
    int connected[NUM_MAP_LOCATIONS] = { 0 };
    // load index with available rail moves based on railMove
    getRailMoves(currentView, railMove, connected, from);
    
    // RAIL case
    if (type == RAIL) {
        if (player == PLAYER_DRACULA) {
            return FALSE;
        // hunter case(s)
        } else {
            return connected[to];
        }     
    // ALL case
    } else if (type == ANY) {
        if (player == PLAYER_DRACULA) {
            // ANY for Drac is BOAT && ROAD
            return (haveConnectionByType(currentView->gameMap, from, to, 
                                         ROAD, player) || 
                    haveConnectionByType(currentView->gameMap, from, to, 
                                         BOAT, player)); 
        } else {
            return (connected[to] || 
                    haveConnectionByType(currentView->gameMap, from, to, 
                                         type, player));
        }   
    // ROAD && BOAT cases        
    } else {
        return haveConnectionByType(currentView->gameMap, from, to, 
                                    type, player);
    }
}


