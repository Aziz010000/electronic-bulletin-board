#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"messageLib.h"
#include"helper.h"

#define SIZE 1024





Message *createMessage(char *object, char *text, char *id)
{
	Message *myMessage = malloc(sizeof(Message)); 
	if(myMessage == NULL){
		fprintf(stderr, "Error in malloc occured!"); 
		exit(EXIT_FAILURE); 
	}
	strncpy(myMessage->object, object, strlen(object)); 
	myMessage->object[strlen(object)] = 0; 
	strncpy(myMessage->text, text, strlen(text)); 
	myMessage->text[strlen(text)] = 0; 
	strncpy(myMessage->idMessage, id, strlen(id)); 
	myMessage->idMessage[strlen(id)] = 0; 
	myMessage->value = 1; 
	return myMessage; 
}


void printMessage(Message *myMessage)
{
	if(myMessage){
		printf("Object is %s text is %s\n", myMessage->object, myMessage->text); 
	}
}



User *createUser(char *username, char *password)
{
	User *myUser = malloc(sizeof(User)); 
	if(myUser == NULL){
		fprintf(stderr, "Error in malloc occured!"); 
		exit(EXIT_FAILURE); 
	}

	strncpy(myUser->username, username, strlen(username)); 
	myUser->username[strlen(username)] = 0; 
	strncpy(myUser->password, password, strlen(password)); 
	myUser->password[strlen(password)] = 0; 
	myUser->count = 0; 
	myUser->messages = NULL; 
	myUser->next = NULL; 
	return myUser; 
}

void printUser(User *myUser)
{
	if(myUser){
		printf("This is the user %s\n", myUser->username); 
		for(int i = 0; i < myUser->count; i++){
			printf("%s\n", myUser->messages[i]->object); 
			printf("%s\n", myUser->messages[i]->text); 
		}
	}
}


void addUser(BulletinBoard *myBoard, char *username, char *password)
{
	User *newUser = createUser(username, password); 
	User *current;  
	

	
	if( myBoard->head== NULL)
	{
		myBoard->head = newUser; 
		return; 
	}
	
	current = myBoard->head; 
	
	while(current->next)
	{
		current = current->next; 
	}
	
	current->next = newUser; 

}

void addMessage(User *myUser, char *object, char *text, char *idMessage){
	Message *myMessage = createMessage(object, text, idMessage); 
	myUser->messages = realloc(myUser->messages, sizeof(Message)*++myUser->count); 
	if(myUser->messages == NULL){
	}
	myUser->messages[myUser->count-1] = myMessage; 
}

void addMessageUser(BulletinBoard *myBoard, char *username, char *object, char *text, char *file){
	
	char idMessage[SIZE_MESSAGE_ID+1]; 
	User *current = myBoard->head; 
	
	snprintf(idMessage, sizeof(idMessage), "%06d",++myBoard->idCount); 
	++myBoard->msgCount; 
	
	while(current){
		if(strcmp(current->username, username) == 0){
			break; 
		}
		current = current->next; 
	}
	if(current){
		addMessage(current, object, text,idMessage); 
	}
	
	wrMessage(username, object, text, idMessage, file);


}

//questa elimina dato un indice, è la funzione di livello superiore che se la chiama le fornisce l'indice da eliminare
void delMessageByIndex(User *myUser, int index){
	Message *myMessage = myUser->messages[index]; 
	
	for(int i = index; i<myUser->count-1; i++)
	{
		myUser->messages[i] = myUser->messages[i+1]; 
	}
	myUser->messages = realloc(myUser->messages, sizeof(Message)*--myUser->count); 
	free(myMessage); 
}

User *findUser(BulletinBoard *myBoard, char *username){
	User *current = myBoard->head; 	
	
	while(current)
	{ 
		if(strcmp(current->username, username) == 0){
			break; 
		}
		current = current->next; 
	}
	if(current)return current; 
	return NULL; 

}

//questa funzione può fallire hai dato un oggetto non presente e quindi restituisce -1
int delMessageUser(BulletinBoard *myBoard, char *username, char *idMessage){
	User *myUser = findUser(myBoard, username); 
	if(myUser == NULL) return -1; 
	for(int i = 0; i<myUser->count; i++)
	{
		if(strcmp(myUser->messages[i]->idMessage, idMessage) == 0)
		{
			delMessageByIndex(myUser, i); 
			--myBoard->msgCount;
			return 0; 
		}
	}
	return -1; 
}

void putZero(FILE *f, int pos)
{
	if (fseek(f, pos, SEEK_SET) != 0) {
            perror("Errore con fseek");
            fclose(f);
        }
        
        //when something goes wrong fputc returns EOF 
        if (fputc('0', f) == EOF) {
            perror("Errore nella scrittura del carattere");
            fclose(f);
        }
       fseek(f, pos+2, SEEK_SET); 

}

