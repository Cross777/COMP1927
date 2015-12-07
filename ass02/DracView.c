// DracView.c ... DracView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"
#include "Map.h"

#include <stdio.h> // for debugging using printf's
#include <string.h> // for parsing pastPlays string

#define VAMPIRE 1
#define TRAP 2

#define ABBREV_LEN 2

#define IS_DRAC 1
#define NOT_DRAC 0

#define DB_THRESH 1

typedef struct _encounterData {
    LocationID loc;
    int encounterType;
} encounterData;

struct dracView {
    GameView game;
    encounterData encounters[TRAIL_SIZE + 1]; // traps and vampires
};

// Function prototypes
static void runPastPlays(DracView g, char * pastPlays, int turnsPlayed);
static void addToEncounterHistory (encounterData histArray[TRAIL_SIZE], encounterData encounter);
static void removeFromEncounterHistory (encounterData histArray[TRAIL_SIZE], encounterData encounter, int dracOrNot);

// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
    DracView view = malloc(sizeof(struct dracView));
    view->game = newGameView(pastPlays, messages);
    int turnsSum = 0;
    char * currStr = strstr(pastPlays," ");
    while (currStr != NULL) {
        currStr++;
        turnsSum++;
        currStr = strstr(currStr, " ");
    }
    if (turnsSum > 0)
        turnsSum++;
    runPastPlays(view, pastPlays, turnsSum);
    return view;
}

// Interprets the past plays string and inputs the appropriate data
// on a turn by turn basis
static void runPastPlays(DracView g, char * pastPlays, int turnsPlayed)
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
                if (locAbbrev[1] >= '1' && locAbbrev[1] <= '5')
                    locID = DOUBLE_BACK_1 - 1 + atoi(locAbbrev+1);
                //locID = g->drac.trail[atoi(locAbbrev+1) - 1];
        }

        // deal with the rest of the info based on whether the player is
        // a hunter or dracula
        if (currPlayer == PLAYER_DRACULA) { // dracula's turn

            if (pastPlays[currChar] == 'T') {
                encounterData placeTrap = {locID, TRAP}; // Trap was Placed
                addToEncounterHistory(g->encounters, placeTrap);

            }
            currChar++;

            if (pastPlays[currChar] == 'V') {
                encounterData placeVamp = {locID, VAMPIRE}; // immature Vampire was placed
                addToEncounterHistory(g->encounters, placeVamp);
            }
            currChar++;

            if (pastPlays[currChar] == 'M') {
                LocationID trapLoc = UNKNOWN_LOCATION;
                encounterData removeTrap = {trapLoc, TRAP}; // trap left the trail
                removeFromEncounterHistory(g->encounters, removeTrap, IS_DRAC); // del from trail
            } else if (pastPlays[currChar] == 'V') {
                LocationID vampLoc = UNKNOWN_LOCATION;
                encounterData removeVamp = {vampLoc, VAMPIRE}; // vampire matured
                removeFromEncounterHistory(g->encounters, removeVamp, IS_DRAC); // del from trail
            }
            currChar += 2;
        } else { // hunter's turn
            int j;
            for (j = 0; j < 4; j++) {
                if (pastPlays[currChar] == 'T') {
                    encounterData removeTrap = {locID, TRAP}; // trap found
                    removeFromEncounterHistory(g->encounters, removeTrap, NOT_DRAC); // remove from map
                } else if (pastPlays[currChar] == 'V') {
                    encounterData removeVamp = {locID, VAMPIRE}; // vampire found
                    removeFromEncounterHistory(g->encounters, removeVamp, NOT_DRAC); // remove from map

                }
                currChar++;
            }
        }
        // go to the next player and loop around if necessary
        if (++currPlayer > PLAYER_DRACULA)
            currPlayer = PLAYER_LORD_GODALMING;
        // skip over the space separating the turns
        currChar++;
    }
}

