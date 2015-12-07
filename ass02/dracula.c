// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "DracView.h"
#include "Places.h"

#define ABREV_LOCATIONS {"AS", "AL", "AM", "AT", "AO", "BA", "BI", \
                         "BB", "BE", "BR", "BS", "BO", "BU", "BC", \
                         "BD", "CA", "CG", "CD", "CF", "CO", "CN", \
                         "DU", "ED", "EC", "FL", "FR", "GA", "GW", \
                         "GE", "GO", "GR", "HA", "IO", "IR", "KL", \
                         "LE", "LI", "LS", "LV", "LO", "MA", "MN", \
                         "MR", "MS", "MI", "MU", "NA", "NP", "NS", \
                         "NU", "PA", "PL", "PR", "RO", "SA", "SN", \
                         "SR", "SJ", "SO", "JM", "ST", "SW", "SZ", \
                         "TO", "TS", "VA", "VR", "VE", "VI", "ZA", "ZU" }
#define TELE_DRAC "TP"
#define HIDE_DRAC "HI"
#define DB_THRESH 1
#define RANDOM_LOC rand() % numLocations
;
#define NUM_HUNTERS 4
// our function prototypes

static void whereTheyMayGo (DracView game, int locs[NUM_MAP_LOCATIONS]);
static char * decideDoubleBack(char *bestPlay, int *hunterAdj, LocationID hist[TRAIL_SIZE]);
static void printTrail (LocationID hist[TRAIL_SIZE]);

void decideDraculaMove(DracView gameState)
{

    int i;

    // information collection
    LocationID myLocation = whereIs(gameState, PLAYER_DRACULA);

    // array that contains abreviated location strings
    char *abbrevLoc[NUM_MAP_LOCATIONS] = ABREV_LOCATIONS;
    LocationID myTrail[TRAIL_SIZE];
    giveMeTheTrail(gameState, PLAYER_DRACULA, myTrail);

    // fix up current location
    if (myLocation >= DOUBLE_BACK_1 && myLocation <= DOUBLE_BACK_5)
        myLocation = myTrail[myLocation - DOUBLE_BACK_1 + 1];

    // change curr loc in trail !!!DON'T DO THIS HERE!!!
    //myTrail[0] = myLocation;

    fprintf(stderr, "I'm at %d\n", myLocation);

    int round = giveMeTheRound(gameState);

    //int myHealth = howHealthyIs(gameState, PLAYER_DRACULA);


    // get available moves
    int numLocations = 0;
    LocationID *availMoves = whereCanIgo(gameState, &numLocations, 1, 0);
    if (numLocations == 0)
        availMoves = whereCanIgo(gameState, &numLocations, 0, 1);

    fprintf(stderr, "Places to move to = %d\n", numLocations);


	// intial heuristic
    char *backup = NULL;
    char *bestPlay = NULL;

    // get where any of the hunters could move to
    int hunterAdj[NUM_MAP_LOCATIONS];
    whereTheyMayGo (gameState, hunterAdj);

    /* double_back && hide hide heuristic */
    // possible add condition that hunters do not know any of dracs
    // trail to do this
    if (round) {
        if (canHide(myTrail, myLocation) && !hunterAdj[myLocation])
            bestPlay = HIDE_DRAC;
        //else if (canDoubleBack(myTrail, myLocation))
        //    decideDoubleBack(bestPlay, hunterAdj);
    // round 0: Drac can start anywhere
    } else {

        if (isHunterInLoc(gameState, STRASBOURG) == FALSE)
            bestPlay = abbrevLoc[STRASBOURG];
        else if (isHunterInLoc(gameState, SZEGED) == FALSE)
            bestPlay = abbrevLoc[SZEGED];
        else if (isHunterInLoc(gameState, NUREMBURG) == FALSE)
            bestPlay = abbrevLoc[NUREMBURG];
        else if (isHunterInLoc(gameState, MUNICH) == FALSE)
            bestPlay = abbrevLoc[MUNICH];
        else if (isHunterInLoc(gameState, BELGRADE) == FALSE)
            bestPlay = abbrevLoc[BELGRADE];
    } 

    if (numLocations > 0 && bestPlay == NULL) {
	    for (i = 0; i < numLocations; i++) {
	        if (!validPlace(availMoves[i])) {
	        	#ifdef DEBUG
                fprintf(stderr, "dracula.c: availMoves[%d] = %d && is not a valid place", i, availMoves[i]);
                #endif
	            continue;
	        } else if (!isHunterInLoc(gameState, availMoves[i]) &&
             availMoves[i] != myLocation && !isLocOnTrail(myTrail, availMoves[i])) {
                // check the location has a connection to myLocation
                if (doLocationsHaveConnection(gameState, myLocation, availMoves[i], ANY)) {
	                bestPlay = abbrevLoc[availMoves[i]];
	                // only stop looking if a hunter can't move to that location in their next move
                    if (hunterAdj[i] == FALSE)
                        break;
                    else
                        backup = bestPlay;
	            }

	        }
	    }
	}

	// go to last move adjacent to a hunter
	if (bestPlay == NULL) bestPlay = backup;

    if (bestPlay == NULL && canDoubleBack(myTrail, myLocation)) {
        bestPlay = decideDoubleBack(bestPlay, hunterAdj, myTrail);
    }


	// if still NULL, then no available moves and must teleport
	if (backup == NULL && bestPlay == NULL) {
	    bestPlay = TELE_DRAC;

    printTrail(myTrail);

	    for (i = 0; i < numLocations; i++) {
	        fprintf(stderr, "availMoves[%d]:......... %d\n", i, availMoves[i]);
	    }
	    fprintf(stderr, "numLocations:......... %d\n", numLocations);
	}
	registerBestPlay(bestPlay,"message");
}

