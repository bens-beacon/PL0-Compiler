/* Semantikroutine
 * C-Code
 * Code by Ben
 * 
*/

#include "smr.h"

/* Globales														*/
int compid;																	/* Globale Var Condition	*/
extern int LenCode;													/* Codelänge							*/
extern tProc* AktProc;
extern tMorph Morph;												/* Aktuelles Morphem			*/
extern tlist* Constblock;										/* Konstanteliste					*/
extern tlist* Labellist;										/* Labelkeller						*/
tBez* BezConstGlobal;												/* Globaler Bez für Const	*/
extern char* pCode;
extern char* vCode;

/* Ist Konstantenname schon vorhanden */
int bl1()
{
	tBez* Bez_tmp = searchBEZ(AktProc, Morph.Val.pStr);
	if(Bez_tmp != NULL)												/* Bez schon vorhanden ?	*/
	{
		printf(ANSI_COLOR_RED " >> Constname \"%s\" is already defined!\n",Morph.Val.pStr);
		exit(EXIT_FAILURE);
	}
	tBez* BezConst = createBezConst(Morph.Val.pStr);/* Erstelle ConstBez*/
	BezConstGlobal = BezConst;
	return OK;
}

/* Bezeichner mit Konstante anlegen	  */
int bl2()
{
	tConst* Const_tmp = searchConst(Morph.Val.Num);		
	if(Const_tmp == NULL)											/* Konstenwert vorhanden?	*/
	{																					/* Nicht Vorhanden, erste */
		tBez* BezConst = addValueBezConst(AktProc, BezConstGlobal, Morph.Val.Num);
		insertend(AktProc->pLBez,BezConst);
	}
	else 																			/* Vorhanden, Index übern */
	{
		tBez* BezConst = addValueBezConst(AktProc, BezConstGlobal, Const_tmp->Val);
		insertend(AktProc->pLBez,BezConst);
	}
	return OK;
}

/* Bezeichner mit Variable anlegen 	  */
int bl3()
{
	tBez* Bez_tmp = searchBEZ(AktProc, Morph.Val.pStr);
	if(Bez_tmp != NULL)												/* Bez schon vorhanden ?	*/
	{
		printf(ANSI_COLOR_RED " >> Varname \"%s\"  is already defined!\n", Morph.Val.pStr);	
		exit(EXIT_FAILURE);
	}
	else
	{
			tBez* BezVar = createBezVar(AktProc, Morph.Val.pStr); 
			insertend(AktProc->pLBez,BezVar);
	}
	return OK;
}

/* Bezeichner mit neuen Procedur 		  */
int bl4()
{
	tBez* Bez_tmp = searchBEZ(AktProc, Morph.Val.pStr);
	if(Bez_tmp != NULL)
	{
		if(Bez_tmp->Kz == KzConst)
		{
			printf(ANSI_COLOR_RED " >> \"%s\" is already defined as Const!\n",Morph.Val.pStr);	
			exit(EXIT_FAILURE);					
		}
		else if(Bez_tmp->Kz == KzVar)
		{
			printf(ANSI_COLOR_RED " >> \"%s\" is already defined as Var!\n",Morph.Val.pStr);	
			exit(EXIT_FAILURE);				
		}	
		else if(Bez_tmp->Kz == KzProc)
		{
			printf(ANSI_COLOR_RED " >> \"%s\" is already defined as Proc!\n",Morph.Val.pStr);	
			exit(EXIT_FAILURE);				
		}
	}
	else
	{
		tBez * BezProc = createBezProc(AktProc, Morph.Val.pStr);
		tProc* Proc_tmp = BezProc->pObj;		
		insertend(AktProc->pLBez,BezProc);		
		AktProc = Proc_tmp;		
		
	}	
	return OK;
}