// updates the specified encounter history array, placing the new item
// at the beginning and removing the last item
// (relegating it into the 7th element where it stays for one round
// to make sure the trap was correctly removed implicitly)
static void addToEncounterHistory (encounterData histArray[TRAIL_SIZE + 1],
encounterData encounter)
{
    int i;
    for (i = TRAIL_SIZE; i > 0; i--) {
        histArray[i].loc = histArray[i - 1].loc;
        histArray[i].encounterType = histArray[i - 1].encounterType;
    }
    histArray[0].loc = encounter.loc;
    histArray[0].encounterType = encounter.encounterType;
}


// look for the specified encounter and remove it from the trail of
// encounters. Starts looking from the oldest encounter
// if drac calls the function, it should have no work to do:
// make sure the item was just deleted (is in the 7th element)
static void removeFromEncounterHistory (encounterData histArray[TRAIL_SIZE + 1], encounterData encounter, int drac)
{
    int i;
    // If dracula, make sure that thing was just removed from the history
    if (drac) {
        for (i = TRAIL_SIZE; i >= 0; i--) {
            if (histArray[i].encounterType == encounter.encounterType) {
                histArray[i].encounterType = NONE;
                return;
            }
        }
    } else {
        // find and remove the item if a hunter found the item
        for (i = 0; i < TRAIL_SIZE; i++) {
            if (histArray[i].loc == encounter.loc && histArray[i].encounterType == encounter.encounterType) {
                histArray[i].encounterType = NONE;
                return; // we're done in this case - we only have to find it once
            }
        }
    }
}


// Frees all memory previously allocated for the DracView toBeDeleted
void disposeDracView(DracView toBeDeleted)
{
    disposeGameView(toBeDeleted->game);
    free(toBeDeleted);
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(DracView currentView)
{
    // printf("Round = %d\n", getRound(currentView->game)); // debug
    return getRound(currentView->game);
}

// Get the current score
int giveMeTheScore(DracView currentView)
{
    return getScore(currentView->game);
}

// Get the current health points for a given player
int howHealthyIs(DracView currentView, PlayerID player)
{
    return getHealth(currentView->game, player);
}

// Get the current location id of a given player
LocationID whereIs(DracView currentView, PlayerID player)
{

    LocationID cur = getLocation(currentView->game, player);
    if (cur == UNKNOWN_LOCATION && player == PLAYER_DRACULA) {
        LocationID myTrail[TRAIL_SIZE];
        giveMeTheTrail(currentView, player, myTrail);
        cur = myTrail[1];
    }
    return cur;
}

// Get the most recent move of a given player
void lastMove(DracView currentView, PlayerID player,
                 LocationID *start, LocationID *end)
{
    // TODO test this function
    LocationID trail[TRAIL_SIZE];
    getHistory(currentView->game, player, trail);
    *start = trail[1];
    *end = trail[0];
}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                         int *numTraps, int *numVamps)
{
    *numTraps = 0;
    *numVamps = 0;

    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        if (currentView->encounters[i].loc == where) {
            if (currentView->encounters[i].encounterType == TRAP) {
                (*numTraps)++;
            } else if (currentView->encounters[i].encounterType == VAMPIRE) {
                (*numVamps)++;
            }
        }
    }
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(DracView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    getHistory(currentView->game, player, trail);
}

//// Functions that query the map to find information about connectivity

