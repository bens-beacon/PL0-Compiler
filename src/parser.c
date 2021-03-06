/* PARSER
 * C-CODE
 * Code by Ben
 * 
*/

#include "parser.h"

tProc* AktProc;															/* Aktuelle Prozedur	 		*/
tlist* Constblock;													/* Konstanteliste					*/
tlist* Labellist;														/* Labelkeller						*/
extern int line, col;												/* Zeile Spalte 					*/
int output = 0;															/* parameter um Infos zu 	*/

/* ---- BÖGEN ------------------------------------------------------- */
/* Programm 	--------------------------------------- */
tBog gProg[]=
{
/* 0 */ {BgGr,{(ul)iBlock	}, NULL, 1, 0}, 	/* Block		*/	
/* 1 */ {BgSy,{(ul)'.'		},  pr1, 2, 0}, 	/* . 				*/	
/* 2 */ {BgEn,{(ul)0			}, NULL, 0, 0}  	/* Ende 		*/	
};
/* Block			--------------------------------------- */
tBog gBlock[]=
{
/* 0 */ {BgSy,{(ul)zCST		}, NULL, 4, 1}, 	/* CONST		*/	
/* 1 */ {BgSy,{(ul)zVAR		}, NULL, 9, 2}, 	/* VAR			*/
/* 2 */ {BgSy,{(ul)zPRC		}, NULL,12,16}, 	/* Procedure*/
/* 3 */ {BgGr,{(ul)iState	},  bl5,17, 0}, 	/* Statement*/
/* 4 */ {BgMo,{(ul)mcIdent},  bl1, 5, 0}, 	/* Ident		*/		
/* 5 */ {BgSy,{(ul)'='		}, NULL, 6, 0}, 	/*   =			*/		
/* 6 */ {BgMo,{(ul)mcNum	},  bl2, 8, 0}, 	/* Numeral	*/
/* 7 */ {BgSy,{(ul)','		}, NULL, 4, 0}, 	/*   ,			*/		
/* 8 */ {BgSy,{(ul)';'		}, NULL, 1, 7}, 	/*   ;			*/
/* 9 */ {BgMo,{(ul)mcIdent},  bl3,11, 0}, 	/* Ident		*/
/*10 */ {BgSy,{(ul)','		}, NULL, 9, 0}, 	/*   ,			*/
/*11 */ {BgSy,{(ul)';'		}, NULL, 2,10}, 	/*   ;			*/
/*12 */ {BgMo,{(ul)mcIdent},  bl4,13, 0}, 	/* Ident		*/
/*13 */ {BgSy,{(ul)';'		}, NULL,14, 0}, 	/*   ;			*/	
/*14 */ {BgGr,{(ul)iBlock	}, NULL,15, 0}, 	/* Block		*/	
/*15 */ {BgSy,{(ul)';'		}, NULL, 2, 0}, 	/*   ;			*/
/*16 */ {BgNl,{(ul)0			},  bl6, 3, 0}, 	/* Nilbogen */
/*17 */ {BgEn,{(ul)0			}, NULL, 0, 0}  	/* Ende 		*/
};
/* Statement  --------------------------------------- */
tBog gState[]=
{
/* 0 */ {BgMo,{(ul)mcIdent},  st1, 8, 1}, /* Ident		*/
/* 1 */ {BgSy,{(ul)zIF		}, NULL,11, 2}, /* IF				*/
/* 2 */ {BgSy,{(ul)zWHL		},  st5,14, 3}, /* WHILE		*/
/* 3 */ {BgSy,{(ul)zBGN		}, NULL,17, 4}, /* BEGIN		*/
/* 4 */ {BgSy,{(ul)zCLL		}, NULL,20, 5}, /* CALL			*/
/* 5 */ {BgSy,{(ul)'?'		}, NULL,21, 6}, /*   ?			*/
/* 6 */ {BgSy,{(ul)'!'		}, NULL,23,26}, /*   !			*/
/* 7 */ {BgNl,{						}, NULL,10, 0}, /* NIL			*/								
/* 8 */ {BgSy,{(ul)zERG		}, NULL, 9, 0}, /* 	:=			*/ 								
/* 9 */ {BgGr,{(ul)iExpr	},  st2,10, 0}, /* Expressio*/
/*10 */ {BgEn,{(ul)0			}, NULL, 0, 0}, /* Ende 		*/
/*11 */ {BgGr,{(ul)iCond	},  st3,12, 0}, /* Condition*/
/*12 */ {BgSy,{(ul)zTHN		}, NULL,13, 0}, /* THEN			*/
/*13 */ {BgGr,{(ul)iState	}, NULL,24, 0}, /* Statement*/
/*14 */ {BgGr,{(ul)iCond	},  st6,15, 0}, /* Condition*/
/*15 */ {BgSy,{(ul)zDO		}, NULL,16, 0}, /* DO				*/
/*16 */ {BgGr,{(ul)iState	},  st7,10, 0}, /* Statement*/
/*17 */ {BgGr,{(ul)iState	}, NULL,19, 0}, /* Statement*/
/*18 */ {BgSy,{(ul)';'		}, NULL,17, 0}, /*   ;			*/
/*19 */ {BgSy,{(ul)zEND		}, NULL,10,18}, /* END			*/
/*20 */ {BgMo,{(ul)mcIdent},  st8,10, 0}, /* Ident		*/
/*21 */ {BgMo,{(ul)mcIdent},  st9,10, 0}, /* Ident		*/
/*22 */ {BgGr,{(ul)iExpr	}, st10,10, 0}, /* Expressio*/
/*23 */ {BgMo,{(ul)mcStrin}, st11,10,22}, /* String		*/
/*24 */ {BgSy,{(ul)zELSE	}, st12,25,25}, /* ELSE	  	*/
/*25 */ {BgGr,{(ul)iState	}, st13,10, 0}, /* Statement*/
/*26 */ {BgSy,{(ul)zDO		}, st14,27, 7}, /* DO				*/
/*27 */ {BgGr,{(ul)iState	}, NULL,28, 0}, /* Statement*/
/*28 */ {BgSy,{(ul)zWHL		}, NULL,29, 0}, /* WHILE		*/
/*29 */ {BgGr,{(ul)iCond	}, st15,30, 0}, /* Condition*/
/*30 */ {BgSy,{(ul)';'		}, NULL,10, 0}, /*   ;			*/
};
/* Expression --------------------------------------- */
tBog gExpr[]=
{
/* 0 */ {BgSy,{(ul)'-'		}, NULL, 2, 1}, /*  - 			*/
/* 1 */ {BgGr,{(ul)iTerm	}, NULL, 3, 0}, /* Term			*/
/* 2 */ {BgGr,{(ul)iTerm	},  ex1, 3, 0}, /* Term			*/
/* 3 */ {BgNl,{						}, NULL, 5, 0}, /* NIL			*/
/* 4 */ {BgEn,{(ul)0			}, NULL, 0, 5}, /* Ende 		*/
/* 5 */ {BgSy,{(ul)'+'		}, NULL, 7, 6}, /*  + 			*/
/* 6 */ {BgSy,{(ul)'-'		}, NULL, 8, 4}, /*  - 			*/
/* 7 */ {BgGr,{(ul)iTerm	},  ex2, 3, 0}, /* Term			*/
/* 8 */ {BgGr,{(ul)iTerm	},  ex3, 3, 0}  /* Term			*/
};
/* Term 			--------------------------------------- */
tBog gTerm[]=
{
/* 0 */ {BgGr,{(ul)iFact	}, NULL, 1, 0}, /* Faktor		*/
/* 1 */ {BgNl,{						}, NULL, 3, 0}, /* NIL			*/
/* 2 */ {BgEn,{(ul)0			}, NULL, 3, 0}, /* END 			*/
/* 3 */ {BgSy,{(ul)'*'		}, NULL, 5, 4}, /*  * 			*/
/* 4 */ {BgSy,{(ul)'/'		}, NULL, 6, 2}, /*  / 			*/
/* 5 */ {BgGr,{(ul)iFact	},  te1, 1, 0}, /* Faktor		*/
/* 6 */ {BgGr,{(ul)iFact	},  te2, 1, 0}  /* Faktor		*/
};
/* Faktor 		--------------------------------------- */
tBog gFact[]=
{
/* 0 */ {BgMo,{(ul)mcIdent},  fa2, 5, 1}, /* Ident		*/
/* 1 */ {BgMo,{(ul)mcNum  },  fa1, 5, 2}, /* Number		*/
/* 2 */ {BgSy,{(ul)'('    }, NULL, 3, 0}, /*   (			*/
/* 3 */ {BgGr,{(ul)iExpr  }, NULL, 4, 0}, /* Expressio*/
/* 4 */ {BgSy,{(ul)')'    }, NULL, 5, 0}, /*   )			*/
/* 5 */ {BgEn,{(ul)0      }, NULL, 0, 0}  /* END	  	*/
};	
/* Condition 	--------------------------------------- */
tBog gCond[]=
{
/* 0 */ {BgSy,{(ul)zODD		}, NULL, 2, 1}, /* ODD			*/	
/* 1 */ {BgGr,{(ul)iExpr	}, NULL, 4, 0}, /* Expressio*/
/* 2 */ {BgGr,{(ul)iExpr	}, NULL, 3, 0}, /* Expressio*/
/* 3 */ {BgEn,{(ul)0			}, NULL, 0, 0}, /* END 			*/
/* 4 */ {BgSy,{(ul)'='		},  co2,10, 5}, /*  = 			*/
/* 5 */ {BgSy,{(ul)'#'		},  co3,10, 6}, /*  # 			*/
/* 6 */ {BgSy,{(ul)'<'		},  co4,10, 7}, /*  < 			*/
/* 7 */ {BgSy,{(ul)zLE		},  co5,10, 8}, /*  <= 			*/
/* 8 */ {BgSy,{(ul)'>'		},  co6,10, 9}, /*  > 			*/
/* 9 */ {BgSy,{(ul)zGE		},  co7,10, 0}, /*  >= 			*/
/*10 */ {BgGr,{(ul)iExpr	},  co8, 3, 0}  /* Expressio*/
};

