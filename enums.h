#ifndef __ENUMS_H
#define __ENUMS_H

enum sign {POSITIVE, NEGATIVE}; /*utilis� dans les rationels*/
enum varStatus {NORMAL, POST, DERIVATIVE,CONSTANT}; /*utilis� pour d�signer le statut d'une variable dans une expression*/
enum exprType{TERM,LOCINIT,PARAM,
	      TRUE, FALSE,
	      AND, OR, 
	      PLUS, MINUS, DIV, TIMES, 
	      LES, LEQ, EQU, GEQ, GRT}; /*utilis� dans la d�finition des expressions*/
enum varType { INTEGRATOR, STOPWATCH, CLOCK, ANALOG, PARAMETER, DISCRETE, ANY_VAR_TYPE };
enum nameType {AUTOMATON,LOCATION, LABEL, VAR};/*utilis� dans les AVL*/
enum labelType {EVENT,ORDER,INTERNAL,STANDARD, BROADCAST, URGENT};
enum transitionStatus{ORDINARY, ASAP,BROADCASTEMITTER,BROADCASTRECEIVER};
enum automType {ELASTIC, UPPAAL,HYTECH};
enum locationType {SLW, URG, COMMIT};
#endif