/* lösche Prozedur 										*/
int bl5()
{
	code(retProc);
	tProc* Proc_tmp = AktProc;
	
	if( AktProc->pParent != NULL)
	{
		AktProc = Proc_tmp->pParent;						/* ElternProzedur wird akt*/
	}	
	clear(Proc_tmp);
	
	CodeOut();																/* Schreibe i Ausgabedatei*/
}

/* Beginn des Anwesiungsteils					*/
int bl6()
{
	tProc* Proc_tmp = AktProc;
	code(entryProc,0,Proc_tmp->IdxProc,AktProc->SpzzVar); /* entryproc	*/
	return OK;
}

/* Abschlussarbeiten									*/
int pr1()
{
	/* Konstanten block hate extra Funktion im Codegen */
	// Löschen der restlichen Prozeduren
	return OK;									
}

/* Wertzuweisung 	LS		PushAdrVar...	*/
int st1()
{
	tBez * Bez_tmp = globalsearchBEZ(AktProc,Morph.Val.pStr);
	if( Bez_tmp == NULL)
	{
		printf(ANSI_COLOR_RED " >> Variable \"%s\" is not defined!\n",Morph.Val.pStr);	
		exit(EXIT_FAILURE);			
	}
	else
	{
		if(Bez_tmp->Kz == KzVar)
		{		
			tVar* Var_tmp = Bez_tmp->pObj;
			if(Bez_tmp->IdxProc==0) code(puAdrVrMain,Var_tmp->Dspl);	/* Mai*/
			else if(AktProc->IdxProc == Bez_tmp->IdxProc) 
															code(puAdrVrLocl,Var_tmp->Dspl);	/* Loc*/
			else {code(puAdrVrGlob,Var_tmp->Dspl,Bez_tmp->IdxProc);	}	/* Glo*/																		
		}
		else if(Bez_tmp->Kz == KzConst)
		{
			printf(ANSI_COLOR_RED " >> \"%s\" is a Const not a Variable!\n",Morph.Val.pStr);	
			exit(EXIT_FAILURE);					
		}
		else if(Bez_tmp->Kz == KzProc)
		{
			printf(ANSI_COLOR_RED " >> \"%s\" is a Proc not a Variable!\n",Morph.Val.pStr);	
			exit(EXIT_FAILURE);				
		}		
	} 	
	return OK;
}

/* Wertzuweisung RS										*/
int st2()
{
	code(storeVal);														/* StoreValue							*/
	return OK;
}

/* Condition													*/
int st3()
{
	int jn_start = (pCode-vCode)+1;						/* Zeigt jnot Adressesbyte*/
	pushLabel(jn_start);											/* Add Label							*/
	code(jnot,0);
	return OK;
}

/* Zeigt auf Else oder danach Bereich	*/
int st4()
{
	int jn_start = getNjmp();									/* Get JmpNumber					*/
	int jn_end = pCode-vCode;
	int n_bytes = jn_end-jn_start-2;
	wr2ToCodeAtP(n_bytes,vCode+jn_start);			/* Adresse nachtragen			*/
	return OK;
}

/* ELSE nach Statement 								*/
int st12()
{
	code(jmp,0);
	
	/* Trage Sprungadresse nach */
	int jn_start = getNjmp();									/* Get JmpNumber					*/
	int jn_end = pCode-vCode;
	int n_bytes = jn_end-jn_start-2;
	wr2ToCodeAtP(n_bytes,vCode+jn_start);			/* Adresse nachtragen			*/

	/* Neues Label */
	int jn_start_new = (pCode-vCode)-2;				/* Aktuelle Stelle +1			*/
	pushLabel(jn_start_new);									/* Label auf YY von JMP		*/
	return OK;
}

/* Muss Auf ersten Bereich zeigen	*/
int st13()
{
	int jn_start = getNjmp();									/* Get JmpNumber					*/
	int jn_end = pCode-vCode;
	int n_bytes = jn_end-jn_start-2;
	wr2ToCodeAtP(n_bytes,vCode+jn_start);			/* Adresse nachtragen			*/

	return OK;
}