// What are my (Dracula's) possible next moves (locations)
LocationID *whereCanIgo(DracView currentView, int *numLocations, int road, int sea)
{
    *numLocations = 0;
    int roundNum = getRound(currentView->game);
    LocationID loc = getLocation(currentView->game, PLAYER_DRACULA);

    LocationID myTrail[TRAIL_SIZE];
    giveMeTheTrail(currentView, PLAYER_DRACULA, myTrail);

    if (loc == DOUBLE_BACK_1)
        loc = myTrail[1];
    else if (loc == DOUBLE_BACK_2)
        loc = myTrail[2];
    else if (loc == DOUBLE_BACK_3)
        loc = myTrail[3];
    else if (loc == DOUBLE_BACK_4)
        loc = myTrail[4];
    else if (loc == DOUBLE_BACK_5)
        loc = myTrail[5];

    if (loc == UNKNOWN_LOCATION)
        loc = myTrail[1];

    int numConnections = 0;
    LocationID *connected = connectedLocations(currentView->game, &numConnections, loc, PLAYER_DRACULA,roundNum,road,0,sea);

    int i, j = 0;
    LocationID *valid = NULL;
    for (i = 0; i < numConnections; i++) {
        if (validPlace(connected[i]) && connected[i] != ST_JOSEPH_AND_ST_MARYS) {
            if (!isLocOnTrail(myTrail, connected[i])) {
                valid = realloc(valid, (j+1)*sizeof(LocationID));
                valid[j++] = connected[i];
                (*numLocations)++;
            }
        }
    }
    free(connected);
    return valid;
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(DracView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    if (player == PLAYER_DRACULA)
        return whereCanIgo (currentView, numLocations, road, sea);

    int roundNum = getRound(currentView->game);
    LocationID loc = getLocation(currentView->game, player);
    return connectedLocations(currentView->game, numLocations, loc, player,roundNum,road,rail,sea);
}


/*returns TRUE if locations have a connection of the type supplied if it is a valid move this turn */

int doLocationsHaveConnection (DracView currentView, LocationID from, LocationID to, TransportID type)
{
    fprintf(stderr,"*****In DracView.c*******");
    return locHaveConnection(currentView->game, PLAYER_DRACULA, from, to, type);
}

int isLocOnTrail (LocationID *myTrail, LocationID loc)
{
    if (!validPlace(loc)) {
        fprintf(stderr, "DracView.c:isLocOnTrail(): loc is invalid (%d)", loc);
        return TRUE;
    }
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        if (myTrail[i] == loc) {
            return TRUE;
        }
    }
    return FALSE;
}


int canHide(LocationID *myTrail, LocationID currLoc)
{
    if (currLoc == UNKNOWN_LOCATION) return FALSE;
    else if (validPlace (currLoc) && isSea(currLoc)) return FALSE;
    else if (currLoc == SEA_UNKNOWN) return FALSE;

    //fprintf(stderr, "Now I'm Here!\n");

    int i;
    // can hide if no adjacent Locations in trail are the same
    for (i = 0; i < TRAIL_SIZE-1; i++) {
        fprintf(stderr, "myTrail[%d].... %d\n", i, myTrail[i]);
        if (myTrail[i] == UNKNOWN_LOCATION && validPlace(myTrail[i+1]))
            return FALSE;

    }

    return TRUE;


}

int canDoubleBack(LocationID *myTrail, LocationID currLoc)
{

    int i;
    int counts[NUM_MAP_LOCATIONS];
    for (i = 0; i < NUM_MAP_LOCATIONS; i++)
        counts[i] = UNKNOWN;

    // can double back if no two locations in the trail are the same
    for (i = 0; i < TRAIL_SIZE; i++) {
        counts[myTrail[i]]++;
        if (counts[myTrail[i]] > DB_THRESH) {
            return TRUE;
        }
    }

    return FALSE;
}

int isHunterInLoc (DracView gameState, LocationID loc)
{
    int i;

    for (i = 0; i < NUM_PLAYERS-1; i++)
        if (loc == whereIs(gameState, i))
            return TRUE;

    return FALSE;
}

LocationID *getDracShortestPath (DracView currentView, LocationID from,
                                 LocationID to, int *j, int road, int boat)
{
    Link pathData;
    pathData = shortestPath(currentView->game, from, to, PLAYER_DRACULA,
                            road, 0, boat);
    // working backwards from destination
    LocationID cur = to;
    *j = 0;
    LocationID *path = malloc((*j+1)*sizeof(LocationID));
    path[(*j)++] = cur;
    while (getLinkLoc(pathData, cur) != from) {
        path = realloc(path, (*j+1)*sizeof(LocationID));
        path[(*j)++] = cur = getLinkLoc(pathData, cur);
    }
    free(pathData);
    // path is obtained in reverse order and must be flipped
    flipPath(path, *j-1);
    #ifdef DEBUG
        printf("path: %d ---> %d\n", from, to);
        printList("DracView.c: getShortestPath(): path", path, *j);
    #endif
    return path;
}
