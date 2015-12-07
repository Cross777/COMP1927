// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"



#define ABREV_LOCATIONS {"AS", "AL", "AM", "AT", "AO", "BA", "BI", \
                         "BB", "BE", "BR", "BS", "BO", "BU", "BC", \
                         "BD", "CA", "CG", "CD", "CF", "CO", "CN", \
                         "DU", "ED", "EC", "FL", "FR", "GA", "GW", \
                         "GE", "GO", "GR", "HA", "IO", "IR", "KL", \
                         "LE", "LI", "LS", "LV", "LO", "MA", "MN", \
                         "MR", "MS", "MI", "MU", "NA", "NP", "NS", \
                         "NU", "PA", "PL", "PR", "RO", "SA", "SN", \
                         "SR", "SJ", "SO", "JM", "ST", "SW", "SZ", \
                         "TO", "TS", "VA", "VR", "VE", "VI", "ZA", "ZU"}
#define RANDOM_LOC rand() % numLocations

//local fucntion prototypes
static int isHunterInLoc (HunterView gameState, LocationID loc);
static LocationID latestDracMove (HunterView gameState);

void decideHunterMove(HunterView gameState)
{
    // information collection
    PlayerID myPlayer = whoAmI(gameState);
    LocationID myLocation = whereIs(gameState, myPlayer);
    int round = giveMeTheRound(gameState);
    int myHealth = howHealthyIs(gameState, myPlayer);
    LocationID lastDracLoc = latestDracMove(gameState);
    
    
    // legal moves
    int numLocations = 0;
    LocationID *availMoves;
    int railMove = (myPlayer + giveMeTheRound(gameState)) % 4;
     
    // array that contains abreviated location strings
    char *abbrevLoc[NUM_MAP_LOCATIONS] = ABREV_LOCATIONS;
    
    // intial heuristic: best play is current location 
    // No longer needed***
    char *bestPlay = abbrevLoc[myLocation];
    // char *bestPlay;
    // round 0: location(s) can be anywhere   
    if (!round) {
        if (myPlayer == PLAYER_LORD_GODALMING) { 
            bestPlay = abbrevLoc[PARIS];
        } else if (myPlayer == PLAYER_DR_SEWARD) {
            bestPlay = abbrevLoc[BUCHAREST];
        } else if (myPlayer == PLAYER_VAN_HELSING) {
            bestPlay = abbrevLoc[MUNICH];
        } else if (myPlayer == PLAYER_MINA_HARKER) {
            bestPlay = abbrevLoc[SARAGOSSA];
        }
        registerBestPlay(bestPlay,"insert message here");   
    /* all hunters will research drac's starting location if no better 
       location is revealed || heal when in range of death by trap
       (Nick: I'm thinking moving around more is better than being 
        conservative with healing - if we hit drac and die it's worth it) */
    } else if ((round == 6 && lastDracLoc == UNKNOWN_LOCATION)  ||
                myHealth < 3) { 
        bestPlay = abbrevLoc[myLocation];       
    
    //} else if (latestDracMove(gameState) > UNKNOWN_LOCATION) {
        /* perhaps use shortest path or some other algorithm to move
           towards drac */    
    
    } else {
        /* best play is random valid move location (not sea) && 
           not occupied by any hunter */
        availMoves = whereCanIgo(gameState, &numLocations, 1, railMove, 0);
        do {
            bestPlay = abbrevLoc[availMoves[RANDOM_LOC]];
        } while (!isHunterInLoc(gameState, abbrevToID(bestPlay)));        
    } 
    
    if (round) {  
        if (doLocationsHaveConnection(gameState, myPlayer, myLocation, 
                                      abbrevToID(bestPlay), ANY)) {
            registerBestPlay(bestPlay,"insert message here");
        } else {
            registerBestPlay(abbrevLoc[myLocation],"insert message here");
        }  
    } 
 
}
    
static int isHunterInLoc (HunterView gameState, LocationID loc) 
{
    int i;
    for (i = 0; i < NUM_PLAYERS-1; i++) 
        if (loc == whereIs(gameState, i))
            return TRUE;

    return FALSE;
}  
    
static LocationID latestDracMove (HunterView gameState) 
{
    LocationID trail[TRAIL_SIZE];
    int i;
    for (i = 0; i < TRAIL_SIZE; i++)
        trail[i] = UNKNOWN_LOCATION;

    giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
    
    for (i = TRAIL_SIZE-1; i >= 0; i--) 
        if (trail[i] != UNKNOWN_LOCATION)
            return trail[i];
    
    return UNKNOWN_LOCATION;
}


