// Map.h ... interface to Map data type

#ifndef MAP_H
#define MAP_H

#include "Places.h"
#include "Globals.h"

#define MAX_CONNECTIONS 20

typedef struct edge{
    LocationID  start;
    LocationID  end;
    TransportID type;
} Edge;

// graph representation is hidden 
typedef struct MapRep *Map; 

// operations on graphs 
Map  newMap();  
void disposeMap(Map g); 
void showMap(Map g); 
int  numV(Map g);
int  numE(Map g, TransportID t);

/* Helper Function Prototypes */
int connections(Map g, LocationID start, LocationID end, TransportID type[]);
int haveConnectionByType (Map g, LocationID from, LocationID to, TransportID type, PlayerID player);
LocationID *allAdjacent (Map g, LocationID loc, TransportID type, LocationID *adjacent);



#endif
