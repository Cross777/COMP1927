// testHunterView.c ... test the HunterView ADT

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "HunterView.h"
#include "GameView.h"
 

int main()
{
    int i;
    HunterView hv;
    
    printf("Test basic empty initialisation\n");
    PlayerMessage messages1[] = {};
    hv = newHunterView("", messages1);
    assert(whoAmI(hv) == PLAYER_LORD_GODALMING);
    assert(giveMeTheRound(hv) == 0);
    assert(howHealthyIs(hv,PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
    assert(howHealthyIs(hv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
    assert(giveMeTheScore(hv) == GAME_START_SCORE);
    assert(whereIs(hv,PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);
    printf("passed\n");
    disposeHunterView(hv);

    printf("Test for Dracula trail and basic functions\n");
    PlayerMessage messages2[] = {"Hello","Rubbish","Stuff","","Mwahahah"};
    hv = newHunterView("GST.... SAO.... HZU.... MBB.... DC?....", messages2);
    assert(whoAmI(hv) == PLAYER_LORD_GODALMING);
    assert(giveMeTheRound(hv) == 1);
    assert(whereIs(hv,PLAYER_LORD_GODALMING) == STRASBOURG);
    assert(whereIs(hv,PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
    assert(whereIs(hv,PLAYER_VAN_HELSING) == ZURICH);
    assert(whereIs(hv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
    assert(whereIs(hv,PLAYER_DRACULA) == CITY_UNKNOWN);
    assert(howHealthyIs(hv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
    printf("passed\n");
    disposeHunterView(hv);

    printf("Test for encountering Dracula and hunter history\n");
    PlayerMessage messages3[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!"};
    hv = newHunterView("GST.... SAO.... HCD.... MAO.... DGE.... GGED...", messages3);
    assert(whereIs(hv,PLAYER_DRACULA) == GENEVA);
    assert(howHealthyIs(hv,PLAYER_LORD_GODALMING) == 5);
    assert(howHealthyIs(hv,PLAYER_DRACULA) == 30);
    assert(whereIs(hv,PLAYER_LORD_GODALMING) == GENEVA);
    LocationID history[TRAIL_SIZE];
    giveMeTheTrail(hv,PLAYER_DRACULA,history);
    assert(history[0] == GENEVA);
    assert(history[1] == UNKNOWN_LOCATION);
    giveMeTheTrail(hv,PLAYER_LORD_GODALMING,history);
    assert(history[0] == GENEVA);
    assert(history[1] == STRASBOURG);
    assert(history[2] == UNKNOWN_LOCATION);
    giveMeTheTrail(hv,PLAYER_DR_SEWARD,history);
    assert(history[0] == ATLANTIC_OCEAN);
    assert(history[1] == UNKNOWN_LOCATION);
    printf("passed\n");        
    disposeHunterView(hv);

    printf("Test for Dracula doubling back at sea, and losing blood points (Hunter View)\n");
    PlayerMessage messages4[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!","","","","Back I go"};
    hv = newHunterView("GGE.... SGE.... HGE.... MGE.... DS?.... "
                       "GST.... SST.... HST.... MST.... DD1....", messages4);
    assert(whereIs(hv,PLAYER_DRACULA) == DOUBLE_BACK_1);
    giveMeTheTrail(hv,PLAYER_DRACULA,history);
    assert(history[0] == DOUBLE_BACK_1);
    assert(history[1] == SEA_UNKNOWN);
    assert(howHealthyIs(hv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4);
    assert(whoAmI(hv) == 0);
    printf("passed\n");
    disposeHunterView(hv);

    printf("Test for connections\n");
    int size, seen[NUM_MAP_LOCATIONS], *edges;

    printf("Checking Galatz road connections\n");
    PlayerMessage messages5[] = {"Gone to Galatz"};
    hv = newHunterView("GGA....", messages5);    
    edges = whereCanTheyGo(hv,&size,PLAYER_LORD_GODALMING,1,0,0);
    memset(seen, 0, NUM_MAP_LOCATIONS*sizeof(int));
    for (i = 0; i< size ; i++) seen[edges[i]] = 1;
    assert(size == 5); assert(seen[GALATZ]); assert(seen[CONSTANTA]);
    assert(seen[BUCHAREST]); assert(seen[KLAUSENBURG]); assert(seen[CASTLE_DRACULA]);
    free(edges);
    disposeHunterView(hv);

    printf("Checking Ionian Sea sea connections\n");
    PlayerMessage messages6[] = {"Sailing the Ionian"};
    hv = newHunterView("GIO....", messages6);
    edges = whereCanTheyGo(hv,&size,PLAYER_LORD_GODALMING,0,0,1);
    memset(seen, 0, NUM_MAP_LOCATIONS*sizeof(int));
    for (i = 0; i < size; i++) seen[edges[i]] = 1;
    assert(size == 7); assert(seen[IONIAN_SEA]); assert(seen[BLACK_SEA]);
    assert(seen[ADRIATIC_SEA]); assert(seen[TYRRHENIAN_SEA]);
    assert(seen[ATHENS]); assert(seen[VALONA]); assert(seen[SALONICA]);
    free(edges);
    disposeHunterView(hv);

    printf("Checking Athens rail connections (none)\n");
    PlayerMessage messages7[] = {"Leaving Athens by train"};
    hv = newHunterView("GAT....", messages7);
    edges = whereCanTheyGo(hv,&size,PLAYER_LORD_GODALMING,0,1,0);
    assert(size == 1);
    assert(edges[0] == ATHENS);
    free(edges);
    disposeHunterView(hv);
    
    /* doLocationsHaveConnections */
    printf("testing...... doLocationsHaveConnections()\n");  
    PlayerMessage messages8[] = {"testing connections"};
    hv = newHunterView("GGE.... SGE.... HGE.... MGE.... DS?.... "
                       "GGE....", messages8);
    PlayerID p = PLAYER_LORD_GODALMING; // railMove == 2
    // futurePlays for testing multiple hunters/ railMoves
    // ROAD tests
    assert(doLocationsHaveConnection(hv, p, PARIS, BRUSSELS, ROAD));
    assert(doLocationsHaveConnection(hv, p, MANCHESTER, LONDON, ROAD));
    assert(doLocationsHaveConnection(hv, p, ATHENS, VALONA, ROAD));    
    assert(!doLocationsHaveConnection(hv, p, MUNICH, BRUSSELS, ROAD));
    assert(!doLocationsHaveConnection(hv, p, VIENNA, BERLIN, ROAD));
    assert(!doLocationsHaveConnection(hv, p, SOFIA, ST_JOSEPH_AND_ST_MARYS,
                                      ROAD)); 
    // RAIL tests w railMove == 2   
    printf("testing...... railMove == 2?\n");
    assert(doLocationsHaveConnection(hv, p, HAMBURG, PRAGUE, RAIL));
    assert(doLocationsHaveConnection(hv, p, ROME, BARI, RAIL));
    assert(doLocationsHaveConnection(hv, p, PARIS, MARSEILLES, RAIL));    
    assert(!doLocationsHaveConnection(hv, p, MADRID, PARIS, RAIL));
    assert(!doLocationsHaveConnection(hv, p, LONDON, BRUSSELS, RAIL));
    assert(!doLocationsHaveConnection(hv, p, ATHENS, VALONA, RAIL)); 
    disposeHunterView(hv);
    printf("passed\n");
    
    /* getShortestPath test */
    printf("testing......... shortestPath\n");
    PlayerMessage messages9[] = {"testing shortestPaths"};
    hv = newHunterView("GGE....", messages9);
    p = PLAYER_DR_SEWARD;
    LocationID *path = NULL;
    int pathLength;
    printf("Madrid -> Paris: All\n"); 
    path = getHuntShortestPath(hv, MADRID, PARIS, &pathLength,p,1,1,1); 
    free(path);
    printf("Edinburgh -> Athens: All\n"); 
    path = getHuntShortestPath(hv, EDINBURGH, ATHENS, &pathLength,p,1,1,1);
    free(path);
    printf("Amsterdam -> Athens: Road && Rail\n"); 
    path = getHuntShortestPath(hv, AMSTERDAM, ATHENS, &pathLength,p,1,1,0);
    free(path);
    printf("Madrid -> Varna: Road only\n"); 
    path = getHuntShortestPath(hv, MADRID, VARNA, &pathLength,p,1,0,0);  
    free(path);
    printf("Castle Dracula -> Paris: All\n"); 
    path = getHuntShortestPath(hv, CASTLE_DRACULA, PARIS, &pathLength,p,1,1,1);  
    free(path);    
    disposeHunterView(hv);
    return 0;
}