/* Graphenadressen	*/
tBog *vGr[]={gProg,gBlock,gState,gExpr,gTerm,gFact,gCond};

/* ---- Parse --------------------------------------------------------*/
int pars(tBog* pGraph)
{
  tBog* pBog=pGraph;
  int ret=0;																/* Ergebnis								*/
  if (Morph.MC==mcEmpty)Lex();							/* Starte Lex	für Spaces	*/
  int Graph = 0;
  while(1)
  {

		/* Ausgabefunktion */
		if(output == 1)
		{
			// Gebe Aktuellen / Folge, Nächsten Bogen und Morphem aus
			int g= 0;
			// Finde atuellen Graph 		
			for(int i=0;i<7;i++){ if (vGr[i] == pBog) Graph=i;}	
	
			// Finde aktuellen Zustand	
			tBog *Gra = vGr[Graph];
			for(g=0;;g++)														
			{
				if(Gra[g].iAlt== pBog->iAlt && Gra[g].iNext== pBog->iNext	) break;
			}
			// Ausgabe 									
			printf(ANSI_COLOR_GREEN "");	
			printf("Graph: %-3d | AkBo: %-2i | (Next: %-2i Alt: %-2i) | ",Graph,g,pBog->iNext,pBog->iAlt); 
			switch(Morph.MC)
			{
				case mcSymb:
					if(Morph.Val.Symb > 128)  printf("Keywor: %-5s ",Keyw[Morph.Val.Symb-zBGN]);
					else if (Morph.Val.Symb == 128) printf("Keywor: %-5s ",":=");
					else printf("Symbol: %-5c ",Morph.Val.Symb);
				break;
				case mcNum: printf("Number: %-5ld",Morph.Val.Num);
				break;
				case mcIdent: printf("Ident : %-5s",Morph.Val.pStr); 
				break;
				case mcStrin: printf("String: %-5s",Morph.Val.pStr);
				break;
			}
			printf("\n");
		}
	
		
    switch(pBog->BgD)												/* aktuell Bogentyp				*/
    {
			/* Nilbogen - */ 
      case BgNl: ret=1;   
				break;
			/* Symbol --- */ 
      case BgSy: ret=(Morph.Val.Symb==pBog->BgX.S);	
				break;
			/* Morphem -- */
      case BgMo: ret=(Morph.MC==(tMC)pBog->BgX.M); 		
				break;
			/* neuer Bogen*/  
      case BgGr: ret=pars(vGr[pBog->BgX.G]);					 
				break;
			/* Ende ----- */
      case BgEn:	 return 1;   														
    }
     
    /* Rufe Bogenfunktion */
		if (ret && (pBog->fx != NULL)) ret=pBog->fx();											// <- Diese Funktion muss 1 oder 0 zuückgeben
   
   /* Alternativbogen 		*/
		if (ret == 0)
		{											
			if (pBog->iAlt != 0) pBog=pGraph+pBog->iAlt;											// Warum wird hier addiert pBog=[pBog->iAlt]
      else 																	/* Fehler Alternative 0		*/
      {
				printf(ANSI_COLOR_RED " >> Syntax ERROR! \n");
				printf(ANSI_COLOR_YELLOW " >> After Line: %d Colum: %d \n",line, col);
				exit(EXIT_FAILURE);											
			}
    }  
    else 																		/* LEX für nächsten Bogen */				
    {
      if (pBog->BgD & BgSy || pBog->BgD & BgMo) Lex();
      pBog=pGraph+pBog->iNext;
    }
  }
}

/* ---- MAIN -------------------------------------------------------- */
int main(int argc, char *argv[])
{
	char *pBuf=argv[1];
	
	// aktiviere Ausgabefunktion
	if (argc > 2)
	{
		char *input=argv[2];
		if(0==strcmp("-I",input)) output =1 ;
		else printf(ANSI_COLOR_CYAN" >> To Show Compileroutput add '-I'! \n");
	}
	
	initLex(pBuf);														/* Lexer initialisieren 	*/
	
	Constblock = createlist();								/* Erstelle Konstantenblo */
	Labellist = createlist();									/* Erstelle Labelkeller 	*/
	AktProc = createProc(NULL);								/* MainProzedur						*/
	
	/* Write Code in CodeFile */
	openOFile(pBuf);
	pars(gProg);
	closeOFile();	
	printf(ANSI_COLOR_GREEN " >> Successfully compiled! \n");
	return 0;	
}