/* DO															*/
int st14()
{
	pushLabel(pCode-vCode);										/* Startadresse						*/
	return OK;
}

/* nach Condition 								*/
int st15()
{
	int jn_start = getNjmp();									/* Get JmpNumber					*/
	int jn_end = pCode-vCode;
	int n_bytes = jn_end-jn_start+3;
	code(jnot	,-n_bytes);
	return OK;
}


/* WHILE															*/
int st5()
{
	pushLabel(pCode-vCode);
	return OK;
}

/* WHILE nach Condition								*/
int st6()
{
	int jn_start = (pCode-vCode)+1;						/* Zeigt jnot Adressesbyte*/
	pushLabel(jn_start);											/* Add Label							*/
	code(jnot,0);
	return OK;
}

/* WHILE	nach Statement						*/
int st7()
{
	int jn_start = getNjmp();									/* Get JmpNumber					*/
	int jn_end = pCode-vCode;
	int n_bytes = jn_end-jn_start+1;
	wr2ToCodeAtP(n_bytes,vCode+jn_start);			/* Adresse nachtragen			*/

	int co_start = getNjmp();
	n_bytes = pCode-vCode-co_start+3;
	code(jmp,-n_bytes);

	return OK;
}


/* Rufe Funktion auf */
int st8()
{
	tBez * Bez_tmp = globalsearchBEZ(AktProc,Morph.Val.pStr);
	if( Bez_tmp == NULL)
	{
		printf(ANSI_COLOR_RED " >> Procedure \"%s\" is not defined!\n",Morph.Val.pStr);	
		exit(EXIT_FAILURE);			
	}
	else
	{
		if(Bez_tmp->Kz == KzVar)
		{			
			printf(ANSI_COLOR_RED " >> \"%s\" is a Var not a Proc!\n",Bez_tmp->pName);	
			exit(EXIT_FAILURE);																		
		}
		else if(Bez_tmp->Kz == KzConst)
		{
			printf(ANSI_COLOR_RED " >> \"%s\" is a Const not a Proc!\n",Bez_tmp->pName);	
			exit(EXIT_FAILURE);					
		}
		else if(Bez_tmp->Kz == KzProc)
		{
			tProc* Proc_tmp = Bez_tmp->pObj;
			code(call,Proc_tmp->IdxProc);			
		}		
	} 	
	return OK;
}

/* Eingabe	 													*/
int st9()
{
	tBez * Bez_tmp = globalsearchBEZ(AktProc,Morph.Val.pStr);
	if( Bez_tmp == NULL)
	{
		printf(ANSI_COLOR_RED " >> Variable \"%s\" is not defined!\n",Morph.Val.pStr);	
		exit(EXIT_FAILURE);			
	}
	else
	{
		if(Bez_tmp->Kz == KzVar)
		{		
			tVar* Var_tmp = Bez_tmp->pObj;
			if(Bez_tmp->IdxProc==0) code(puAdrVrMain,Var_tmp->Dspl);	/* Mai*/
			else if(AktProc->IdxProc == Bez_tmp->IdxProc) 
															code(puAdrVrLocl,Var_tmp->Dspl);	/* Loc*/
			else {code(puAdrVrGlob,Var_tmp->Dspl,Bez_tmp->IdxProc);	}	/* Glo*/			
			code(getVal);																					/* GETVAL */		
		}
		else if(Bez_tmp->Kz == KzConst)
		{
			printf(ANSI_COLOR_RED " >> \"%s\" is a Const not a Variable!\n",Bez_tmp->pName);	
			exit(EXIT_FAILURE);					
		}
		else if(Bez_tmp->Kz == KzProc)
		{
			printf(ANSI_COLOR_RED " >> \"%s\" is a Proc not a Variable!\n",Morph.Val.pStr);
			exit(EXIT_FAILURE);				
		}		
	}
	return OK;
	
}

/* Ausgabe 														*/
int st10()
{
	code(putVal);															/* PUTVAL									*/									
	return OK;
}