//ATTENZIONE LA LIBRARY DI MESSAGE LIB STA DIVETANDO UN CASINO TROPPA DIPENDENZA DAI FILE BISOGNA DISACCOPIARE LE LIBRARY UNA PER LA LIB IN MEMORIA E UNA PER I FILE E USARE IL SERVER CHE CHIAMA UNA FUNZIONE PER METTERLE IN COMUNICAZIONE




int equalIdMessage(char *buffer, char *idMessage)
{
		char *token = strtok(buffer,",");
		char *pen = NULL; 
		char *last = NULL; 
		
		
		while(token)
		{
			pen = last; 
			last = token; 
			token = strtok(NULL, ","); 
		
		}
			
		removeQuotes(pen); 


	if(strcmp(idMessage, pen) == 0 )
	{
		return 0; 
	
	}
	return -1; 

}

//this goes to a separate file
void delMessageFile(FILE *f , char *idMessage)
{
	int pos; 
	char buffer[SIZE]; 
	
	while(fgets(buffer, sizeof(buffer), f) )
	{
		
		if(equalIdMessage(buffer, idMessage) == 0)
		{
			pos = ftell(f) -2; 
			putZero(f,pos); 
		
		}
	}

}




void printUserMessage(BulletinBoard *myBoard, char *username)
{
	User *myUser = findUser(myBoard, username); 
	if(myUser){
		printf("I messaggi di %s sono :\n", username); 
		for(int i = 0; i<myUser->count; i++){
			printf("Object %s\n", myUser->messages[i]->object); 
			printf("Text %s\n", myUser->messages[i]->text); 
			printf("Id %s\n", myUser->messages[i]->idMessage);
		}
	
	}else{
		printf("L'utente non dispone di messagi\n"); 
	}
}

//this stuff is awful i know
//you should do it in an appropriate function present in the file functionsever
void fillUsers(BulletinBoard *myBoard, char *file)
{
	char buffUser[SIZE_USERNAME]; 
	char buffPass[SIZE_PASSWORD]; 
	char buff[SIZE]; 
	
	FILE *myFile = fopen(file, "r"); 
	if(myFile == NULL){
		fprintf(stderr, "Error opening file!"); 
		exit(EXIT_FAILURE); 
	
	}
	while(fgets(buff, 1024, myFile)){
		fill(buff,buffUser, buffPass);
		addUser(myBoard, buffUser, buffPass); 
	} 
	fclose(myFile); 
} 

int checkUserPass(BulletinBoard *myBoard, char *username, char *password)
{
	User *current = myBoard->head;
	
	while(current)
	{
		if( strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0) return 0; 
		current = current->next; 
	}
	return -1; 
}

void visualizeUsers(BulletinBoard *myBoard)
{
	User *current = myBoard->head; 
	while(current)
	{
		printf("Presente user %s\n", current->username); 
		printUser(current); 
		current = current->next; 
	}
}

void addMessageUserOld(BulletinBoard *myBoard, char *username, char *object, char *text, char *idMessage)
{
	User *current = myBoard->head; 

	while(current)
	{
		if(strcmp(current->username, username) == 0){
			break; 
		}
		current = current->next; 
	}
	if(current){
		addMessage(current, object, text,idMessage); 
	}


}


//also this stuff
void fillMessagesUsers(BulletinBoard *myBoard, char *file)
{
	char buffUser[SIZE_USERNAME+1]; 
	char buffObj[SIZE_OBJECT+1]; 
	char buffText[SIZE_TEXT+1]; 
	char buffIdMessage[SIZE_MESSAGE_ID+1]; 
	char buff[SIZE]; 
	int v;
	int numTemp;  
	
	
 	FILE *myFile = fopen(file, "r"); 
 	if(myFile == NULL
 	){
 		fprintf(stderr, "Error opening file\n"); 
 		exit(EXIT_FAILURE); 
 	}
 	
	while(fgets(buff, SIZE, myFile))
	{
		fillMsg(buff, buffUser, buffObj, buffText, buffIdMessage, &v);
		if(v!= 0){
			addMessageUserOld(myBoard, buffUser, buffObj, buffText, buffIdMessage); 
			++myBoard->msgCount; 
			numTemp = strtol(buffIdMessage, NULL, 10); 
			if(numTemp > myBoard->idCount) myBoard->idCount = numTemp; 
		}
	}
}

BulletinBoard *createBulletinBoard()
{
	BulletinBoard *myBoard = malloc(sizeof(BulletinBoard)); 
	if(myBoard == NULL)
	{
		fprintf(stderr, "Error in malloc occured\n"); 
		exit(EXIT_FAILURE); 
	}
	myBoard->head = NULL; 
	myBoard->msgCount = 0; 
	myBoard->idCount = 0; 
	return myBoard; 
}

