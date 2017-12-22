/* NAMELIST
 * C-Code
 * Code by Ben
 *
*/

#include "namelist.h"

/* ---- Funktionen -------------------------------------------------- */ 
/* Procedure erstellen 		*/
tProc* createProc(short IdxProc, tProc* pParent)
{
	tProc *Proc 				= malloc(sizeof(tProc)); 
	Proc->Kz						= KzProc;
	Proc->pLBez 				= createlist();
	Proc->IdxProc 			= IdxProc;
	Proc->pParent 			= pParent;
	Proc->SpzzVar				= 0;
	return Proc;
}
/* tBez Proc anhängen			*/
tBez* createBezProc(tProc* Proc, char *pName)																			
{
		tProc *newProc		= createProc(Proc->IdxProc++,Proc);								
		tBez *newBez			= malloc(sizeof(tBez));
		newBez->Kz 				= KzProc;
		newBez->IdxProc		= Proc->IdxProc;
		newBez->pObj 			= newProc;
		newBez->pName			= pName;
		
		return newBez;
}
/* tBez Var anhängen 			*/
tBez* createBezVar(tProc* Proc,char *pName)
{
		tVar *newVar 			= malloc(sizeof(tVar)); 
		newVar->Kz				= KzVar;
		newVar->Dspl			= Proc->SpzzVar;
		Proc->SpzzVar		 += 4;
		tBez *newBez 			= malloc(sizeof(tBez));
		newBez->Kz 				= KzVar;
		newBez->IdxProc		= Proc->IdxProc;
		newBez->pObj 			= newVar;
		newBez->pName			= pName;
		
		return newBez;
}
/* tBez  Const anhängen		*/
tBez* createBezConst(tProc* Proc,long Val,char *pName)
{
		tConst *newConst 	= malloc(sizeof(tConst)); 
		newConst->Kz			= KzConst;
		newConst->Val			= Val;
		newConst->Idx			= Constblock->length;	/* Index ist akt. Länge		*/
		insertend(Constblock,newConst);					/* Füge an Konstantenblock*/
		tBez *newBez 			= malloc(sizeof(tBez));
		newBez->Kz 				= KzConst;
		newBez->IdxProc		= Proc->IdxProc;
		newBez->pObj 			= newConst;
		newBez->pName			= pName;
		return newBez;
}
/* tBez  Const mit Idx an */
tBez* createBezConstIdx(tProc* Proc,long Val,char *pName,int Idx)
{
		tConst *newConst 	= malloc(sizeof(tConst)); 
		newConst->Kz			= KzConst;
		newConst->Val			= Val;
		newConst->Idx			= Idx;								/* Index wurde übergeben	*/
		insertend(Constblock,newConst);					/* Füge an Konstantenblock*/
		tBez *newBez 			= malloc(sizeof(tBez));
		newBez->Kz 				= KzConst;
		newBez->IdxProc		= Proc->IdxProc;
		newBez->pObj 			= newConst;
		newBez->pName			= pName;
		return newBez;
}


/* Suche Constante				*/
/* Hier wird die übergebene Konstante gesucht! Und wenn gefunden der 
 * Bezeichner zurückgegeben. 
*/																						
tConst * searchConst(long Val)
{
	Constblock->curr = Constblock->first;
	while(1)
	{
		tConst *Const_tmp = Constblock->curr->data;
		if(Const_tmp->Val==Val){return Constblock->curr->data; }
		else if(Constblock->curr == Constblock->last){return NULL;}
		else Constblock->curr = Constblock->curr->next;
	}
}

/* Suche Bezeichner				*/ 		
/* Hier wird der Bezeichner in der übergebenen Prozedur gesucht. Die 
 * übergebene Prozedur ist aber immer die aktuelle.
*/																				
tBez* searchBEZ(tProc* Proc, char *pName)																
{
	Proc->pLBez->curr	= Proc->pLBez->first;		/* Hole erstes Element		*/
	while(1)
	{
		tBez* Bez_tmp = Proc->pLBez->curr->data;
		if(Bez_tmp->pName == pName){return Proc->pLBez->first->data;}
		else if(Proc->pLBez->curr == Proc->pLBez->last){return NULL;}	
		else Proc->pLBez->curr = Proc->pLBez->curr->next;
	}
}

/* Globale Suche 					*/
/* Ausgehend vond er akutellen Prozedur werden alle überliegenden 
 * Elternprozeduren durchsucht. Wenn Main Prozedur erreicht und 
 * durchsucht und nicht gefunden -> dann ENDE.
*/
tBez* globalsearchBEZ(tProc* Proc, char *pName)																			
{
	tProc* Proc_tmp = Proc;
	tBez * Bez_tmp;

	while(1)
	{
		Bez_tmp = searchBEZ(Proc_tmp,pName);		/* normale Suche					*/
		if (Bez_tmp != NULL) return Bez_tmp; 		/* Wenn gefunden					*/
		else if (Proc_tmp->pParent == 0 && Bez_tmp == NULL ) 
				return NULL;												/* Ende										*/
		else Proc_tmp = Proc_tmp->pParent;			/* Nicht gefunden					*/
	}
}

/* Lösche Listen					*/
/* Diese Funktion gibt von einer Procedur das Objekt den Bezeichner 
 * und die Procedur selbst wieder frei.
*/
void clear(tProc* Proc)
{
	if(Proc->pLBez->last!=NULL)								/* Bis Liste alle					*/
	{ 
		tBez* Bez_tmp = Proc->pLBez->last->data;
		free(Bez_tmp->pObj);										/* lösche Objekt					*/				
		removelast(Proc->pLBez);								/* lösche letzten Bezei		*/
		clear(Proc);														
	}
	else 
	{
		//printf("Löschen - Erfolgreich");
		free(Proc);															/* Procedur frei geben		*/
	}
}