// fills the array 'locs' with TRUE in all array positions corresponding
// to where any of the hunters could possibly move to in their next move
// the rest of the array elements are filled with FALSE
static void whereTheyMayGo (DracView game, int locs[NUM_MAP_LOCATIONS])
{
    // fill the array with FALSE
    int i;
    for (i = 0; i < NUM_MAP_LOCATIONS; i++)
        locs[i] = FALSE;

    // fill it with the actually data for each hunter
    int player = 0;
    while (player < PLAYER_DRACULA) {
        // create the array and fill it with where that hunter can go
        int numMapLocs = NUM_MAP_LOCATIONS;
        LocationID *playersLocs;
        playersLocs = whereCanTheyGo(game, &numMapLocs, player, TRUE, TRUE, TRUE);

        // transfer the data from one array to the other
        i = 0;
        while (i < NUM_MAP_LOCATIONS && playersLocs[i] >= MIN_MAP_LOCATION
        && playersLocs[i] <= MAX_MAP_LOCATION)
            locs[playersLocs[i++]] = TRUE;
        player++;
    }
}


static char * decideDoubleBack(char *bestPlay, int *hunterAdj, LocationID hist[TRAIL_SIZE])
{
    if (hist[1] != NOWHERE) {
        if (!hunterAdj[hist[1]]) {
            bestPlay = "D2";
        }
    } else if (hist[0] != NOWHERE) {
        if (!hunterAdj[hist[0]]) {
            bestPlay = "D1";
        }
    } else if (hist[2] != NOWHERE) {
        if(!hunterAdj[hist[2]]) {
        bestPlay = "D3";
        }
    } else if (hist[3] != NOWHERE) {
        if (!hunterAdj[hist[3]]) {
	    bestPlay = "D4";
        }
	} else if (hist[4] != NOWHERE) {
        if (!hunterAdj[hist[4]]) {
	    bestPlay = "D5";
        }
	}
    return bestPlay;
}

static void printTrail (LocationID hist[TRAIL_SIZE])
{
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        fprintf(stderr, "Trail[%d] = %d\n", i, hist[i]);
    }
}