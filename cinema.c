#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define STUDENT 7
#define FULLFARE 10

	/*struct which is hold words which is read from input file*/
	struct readingFile
	{
    	char *word[5];					/*words in  line*/
    	struct readingFile *next ;		/*node which is hold next line*/
	} readingFile;  

	/*struct which is hold halls*/
	struct hall{
		char *hallName;
		char *movieName;
		int width;
		int height;
		char **seatStatus;				/*matrice that represents seats in hall*/
		struct hall *next;				/*node which is hold next hall*/
		int end;
	}hall;
	
	void fileRead(FILE *,struct readingFile *);						/*function which is reading file*/
	struct hall* createHall(struct hall *,struct readingFile *,FILE *);	/*function for creating hall*/
	void buyTicket(struct hall *,struct readingFile *,FILE *);				/*function for buying ticket*/
	int findHeight(char *);											/*this is finding seat index from seatlabel*/
	int controlSeat(struct hall *, int, int, int);					/*checking for the seats if it is full or not*/
	char findTicketPerson(char *);									/*checking student or full ticket*/
	void cancelTicket(struct hall *,struct readingFile *,FILE *);			/*function for cancelling ticket*/
	void showHall(struct hall *,struct readingFile *,FILE *);				/*it is showing halls situation*/
	void statistics(struct hall *,int,FILE *);								/*shows all statistics in cinema*/
		
	int main(int argc, char *argv[])
	{
		FILE *inputFile =fopen(argv[1],"r");
		if (!inputFile)
        		return 0;
		FILE *outputFile = fopen("output.txt" , "w");
		if (!outputFile)
        		return 0;
		
		
		struct readingFile *lineArray = (struct readingFile *) malloc (sizeof(struct readingFile)); /*for holding first line it creates a place for it*/
		struct hall *hallList =(struct hall *) malloc (sizeof(struct hall));				/*linkedlist which is hold halls*/
		hallList->end = 0;
		fileRead(inputFile,lineArray);
		fclose(inputFile);
		
		/*checking for the first word to find out first operation to do*/
		while(lineArray->next!=NULL){
			if(!strcmp(lineArray->word[0],"CREATEHALL")){
				hallList = createHall(hallList,lineArray,outputFile);
			}else if(!strcmp(lineArray->word[0],"BUYTICKET")){
				buyTicket(hallList,lineArray,outputFile);
			}else if(!strcmp(lineArray->word[0],"CANCELTICKET")){
				cancelTicket(hallList,lineArray,outputFile);
			}else if(!strcmp(lineArray->word[0],"SHOWHALL")){
				showHall(hallList,lineArray,outputFile);
			}else if(!strcmp(lineArray->word[0],"STATISTICS")){
				int count=0;
				statistics(hallList,count,outputFile);
			}
			lineArray = lineArray->next;	/*next line*/
		}
		fclose(outputFile);		
		return 0;
	}
		/*function which is reading file*/
	void fileRead(FILE *inputFile,struct readingFile *lineArray)
	{
    		char buf[200];
    		char *word;
    		int i;
    		
    		if(fgets(buf,200, inputFile)==NULL)
			{
    			return;
			}
			else
			{	
				int count = 0;
				word = strtok(buf, " \n");
				while(word != NULL){
					lineArray->word[count] = malloc (strlen(word)+1);		/*creates a place for word plus one*/
					strcpy(lineArray->word[count],word);					/*copying that info and go to next line*/
					count++;
					word = strtok(NULL, " \n");
				}
				for(i=0;i<5;i++){					/*Reading, ["] character delete track*/
					if(lineArray->word[i]!=NULL){
						if(strcmp(lineArray->word[i],"\"\""))
							lineArray->word[i] = strtok(lineArray->word[i], "\"");
					}
				}
				lineArray->next = (struct readingFile *)malloc(sizeof(struct readingFile));		/*for next line it takes a place from memory*/
			}
			fileRead(inputFile,lineArray->next);		/*keeps reading in recursive way*/
	}
	/*function for creating hall*/
	struct hall* createHall(struct hall *hallList,struct readingFile *line,FILE *outputFile){
	
		int i,j;
		struct hall *temp = (struct hall *) malloc (sizeof(struct hall));	/*creates temporary hall*/
		temp->end = 1;
		temp->hallName=line->word[1];
		temp->movieName=line->word[2];
		temp->width=atoi(line->word[3]);
		temp->height=atoi(line->word[4]);
		
		/*for seats matrice rows it takes a place from memory*/
		temp->seatStatus = (char **)malloc( temp->height * sizeof(char *) );
			if( temp->seatStatus == NULL )
				printf( "Insufficient memory!" );

		/*same for the columns */
		for( i=0 ; i < temp->height; i++ ) {
			temp->seatStatus[i] = malloc( temp->width * sizeof(char) );
			if( temp->seatStatus[i] == NULL ){
				printf( "Insufficient memory!" );
			}
			for(j = 0; j < temp->width;j++){		
				temp->seatStatus[i][j] = ' ';	/*checking if seats are free or not*/
			}
		}
		temp->next = hallList;	/*add new hall to top of the list*/
		return temp;			/*return new list*/
		
	}
	/*function for buying ticket*/
	void buyTicket(struct hall * hall,struct readingFile *line,FILE *outputFile){
			
			
		if(!strcmp(line->word[1],"\"\"")){
			fprintf(outputFile,"ERROR: Movie name cannot be empty\n");
			printf("ERROR: Movie name cannot be empty\n");
			return;
		}
		if(hall->end==0){
			return;
		}else if(strcmp(line->word[1],hall->movieName)){	/*compare hall names*/
			buyTicket(hall->next,line,outputFile);
		}else{												/*if it is right hall then buy the ticket*/
			
			int i;
			int seatWightNo = (int)(line->word[2][0]-'A');
			int seatHeight = findHeight(line->word[2])-1;
			int ticketNumber = atoi(line->word[4]);
			char ticketPerson = findTicketPerson(line->word[3]);
			
			if(seatWightNo > hall->width || seatHeight > hall->height ){	/*if seats are not avaliable then print out the message*/
				fprintf(outputFile ,"ERROR: Seat %s is not defined at %s\n" , line->word[2] , hall->hallName);
				printf("ERROR: Seat %s is not defined at %s\n" , line->word[2] , hall->hallName);
			}else if((seatWightNo+ticketNumber)>hall->width || controlSeat(hall, seatWightNo, seatHeight, ticketNumber)){	/*if seats are full error*/
				fprintf(outputFile,"ERROR: Specified seat(s) in %s are not available! They have been already taken.\n" , hall->hallName);
				printf("ERROR: Specified seat(s) in %s are not available! They have been already taken.\n" , hall->hallName);
			}else{					/*buy ticket and make filled the seats */
				fprintf(outputFile,"%s [%s] Seat(s) " , hall->hallName , hall->movieName);
				printf("%s [%s] Seat(s) " , hall->hallName , hall->movieName);
				for(i = seatWightNo; i < seatWightNo+ticketNumber; i++){
					hall->seatStatus[seatHeight][i] = ticketPerson;
					fprintf(outputFile,"%c%d",(char)(i+'A') ,seatHeight+1);
					printf("%c%d",(char)(i+'A') ,seatHeight+1);
					if(i != seatWightNo+ticketNumber-1){
						fprintf(outputFile,",");
						printf(",");
					}
				}
				fprintf(outputFile," successfully sold.\n");
				printf(" successfully sold.\n");
			}	
		}
	}
	/*function for cancelling ticket*/
	void cancelTicket(struct hall * hall,struct readingFile *line,FILE *outputFile){
		
		if(!strcmp(line->word[1],"\"\"")){
			fprintf(outputFile,"ERROR: Movie name cannot be empty\n");
			printf("ERROR: Movie name cannot be empty\n");
			return;
		}
		
		if(hall->end==0){
			return;
		}else if(strcmp(line->word[1],hall->movieName)){		/*comparing hall names*/
			cancelTicket(hall->next,line,outputFile);
		}else{											/*if it is right hall then sell the tickets*/
			int i;
			int seatWightNo = (int)(line->word[2][0]-'A');
			int seatHeight = findHeight(line->word[2])-1;
			if(seatWightNo > hall->width || seatHeight > hall->height ){	/*if seats are not avaliable then print out the message*/
				fprintf(outputFile ,"ERROR: Seat %s is not defined at %s\n" , line->word[2] , hall->hallName);
				printf("ERROR: Seat %s is not defined at %s\n" , line->word[2] , hall->hallName);
			}else if(hall->seatStatus[seatHeight][seatWightNo] == ' ' ){	 /*ticket couldn't be sold'*/		
				fprintf(outputFile,"ERROR: Seat %s in %s was not sold.\n" ,  line->word[2], hall->hallName);
				printf("ERROR: Seat %s in %s was not sold.\n" ,  line->word[2], hall->hallName);
			}else{												/*cancel bilet and open it for the new sales*/
				hall->seatStatus[seatHeight][seatWightNo] = ' '; 
				fprintf(outputFile,"%s [%s] purchase is cancelled. Seat %s is now free.\n" , hall->hallName,hall->movieName,line->word[2]);
				printf("%s [%s] purchase is cancelled. Seat %s is now free.\n" , hall->hallName,hall->movieName,line->word[2]);
			}
		}
	}
	
	/*Bif student is the customer then return s otherwise return f*/
	char findTicketPerson(char *word){
		
		if(!strcmp(word,"Student")){
			return 's';	
		}else{
			return 'f';
		}
	}
	
	/*if all seats are free then return 0 otherwise return 1*/
	int controlSeat(struct hall * hall, int seatWightNo, int seatHeight, int ticketNumber){
		
		int i;
		
		for(i = seatWightNo; i < seatWightNo+ticketNumber; i++){
			if(hall->seatStatus[seatHeight][i] != ' '){	
		
				return 1;
			}
		}
		return 0;
	}
		/*find row according to seat label*/
	int findHeight(char * word){
		
		char *temp = malloc (strlen(word)-1 );
		int height;
		strncpy(temp, word+1, strlen(word)-1 );
		return height=atoi(temp);
	}
	/*it is showing halls situation*/
	void showHall(struct hall * hall,struct readingFile * line,FILE *outputFile){
		
		int i,j;
		if(!strcmp(line->word[1],"\"\"")){
			fprintf(outputFile,"ERROR: Hall name cannot be empty\n");
			printf("ERROR: Hall name cannot be empty\n");
			return;
		}
		
		if(hall->end==0){
			return;
		}else if(strcmp(line->word[1],hall->hallName)){
			showHall(hall->next,line,outputFile);		/*compare hall names*/
		}else{
			fprintf(outputFile,"%s sitting plan\n" ,hall->hallName);
			printf("%s sitting plan\n" , hall->hallName);
			for(i=0;i<hall->height*2+1;i++){
				if(i%2==0){
					if(hall->height<10){
						fprintf(outputFile," ");
						printf(" ");
					}else if(hall->height<100){
						fprintf(outputFile,"  ");
						printf("  ");
					}else if(hall->height<1000){
						fprintf(outputFile,"   ");
						printf("   ");
					}
					for(j=0;j<hall->width*2+1;j++){
						fprintf(outputFile,"-");
						printf("-");
					}
				}else{
					if(hall->height<10){
						fprintf(outputFile,"%d",i/2+1);
						printf("%d",i/2+1);
					}else if(hall->height<100){
						fprintf(outputFile,"%-2d",i/2+1);
						printf("%-2d",i/2+1);
					}else if(hall->height<1000){
						fprintf(outputFile,"%-3d",i/2+1);
						printf("%-3d",i/2+1);
					}
					
					for(j=0;j<hall->width;j++){	
						fprintf(outputFile, "|%c", hall->seatStatus[i/2][j]);
						printf("|%c", hall->seatStatus[i/2][j]);
					}
					fprintf(outputFile,"|");
					printf("|");
				}
				fprintf(outputFile,"\n");
				printf("\n");
			}
			if(hall->height<10){
				fprintf(outputFile," ");
				printf(" ");
			}else if(hall->height<100){
				fprintf(outputFile,"  ");
				printf("  ");
			}else if(hall->height<1000){
				fprintf(outputFile,"   ");
				printf("   ");
			}
			for(j=0;j<hall->width;j++){			
				fprintf(outputFile," %c", (char)(j+'A'));
				printf(" %c", (char)(j+'A'));
			}
			fprintf(outputFile,"\n");
			printf("\n");
			
			int k=0;
			if(hall->height<10){
			 	k = 5;
			}else if(hall->height<100){
				k = 4;
			}
			
			for(j=0;j<hall->width-k;j++){			
				fprintf(outputFile," ");
				printf(" ");
			}
			fprintf(outputFile,"C U R T A I N\n");
			printf("C U R T A I N\n");
		}
	}
	/*shows all statistics in cinema*/
	void statistics(struct hall * hall,int count,FILE *outputFile){
		
		int student=0,fullfare=0;
		int money=0;
		int i,j;
		
		if(count==0){
			fprintf(outputFile,"STATISTICS");
			printf("STATISTICS\n");
		}
		if(hall->end==0){
			return;
		}else{
			statistics(hall->next,++count,outputFile);		/*go to end of the list and print them out from it to the start*/
			fprintf(outputFile,"\n");
			for(i=0;i<hall->height;i++){
				for(j=0;j<hall->width;j++){
					if(hall->seatStatus[i][j]=='s'){
						student++;
					}else if(hall->seatStatus[i][j]=='f'){
						fullfare++;
					}
				}
			}
			money= (student*STUDENT)+(fullfare*FULLFARE);
			fprintf(outputFile, "%s %d student(s), %d full fare(s), sum:%d TL" , hall->movieName,student,fullfare,money);
			printf("%s %d student(s), %d full fare(s), sum:%d TL\n" , hall->movieName,student,fullfare,money);
			
		}		
	}

