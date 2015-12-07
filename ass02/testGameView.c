// testGameView.c ... test the GameView ADT

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "GameView.h"

#define PROFIT 1
#define LOCATIONS 30
#define LONDON_ALL { EDINBURGH, ENGLISH_CHANNEL, LIVERPOOL, LONDON, \
                     MANCHESTER, PLYMOUTH, SWANSEA }
#define SIZE_L 7
#define STRASBOURG_ALL { BRUSSELS, COLOGNE, FRANKFURT, GENEVA, MUNICH, \
                         NUREMBURG, PARIS, STRASBOURG, ZURICH }
#define SIZE_S 9
                        
typedef struct _link *Link;    

int testLocalConnections(GameView gv);

int main()
{
    int i;
    GameView gv;

    printf("Test basic empty initialisation\n");
    PlayerMessage messages1[] = {};
    gv = newGameView("", messages1);
    assert(getCurrentPlayer(gv) == PLAYER_LORD_GODALMING);
    assert(getRound(gv) == 0);
    assert(getHealth(gv,PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(gv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
    assert(getScore(gv) == GAME_START_SCORE);
    assert(getLocation(gv,PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);
    printf("passed\n");
    /* test shortestPath */
    printf("testing shortest Path\n");
    Link result = shortestPath(gv,TOULOUSE,PARIS,PLAYER_DRACULA,1,0,0);
    printLinkList("result", result, NUM_MAP_LOCATIONS);
    free(result);
    /* testLocalConnection */
    assert(testLocalConnections(gv) == PROFIT);
    disposeGameView(gv);

    printf("Test for Dracula trail and basic functions\n");

    PlayerMessage messages2[] = {"Hello","Rubbish","Stuff","","Mwahahah"};
    gv = newGameView("GST.... SAO.... HZU.... MBB.... DC?....", messages2);
    assert(getCurrentPlayer(gv) == PLAYER_LORD_GODALMING);
    assert(getRound(gv) == 1);
    
    assert(getLocation(gv,PLAYER_LORD_GODALMING) == STRASBOURG);
    assert(getLocation(gv,PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
    assert(getLocation(gv,PLAYER_VAN_HELSING) == ZURICH);
    assert(getLocation(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
    assert(getLocation(gv,PLAYER_DRACULA) == CITY_UNKNOWN);
    assert(getHealth(gv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
    printf("passed\n");
    disposeGameView(gv);

    printf("Test for encountering Dracula and hunter history\n");
    PlayerMessage messages3[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!"};
    gv = newGameView("GST.... SAO.... HCD.... MAO.... DGE.... GGED...", messages3);

    assert(getRound(gv) == 1);
    assert(getLocation(gv,PLAYER_DRACULA) == GENEVA);
    assert(getHealth(gv,PLAYER_LORD_GODALMING) == 5);
    assert(getHealth(gv,PLAYER_DRACULA) == 30);
    assert(getLocation(gv,PLAYER_LORD_GODALMING) == GENEVA);
    LocationID history[TRAIL_SIZE];
    getHistory(gv,PLAYER_DRACULA,history);
    assert(history[0] == GENEVA);
    assert(history[1] == UNKNOWN_LOCATION);
    getHistory(gv,PLAYER_LORD_GODALMING,history);
    assert(history[0] == GENEVA);
    assert(history[1] == STRASBOURG);
    assert(history[2] == UNKNOWN_LOCATION);
    getHistory(gv,PLAYER_DR_SEWARD,history);
    assert(history[0] == ATLANTIC_OCEAN);
    assert(history[1] == UNKNOWN_LOCATION);
    printf("passed\n");
    disposeGameView(gv);

    printf("Test for Dracula doubling back at sea, and losing blood points (Hunter View)\n");
    PlayerMessage messages4[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!","","","","Back I go"};
    gv = newGameView("GGE.... SGE.... HGE.... MGE.... DS?.... "
                     "GST.... SST.... HST.... MST.... DD1....", messages4);
    assert(getRound(gv) == 2);
    assert(getLocation(gv,PLAYER_DRACULA) == DOUBLE_BACK_1);
    getHistory(gv,PLAYER_DRACULA,history);
    assert(history[0] == DOUBLE_BACK_1);
    assert(history[1] == SEA_UNKNOWN);
    assert(getHealth(gv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4);
    assert(getCurrentPlayer(gv) == 0);
    printf("passed\n");
    disposeGameView(gv);

    printf("Test for Dracula doubling back at sea, and losing blood points (Drac View)\n");
    PlayerMessage messages5[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!","","","","Back I go"};
    gv = newGameView("GGE.... SGE.... HGE.... MGE.... DEC.... "
                     "GST.... SST.... HST.... MST.... DD1....", messages5);
    assert(getLocation(gv,PLAYER_DRACULA) == DOUBLE_BACK_1);
    getHistory(gv,PLAYER_DRACULA,history);
    assert(history[0] == DOUBLE_BACK_1);
    assert(history[1] == ENGLISH_CHANNEL);
    assert(getHealth(gv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4);
    assert(getCurrentPlayer(gv) == 0);
    printf("passed\n");
    disposeGameView(gv);


    // Tests by Alvin Prijatna
    printf("Test for Dracula doubling back x2 back at sea, and losing blood points (unknown locations)\n");
    PlayerMessage messages6[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!","","","","Back I go"};
    gv = newGameView("GGE.... SGE.... HGE.... MGE.... DS?.... "
                     "GST.... SST.... HST.... MST.... DC?.... "
                     "GGE.... SGE.... HGE.... MGE.... DD2....", messages6);
    assert(getLocation(gv,PLAYER_DRACULA) == DOUBLE_BACK_2);
    getHistory(gv,PLAYER_DRACULA,history);
    assert(history[0] == DOUBLE_BACK_2);
    assert(history[1] == CITY_UNKNOWN);
    assert(history[2] == SEA_UNKNOWN);
    assert(getHealth(gv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4);
    assert(getCurrentPlayer(gv) == 0);
    printf("passed\n");
    disposeGameView(gv);

    printf("Test for Dracula doubling back x3 back at sea, and losing blood points (unknown locations)\n");
    PlayerMessage messages7[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!","","","","Back I go"};
    gv = newGameView("GGE.... SGE.... HGE.... MGE.... DS?.... "
                     "GST.... SST.... HST.... MST.... DC?.... "
                     "GST.... SST.... HST.... MST.... DS?.... "
                     "GGE.... SGE.... HGE.... MGE.... DD3....", messages7);
    assert(getLocation(gv,PLAYER_DRACULA) == DOUBLE_BACK_3);
    getHistory(gv,PLAYER_DRACULA,history);
    assert(history[0] == DOUBLE_BACK_3);
    assert(history[1] == SEA_UNKNOWN);
    assert(history[2] == CITY_UNKNOWN);
    assert(history[3] == SEA_UNKNOWN);
    assert(getHealth(gv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 6);
    assert(getCurrentPlayer(gv) == 0);
    printf("passed\n");
    disposeGameView(gv);


    printf("Test for Dracula doubling back x4 back at sea, and losing blood points (unknown locations)\n");
    PlayerMessage messages8[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!","","","","Back I go"};
    gv = newGameView("GGE.... SGE.... HGE.... MGE.... DS?.... "
                     "GST.... SST.... HST.... MST.... DC?.... "
                     "GST.... SST.... HST.... MST.... DS?.... "
                     "GST.... SST.... HST.... MST.... DC?.... "
                     "GGE.... SGE.... HGE.... MGE.... DD4....", messages8);
    assert(getLocation(gv,PLAYER_DRACULA) == DOUBLE_BACK_4);
    getHistory(gv,PLAYER_DRACULA,history);
    assert(history[0] == DOUBLE_BACK_4);
    assert(history[1] == CITY_UNKNOWN);
    assert(history[2] == SEA_UNKNOWN);
    assert(history[3] == CITY_UNKNOWN);
    assert(history[4] == SEA_UNKNOWN);
    assert(getHealth(gv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 6);
    assert(getCurrentPlayer(gv) == 0);
    printf("passed\n");
    disposeGameView(gv);


    printf("Test for Dracula doubling back x5 back at sea, and losing blood points (unknown locations) "
            "and capping of hunter health\n");
    PlayerMessage messages9[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!","","","","Back I go"};
    gv = newGameView("GGE.... SGE.... HGE.... MGE.... DS?.... "
                     "GST.... SST.... HST.... MST.... DC?.... "
                     "GST.... SST.... HST.... MST.... DS?.... "
                     "GST.... SST.... HST.... MST.... DC?.... "
                     "GST.... SST.... HST.... MST.... DS?.... "
                     "GGE.... SGE.... HGE.... MGE.... DD5....", messages9);
    assert(getLocation(gv,PLAYER_DRACULA) == DOUBLE_BACK_5);
    getHistory(gv,PLAYER_DRACULA,history);
    assert(history[0] == DOUBLE_BACK_5);
    assert(history[1] == SEA_UNKNOWN);
    assert(history[2] == CITY_UNKNOWN);
    assert(history[3] == SEA_UNKNOWN);
    assert(history[4] == CITY_UNKNOWN);
    assert(history[5] == SEA_UNKNOWN);
    assert(getHealth(gv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 8);
    assert(getHealth(gv,PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(gv,PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(gv,PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(gv,PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getCurrentPlayer(gv) == 0);
    printf("passed\n");
    disposeGameView(gv);
    // End of Tests by Alvin Prijatna

    printf("Test for connections\n");
    int size, seen[NUM_MAP_LOCATIONS], *edges;
    gv = newGameView("", messages1);
    printf("Checking Galatz road connections\n");
    edges = connectedLocations(gv,&size,GALATZ,PLAYER_LORD_GODALMING,0,1,0,0);
    memset(seen, 0, NUM_MAP_LOCATIONS*sizeof(int));
    for (i = 0; i< size ; i++) seen[edges[i]] = 1;
    assert(size == 5); assert(seen[GALATZ]); assert(seen[CONSTANTA]);
    assert(seen[BUCHAREST]); assert(seen[KLAUSENBURG]); assert(seen[CASTLE_DRACULA]);
    free(edges);
    printf("Checking Strasbourg road connections\n");
    edges = connectedLocations(gv,&size,STRASBOURG,PLAYER_LORD_GODALMING,0,1,0,0);
    memset(seen, 0, NUM_MAP_LOCATIONS*sizeof(int));
    for (i = 0; i< size ; i++) seen[edges[i]] = 1;
    assert(size == 9); assert(seen[MUNICH]); assert(seen[FRANKFURT]);
    assert(seen[NUREMBURG]); assert(seen[ZURICH]); assert(seen[BRUSSELS]);
    assert(seen[PARIS]); assert(seen[GENEVA]); assert(seen[STRASBOURG]);
    assert(seen[COLOGNE]);
    free(edges);
    printf("Checking Ionian Sea sea connections\n");
    edges = connectedLocations(gv,&size,IONIAN_SEA,PLAYER_LORD_GODALMING,0,0,0,1);
    memset(seen, 0, NUM_MAP_LOCATIONS*sizeof(int));
    for (i = 0; i < size; i++) seen[edges[i]] = 1;
    if(size != 7) { printf("size: %d\n", size); abort(); } assert(seen[IONIAN_SEA]); assert(seen[BLACK_SEA]);
    assert(seen[ADRIATIC_SEA]); assert(seen[TYRRHENIAN_SEA]);
    assert(seen[ATHENS]); assert(seen[VALONA]); assert(seen[SALONICA]);
    free(edges);
    printf("Checking Athens rail connections (none)\n");
    edges = connectedLocations(gv,&size,ATHENS,PLAYER_LORD_GODALMING,0,0,1,0);
    assert(size == 1);
    assert(edges[0] == ATHENS);
    free(edges);
    printf("passed\n");
    disposeGameView(gv);
    return 0;
}

int testLocalConnections(GameView gv)
{
    fprintf (stdout, "testing: localConnections()\n");
    LocationID *expected;
    LocationID *result;

    /**** Test Case: Frankfurt
          Hunters - rail only  ****/
    fprintf (stdout, "testing: Hunters: RAIL only\n");
    LocationID loc = FRANKFURT;
    int i;
    int player, round, railMove;
    int size, size_result;

    for (i = PLAYER_LORD_GODALMING; i < NUM_PLAYERS - 1; i++) {
        player = i;
        round = 1;
        railMove = ((round + player) % 4);
        // expected results
        if (railMove == 0) {
            size = 1;
            expected = malloc(size*sizeof(LocationID));
            expected[0] = FRANKFURT;
            
        } else if (railMove == 1) {
           size = 4;
           expected = malloc(size*sizeof(LocationID));
           expected[0] = COLOGNE;       expected[1] = FRANKFURT;
           expected[2] = LEIPZIG;       expected[3] = STRASBOURG;
           
        } else if (railMove == 2) {
            size = 8;
            expected = malloc(size*sizeof(LocationID));
            expected[0] = BERLIN;       expected[1] = BRUSSELS;
            expected[2] = COLOGNE;      expected[3] = FRANKFURT;
            expected[4] = LEIPZIG;      expected[5] = NUREMBURG;
            expected[6] = STRASBOURG;   expected[7] = ZURICH;
            
        } else if (railMove == 3) {
            size = 13;
            expected = malloc(size*sizeof(LocationID));
            expected[0] = BERLIN;       expected[1] = BRUSSELS;
            expected[2] = COLOGNE;      expected[3] = FRANKFURT;
            expected[4] = HAMBURG;      expected[5] = LEIPZIG;
            expected[6] = MILAN;        expected[7] = MUNICH;
            expected[8] = NUREMBURG;    expected[9] = PARIS;
            expected[10] = PRAGUE;      expected[11] = STRASBOURG;
            expected[12] = ZURICH;
            
        } else if (railMove > 3) {
            fprintf(stderr, "testLocalLocations: railMove > 3\n");
            abort();
        }
        
        size_result = 0;
        result = connectedLocations(gv,&size_result,loc,player,round,0,1,0);

        if (memcmp(result, expected, size) != 0) {
            printList("Actual Result", result, LOCATIONS);
            printList("Expected Result", expected, LOCATIONS);
            abort();
        } else {
            printf("RailMove %d: passed!\n", railMove);
        }
        if (size != size_result) {
            printf("*numLocations is incorrect.... \n");
            printf("correct:   %d\nactual:  %d\n", size, size_result);
            abort();
        } else {
	         printf("numLocations.............is correct\n");
        }
        free(expected);
        free(result);
    }
   
    printf("Hunters: RAIL only: tests passed!!!\n");

    /**** Testcase London
          Hunters - ALL  ****/

    fprintf (stdout, "testing: Hunters: ALL\n");

    // establish pre conditions
    loc = LONDON;
    round = 4;
    player = PLAYER_MINA_HARKER; // railMove == 3

    LocationID expectedLondon[SIZE_L] = LONDON_ALL;
        
    // get result and compare
    result = connectedLocations(gv,&size_result,loc,player,round,1,1,1);

    if (memcmp(result, expectedLondon, SIZE_L) != 0) {
        printList("Actual London Result", result, LOCATIONS);
        printList("Expected London Result", expectedLondon, LOCATIONS);
        abort();
    } else {
        fprintf (stdout, "Hunters: ALL: tests passed!!!\n");
    }
    if (SIZE_L != size_result) {
        printf("*num_locations is incorrect.... \n");
        printf("correct:   %d\nactual:  %d\n", SIZE_L, size_result);
        abort();
    }  else {
        printf("numLocations.............is correct\n");
    }
    free(result);
     
    fprintf (stdout, "testing: Dracula: Strasbourg\n");

    // establish pre conditions
    loc = STRASBOURG;
    round = 1;
    player = PLAYER_DRACULA; // railMove == 3
    
    size = SIZE_S;
    LocationID expectedS[SIZE_S] = STRASBOURG_ALL;
    // get result and compare
    result = connectedLocations(gv,&size_result,loc,player,round,1,0,0);

    if (memcmp(result, expectedS, SIZE_S) != 0) {
        printList("Actual Strasbourg Result", result, LOCATIONS);
        printList("Expected Strasbourg Result", expectedS, LOCATIONS);
        abort();
    } else {
        fprintf (stdout, "Hunters: Dracula: Strasbourg: tests passed!!!\n");
    }
    if (SIZE_S != size_result) {
        printf("*num_locations is incorrect.... \n");
        printf("correct:   %d\nactual:  %d\n", SIZE_S, size_result);
        abort();
    }  else {
        printf("numLocations.............is correct\n");
    }
    free(result);
    

    fprintf(stdout, "localConnections(): All tests passed!!!\n");
    return 1;
}