/* Ausgabe 	String										*/
int st11()
{
	code(putStrg,Morph.Val.pStr);							/* PUTSTR									*/									
	return OK;
}

/* Vorzeichen Minus										*/
int ex1()
{
	code(vzMinus);														/* vzMinus								*/
	return OK;
}

/* Addieren														*/
int ex2()
{
	code(OpAdd);															/* opAdd									*/
	return OK;
}

/* Subtrahieren												*/
int ex3()
{
	code(OpSub);															/* opSub									*/
	return OK;
}

/* Mulitplizieren											*/
int te1()
{
	code(OpMult);															/* opMud									*/
	return OK;
}

/* Dividieren													*/
int te2()
{
	code(OpDiv);															/* opDiv									*/
	return OK;
}

/* Faktor Konstante										*/
int fa1()
{
	tConst* Const_tmp = searchConst(Morph.Val.Num);		
	if(Const_tmp == NULL)											/* Konstenwert vorhanden?	*/
	{																					/* Nicht Vorhanden, erste */
		// Konstante anlegen
		tConst *newConst 	= malloc(sizeof(tConst)); 
		newConst->Kz			= KzConst;
		newConst->Idx			= Constblock->length;	/* Index ist akt. Länge		*/
		newConst->Val			= Morph.Val.Num;
		insertend(Constblock,newConst);					/* Füge an Konstantenblock*/
		code(puConst,newConst->Idx);						/* Konstante anlegen 			*/
	}
	else 																			/* Vorhanden, Index übern */
	{
		code(puConst,Const_tmp->Idx);						/* Konstante anlegen 			*/
	}
	return OK;
}

/* Faktor anlegen											*/
int fa2()
{
	tBez * Bez_tmp = globalsearchBEZ(AktProc,Morph.Val.pStr);
	if( Bez_tmp == NULL)
	{
		printf(ANSI_COLOR_RED " >> Variable \"%s\" is not defined!\n",Morph.Val.pStr);	
		exit(EXIT_FAILURE);			
	}
	else
	{
		if(Bez_tmp->Kz == KzVar)
		{		
			tVar* Var_tmp = Bez_tmp->pObj;
			if(Bez_tmp->IdxProc==0) code(puValVrMain,Var_tmp->Dspl);	/* Mai*/
			else if(AktProc->IdxProc == Bez_tmp->IdxProc) 
															code(puValVrLocl,Var_tmp->Dspl);	/* Loc*/
			else {code(puValVrGlob,Var_tmp->Dspl,Bez_tmp->IdxProc);	}	/* Glo*/				
		}
		else if(Bez_tmp->Kz == KzConst)
		{
			tConst* Const_tmp = Bez_tmp->pObj;
			code(puConst,Const_tmp->Idx);					/* Konstante anlegen 			*/
		}
		else if(Bez_tmp->Kz == KzProc)
		{
			printf(ANSI_COLOR_RED " >> \"%s\" is a Proc not a Variable!\n",Morph.Val.pStr);	
			exit(EXIT_FAILURE);				
		}		
	} 
	return OK;
}

/* Condition 													*/
int co1()
{
	code(odd);																/* odd										*/
	return OK;
}

/* = 																	*/
int co2()
{
	compid = cmpEQ;														/* cmpEQ									*/
	return OK;
}

/* # 																	*/
int co3()
{
	compid = cmpNE;														/* cmpNE									*/
	return OK;
}

/* < 																	*/
int co4()
{
	compid = cmpLT;														/* cmpLT									*/
	return OK;
}

/* <= 																*/
int co5()
{
	compid = cmpLE;														/* cmpLE									*/
	return OK;
}

/* >																	*/
int co6()
{
	compid = cmpGT;														/* cmpGT									*/
	return OK;
}

/* >= 																	*/
int co7()
{
	compid = cmpGE;														/* cmpGE									*/
	return OK;
}

/* Codegenerierung											*/
int co8()
{
	code(compid);
	return OK;
}