// HunterView.c ... HunterView ADT implementation

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h" 

struct hunterView {
    int player;
    GameView game;
};

// Creates a new HunterView to summarise the current state of the game
HunterView newHunterView(char *pastPlays, PlayerMessage messages[])
{
    HunterView view = calloc(1, sizeof(struct hunterView));
    view->game = newGameView(pastPlays, messages);
    view->player = getCurrentPlayer(view->game);
    return view;
}
     
     
// Frees all memory previously allocated for the HunterView toBeDeleted
void disposeHunterView(HunterView toBeDeleted)
{
    disposeGameView(toBeDeleted->game);

    free(toBeDeleted);
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(HunterView currentView)
{
    return getRound(currentView->game);
}

// Get the id of current player
PlayerID whoAmI(HunterView currentView)
{
    return currentView->player;
}

// Get the current score
int giveMeTheScore(HunterView currentView)
{
    return getScore(currentView->game);
}

// Get the current health points for a given player
int howHealthyIs(HunterView currentView, PlayerID player)
{
    return getHealth(currentView->game, player);
}

// Get the current location id of a given player
LocationID whereIs(HunterView currentView, PlayerID player)
{
    return getLocation(currentView->game, player);
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(HunterView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    return getHistory(currentView->game, player, trail);
}

//// Functions that query the map to find information about connectivity

// What are my possible next moves (locations)
LocationID *whereCanIgo(HunterView currentView, int *numLocations, int road, int rail, int sea)
{
    return whereCanTheyGo(currentView, numLocations, currentView->player, road, rail, sea);
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(HunterView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    return connectedLocations(currentView->game, numLocations, whereIs(currentView, player), player, giveMeTheRound(currentView), road, rail, sea);
}

// returns TRUE if locations have a connection of the type supplied if it is a valid move this turn
int doLocationsHaveConnection (HunterView currentView, PlayerID player, LocationID from, LocationID to, TransportID type)
{
    
    return locHaveConnection(currentView->game, player, from, to, type);
}

LocationID *getHuntShortestPath (HunterView currentView, LocationID from,
                             LocationID to, int *j, PlayerID player, 
                             int road, int rail, int boat)
{
    Link pathData = shortestPath(currentView->game, from, to, player, road, 
                                 rail, boat);
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
        printList("HunterView.c: getShortestPath(): path", path, *j);
    #endif
    return path;
}
