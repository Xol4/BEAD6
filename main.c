/*Írjon C programot, amely gyermek processzeket indít és ezeken keresztül titkosít majd visszafejt egy egész számot.
A főprogram hozzon létre üzenetsort IPC_PRIVATE kulccsal.
Indítson négy gyermek processzt.
A szülőben ellenőrzötten kérjen be három, 1 és 34 közötti, "int" tipusú egész számot a standard inputról. Az üzenetsoron 
keresztül küldje át a számokat az első gyermek processznek, ezután várakozzon az eredményekre a negyedik gyermek processztől. 
A kapott eredményeket olvassa ki, és hasonlítsa össze a kezdetben bekért számokkal. Ha a számok mindegyike egyezik írja ki a 
standard outputra, hogy "OK", ellenkező esetben hogy "Nem OK".
A gyermek processzek a következő feladatokat lássák el:
Az első gyermek processz fogadja a számokat a szülő processztől az üzenetsoron keresztül. A kapott számok mindegyikét emelje ötödik
 hatványra majd küldje el a második gyereknek az üzenetet.
A második gyerek vegye a kapott számok modulóját 35-tel. A kapott titkosított számokat írja ki a képernyőre az "E" prefix-szel, majd 
küldje tovább az eredményeket a harmadik gyereknek.
A harmadik gyermek processz fogadja a második gyerek titkosított eredményeit az üzenetsoron keresztül. A kapott számokat emelje 5-ödik
 hatványra és küldje el üzenetsoron a negyedik gyereknek.
A negyedik gyerek vegye a kapott számok moduloját 35-tel. Az eredményeket írja ki a képernyőre "D" prefix-szel és küldje tovább azokat
 a szülőnek.
A program futásának befejezése előtt gondoskodjon az üzenetsor törléséről!
Hatványozáshoz lehet használni a pow() parancsot, nézzenek utána a manualban, benne van a helyes fordításhoz szükséges kapcsoló is!
Ellenőrizze a paraméterek számát, és ha több, mint 2, akkor írjon ki hibaüzenetet, és hibakóddal lépjen ki. Továbbá ellenőrizze a 
paramétereket, ha "-h" kapcsolót talál, írjon ki rövid ismertetőt a használatról, és normál visszatéréssel lépjen ki, "-x" kapcsoló 
esetén kihagyja a bekérést, és a 9,15,30 számokkal dolgozik tovább.
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>


struct uzenet
{
	long cim;
	int szam1;
	int szam2;
	int szam3;

	int eredmeny1;
	int eredmeny2;
	int eredmeny3;
};

bool ellenorzes(int szam)
{
	if(szam > 0 && szam <35) 
		return true;
	else 
		return false;
}

int main(int argc,char* argv[])
{

	struct uzenet uz;
	int msgID;
	int i;
	char* kapcsolo;

	msgID=msgget(IPC_PRIVATE,0777|IPC_CREAT);

	if(argc > 2)
	{
		printf("A program 2-nél kevsebb paramétert vár\n");
		return 1;
	}

	if(argc == 2)
		kapcsolo=argv[1];
	else
		kapcsolo="\n";
	

	

	for ( i = 0; i < 5; ++i)
	{
		if(fork() == 0 )
		{
			if(i==0)
			{
				msgrcv(msgID,&uz,sizeof(int) * 6,1,0777);
				uz.eredmeny1=pow((double)uz.szam1,5);
				uz.eredmeny2=pow((double)uz.szam2,5);
				uz.eredmeny3=pow((double)uz.szam3,5);
				uz.cim=2;
				msgsnd(msgID,&uz,sizeof(int) *6,0777);

			}

			if(i==1)
			{
				msgrcv(msgID,&uz,sizeof(int) * 6,2,0777);
				uz.eredmeny1=uz.eredmeny1%35;
				uz.eredmeny2=uz.eredmeny2%35;
				uz.eredmeny3=uz.eredmeny3%35;
				uz.cim=3;
				printf("E%d\nE%d\nE%d\n", uz.eredmeny1,uz.eredmeny2,uz.eredmeny3 );
				msgsnd(msgID,&uz,sizeof(int) *6,0777);

			}

			if(i==2)
			{
				msgrcv(msgID,&uz,sizeof(int) * 6,3,0777);
				uz.eredmeny1=pow((double)uz.eredmeny1,5);
				uz.eredmeny2=pow((double)uz.eredmeny2,5);
				uz.eredmeny3=pow((double)uz.eredmeny3,5);
				uz.cim=4;
				
				msgsnd(msgID,&uz,sizeof(int) *6,0777);
			}

			if(i==3)
			{
				msgrcv(msgID,&uz,sizeof(int) * 6,4,0777);
				uz.eredmeny1=uz.eredmeny1%35;
				uz.eredmeny2=uz.eredmeny2%35;
				uz.eredmeny3=uz.eredmeny3%35;
				uz.cim=5;
				printf("D%d\nD%d\nD%d\n", uz.eredmeny1,uz.eredmeny2,uz.eredmeny3 );
				msgsnd(msgID,&uz,sizeof(int) *6,0777);

			}

			break;
		}

		
	}

	if(i == 5)
	{

		
		if(strcmp(kapcsolo,"-h") == 0)
		{
		printf("Adj meg 3db 0-nál nagyobb és 35-nél kisebb számot \n");
		return 0;
		}	
	

		else if(strcmp(kapcsolo,"-x") == 0)
		{
			uz.szam1=9;
			uz.szam2=15;
			uz.szam3=30;
		}
		else if(strcmp(kapcsolo,"\n") == 0)
		{

		printf("Adj meg harom szamot: \n");
		scanf("%d %d %d",&uz.szam1,&uz.szam2,&uz.szam3);
			if(!ellenorzes(uz.szam1) || !ellenorzes(uz.szam2) || !ellenorzes(uz.szam2))
			{
				printf("A szamok nem lehetnek kisebbek mint 1 és nem lehetnek nagyobbak mint 34\n");
				return -1;
			}
			
		}
		uz.cim=1;
		msgsnd(msgID,&uz,sizeof(int) * 6,0777);

		msgrcv(msgID,&uz,sizeof(int)*6,5,0777);

		if(uz.szam1 == uz.eredmeny1 && uz.szam2 == uz.eredmeny2 && uz.szam3 == uz.eredmeny3)
			printf("OK\n");
		else
			printf("NEM OK\n");

		msgctl(msgID,IPC_RMID,NULL);
		
		}


	}

