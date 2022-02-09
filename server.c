#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <pthread.h>


char wiadomosc_py[2000];
char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct wiadomosc
{
   int id; 
   char wiadomosc[1000]; 
   char autor[50]; 
};

struct chat
{
   int id; 
   int uzytkownik1; 
   int uzytkownik2; 
   struct wiadomosc wiadomosci[100]; 
};


struct uzytkownik
{
   char uzytkownik[100]; 
   char haslo[100]; 
   int id; 
   int zalogowany; 
};



struct uzytkownik uzytkownicy[20]; 
struct chat chats[100]; 

int start(){
   for(int i=0; i<20; i++){
      uzytkownicy[i].id = -1;
      uzytkownicy[i].zalogowany = 0;
      strcpy(uzytkownicy[i].uzytkownik, "");
      strcpy(uzytkownicy[i].haslo, "");
   }
   for(int i=0; i<100; i++){
      chats[i].id = -1;
      chats[i].uzytkownik1 = -1;
      chats[i].uzytkownik2 = -1;
      for(int j=0; j<100; j++){
         chats[i].wiadomosci[j].id = -1;
         strcpy(chats[i].wiadomosci[j].autor, "");
         strcpy(chats[i].wiadomosci[j].wiadomosc, "");
      }
   }
};



void wylogowanie(int id){
   for (int i = 0; i< 20; i++){
      if (uzytkownicy[i].id == id){
         uzytkownicy[i].zalogowany = 0;
         break;
      }
   }
}


char* getData(char id_user[]){
   char *file = malloc (sizeof (char) * 10000); 
   strcpy(file, "");
   char begin[] = "{\"znajomi\":["; 
   strncat(file, begin, strlen(begin));

   char file_uzytkownik[] = "{\"uzytkownik\":\"";
   char file_zalogowany[] = "\",\"zalogowany\":";
   char file_True[] = "true";
   char file_False[] = "false";
   char file_wiadomosci[] = ",\"wiadomosci\":[";
   char file_id_wiadomosci[] = "{\"id_wiadomosci\":";
   char file_autor[] = ",\"autor\":\"";
   char file_text[] = "\",\"wyslany_tekst\":\"";
   char file_text_end[] = "\"},";
   char file_end[] = "]},";
   int id_znaj = 0;
   char user_znaj[50];
   char id_wiad[10];
   for(int i=0; i<100; i++){
      if(chats[i].uzytkownik1 == atoi(id_user)){
         id_znaj = chats[i].uzytkownik2;
      }else if(chats[i].uzytkownik2 == atoi(id_user)){
         id_znaj = chats[i].uzytkownik1;
      }

      if (id_znaj != 0){ 
         strcpy(user_znaj, uzytkownicy[id_znaj-1].uzytkownik);
         strncat(file, file_uzytkownik, strlen(file_uzytkownik));
         strncat(file, user_znaj, strlen(user_znaj));
         strncat(file, file_zalogowany, strlen(file_zalogowany));
         if (uzytkownicy[id_znaj-1].zalogowany == 1){
            strncat(file, file_True, strlen(file_True));
         }else{
            strncat(file, file_False, strlen(file_False));
         }
         strncat(file, file_wiadomosci, strlen(file_wiadomosci));
         for (int j = 0; j< 100; j++){
            if (chats[i].wiadomosci[j].id != -1){
               strncat(file, file_id_wiadomosci, strlen(file_id_wiadomosci));
               sprintf(id_wiad, "%d", chats[i].wiadomosci[j].id);
               strncat(file, id_wiad, strlen(id_wiad));
               strncat(file, file_autor, strlen(file_autor));
               strncat(file, chats[i].wiadomosci[j].autor, strlen(chats[i].wiadomosci[j].autor));
               strncat(file, file_text, strlen(file_text));
               strncat(file, chats[i].wiadomosci[j].wiadomosc, strlen(chats[i].wiadomosci[j].wiadomosc));
               strncat(file, file_text_end, strlen(file_text_end));
               memset(&id_wiad, 0, sizeof(id_wiad));
            }
         }
         strncat(file, file_end, strlen(file_end));
         memset(&id_wiad, 0, sizeof(id_wiad));
      }
      id_znaj = 0;
      memset(&user_znaj, 0, sizeof (user_znaj));
   }

   char end[] = "]}"; 
   strncat(file, end, strlen(end));

   return file; 
};


int rejestracja(char uzytkownik[], char haslo[]){
   for (int i=0; i<20; i++){
      if (strcmp (uzytkownicy[i].uzytkownik, uzytkownik) == 0 ){
         return -1;
      }
      if (uzytkownicy[i].id == -1){
         uzytkownicy[i].id = i+1;
         strcpy(uzytkownicy[i].uzytkownik, uzytkownik);
         strcpy(uzytkownicy[i].haslo, haslo);
         return uzytkownicy[i].id;
      }
   }

   return -1;
}


int logowanie(char uzytkownik[], char haslo[]){
   for(int i=0; i<20; i++){
      if (uzytkownicy[i].id > -1 && strcmp(uzytkownicy[i].uzytkownik, uzytkownik) == 0 && strcmp(uzytkownicy[i].haslo, haslo) == 0 && uzytkownicy[i].zalogowany == 0){
         uzytkownicy[i].zalogowany = 1;
         return uzytkownicy[i].id;
      }
   }
   return -1;
}


int dod_znaj(char uzytkownik[], char user_id[]){
   int id_znaj = 0;
   if (strcmp(uzytkownicy[atoi(user_id)-1].uzytkownik, uzytkownik)==0){
      return -1;
   }

   for(int i=0; i< 20; i++){
      if(uzytkownicy[i].id > 0 && strcmp(uzytkownicy[i].uzytkownik, uzytkownik) == 0){
         id_znaj = uzytkownicy[i].id;
         break;
      }
   }

   if (id_znaj == 0)return -1;
   for(int i=0; i<100; i++){
      if(chats[i].uzytkownik1 == atoi(user_id) && chats[i].uzytkownik2 == id_znaj){
         return -1;
      }else if(chats[i].uzytkownik2 == atoi(user_id) && chats[i].uzytkownik1 == id_znaj){
         return -1;
      }
   }
   for(int i=0; i<100; i++){
      if(chats[i].id == -1){
         chats[i].id = i+1;
         chats[i].uzytkownik1 = atoi(user_id);
         chats[i].uzytkownik2 = id_znaj;
         return 0;
      }
   }

   return -1;
}


int wysylanie(char uzytkownik[], char user_id[], char wiadomosc[]){
   int id_znaj = 0;
   char autor[50];

   
   for(int i=0; i< 20; i++){
      if(uzytkownicy[i].id > 0 && strcmp(uzytkownicy[i].uzytkownik, uzytkownik) == 0){
         id_znaj = uzytkownicy[i].id;
      }
      if(uzytkownicy[i].id == atoi(user_id)){
         strcpy(autor, uzytkownicy[i].uzytkownik);
      }
   }
   if (id_znaj == 0)return -1;
   int chat_id = 0;

   
   for(int i=0; i<100; i++){
      if(chats[i].uzytkownik1 == atoi(user_id) && chats[i].uzytkownik2 == id_znaj){
         chat_id = chats[i].id;
      }else if(chats[i].uzytkownik2 == atoi(user_id) && chats[i].uzytkownik1 == id_znaj){
         chat_id = chats[i].id;
      }
      if (chat_id > 0){
         printf("Chat id: %d\n", chat_id);
         for (int j=0; j<100; j++){
            if(chats[i].wiadomosci[j].id == -1){
               chats[i].wiadomosci[j].id = j + 1;
               strcpy(chats[i].wiadomosci[j].autor, autor);
               strcpy(chats[i].wiadomosci[j].wiadomosc, wiadomosc);
               return 0;
            }
         }
         return -1;
      }
   }

   if (chat_id == 0)return -1;
}


void * socketThread(void *arg)
{
   printf("Nowy uzytkownik \n");
   int newSocket = *((int *)arg);
   int n;
   char username_tag[] = "USERNAME";
   char password_tag[] = "PASSWORD";
   char length_tag[] = "LENGTH";
   char message_tag[] = "MESSAGE";
   char start_tag[] = "START";
   char end_tag[] = "END";
   int id = -1;
   for(;;){
      n=recv(newSocket , wiadomosc_py , 2000 , 0);
      printf("%s\n",wiadomosc_py);
      if(n<1){
         break;
      }

      char *message = malloc(sizeof(wiadomosc_py));
      strcpy(message,wiadomosc_py);

      if (strcmp(message, "Logowanie") == 0 && id < 0){
         send(newSocket,username_tag,sizeof(username_tag),0);
         memset(&wiadomosc_py, 0, sizeof (wiadomosc_py));
         n=recv(newSocket , wiadomosc_py , 2000 , 0);
         printf("%s\n",wiadomosc_py);
         if(n<1){
            break;
         }
         char *login = malloc(sizeof(wiadomosc_py));
         strcpy(login, wiadomosc_py);
         send(newSocket,password_tag,sizeof(password_tag),0);
         memset(&wiadomosc_py, 0, sizeof (wiadomosc_py));
         n=recv(newSocket , wiadomosc_py , 2000 , 0);
         printf("%s\n",wiadomosc_py);
         if(n<1){
            break;
         }
         char *password = malloc(sizeof(wiadomosc_py));
         strcpy(password, wiadomosc_py);
         id = logowanie(login, password);
         if (id > 0){
            send(newSocket,"TAK",4,0);
         }else{
            send(newSocket,"NIE",3,0);
         }
      }else if(strcmp(message, "Wysylanie") == 0 && id > 0){
         send(newSocket,username_tag,sizeof(username_tag),0);
         memset(&wiadomosc_py, 0, sizeof (wiadomosc_py));
         n=recv(newSocket , wiadomosc_py , 2000 , 0);
         printf("%s\n",wiadomosc_py);
         if(n<1){
            break;
         }
         char *username = malloc(sizeof(wiadomosc_py));
         strcpy(username, wiadomosc_py);
         send(newSocket,length_tag,sizeof(length_tag),0);
         memset(&wiadomosc_py, 0, sizeof (wiadomosc_py));
         n=recv(newSocket , wiadomosc_py , 2000 , 0);
         printf("%s\n",wiadomosc_py);
         if(n<1){
            break;
         }
         int length = atoi(wiadomosc_py);
         send(newSocket,message_tag,sizeof(message_tag),0);
         memset(&wiadomosc_py, 0, sizeof (wiadomosc_py));
         n=recv(newSocket , wiadomosc_py , length , 0);
         printf("%s\n",wiadomosc_py);
         if(n<1){
            break;
         }
         char *message = malloc(sizeof(wiadomosc_py));
         strcpy(message, wiadomosc_py);
         char id_string[10];
         sprintf(id_string, "%d", id);
         if (wysylanie(username, id_string, message) == 0){
            send(newSocket, "TAK", 4, 0);
         }else{
            send(newSocket, "NIE", 3, 0);
         }
      }else if(strcmp(message, "Rejestracja") == 0 && id < 0){
         send(newSocket,username_tag,sizeof(username_tag),0);
         memset(&wiadomosc_py, 0, sizeof (wiadomosc_py));
         n=recv(newSocket , wiadomosc_py , 2000 , 0);
         printf("%s\n",wiadomosc_py);
         if(n<1){
            break;
         }
         char *login = malloc(sizeof(wiadomosc_py));
         strcpy(login, wiadomosc_py);
         send(newSocket,password_tag,sizeof(password_tag),0);
         memset(&wiadomosc_py, 0, sizeof (wiadomosc_py));
         n=recv(newSocket , wiadomosc_py , 2000 , 0);
         printf("%s\n",wiadomosc_py);
         if(n<1){
            break;
         }
         char *password = malloc(sizeof(wiadomosc_py));
         strcpy(password, wiadomosc_py);
        if (rejestracja(login, password) == -1){
          send(newSocket,"NIE",3,0);
        }else{
          send(newSocket,"TAK",4,0);
        }

      }else if(strcmp(message, "Dod_znaj") == 0 && id > 0){
         send(newSocket,username_tag,sizeof(username_tag),0);
         memset(&wiadomosc_py, 0, sizeof (wiadomosc_py));
         n=recv(newSocket , wiadomosc_py , 2000 , 0);
         printf("%s\n",wiadomosc_py);
         if(n<1){
            break;
         }
         char *login = malloc(sizeof(wiadomosc_py));
         strcpy(login, wiadomosc_py);
         char id_string[10];
         sprintf(id_string, "%d", id);
         if (dod_znaj(login, id_string) != 0){
            send(newSocket, "NIE", 3, 0);
         }else{
            send(newSocket, "TAK", 4, 0);
         }
      }else if(strcmp(message, "Reload") == 0 && id > 0){
         char id_string[10];
         sprintf(id_string, "%d", id);
         char *JSONresponse = getData(id_string);
         send(newSocket, JSONresponse, strlen(JSONresponse),0);
         free(JSONresponse);
      }
      else{
         send(newSocket,"NIE",3,0);
      }
      memset(&wiadomosc_py, 0, sizeof (wiadomosc_py));
      }
      printf("Użytkownik wylogowany! \n");
      if (id > 0) wylogowanie(id);
      pthread_exit(NULL);
}


int main(){
   int serverSocket, newSocket;
   struct sockaddr_in serverAddr;
   struct sockaddr_storage serverStorage;
   socklen_t addr_size;


   start();
   printf("START\n");

   serverSocket = socket(PF_INET, SOCK_STREAM, 0);

   
  
   serverAddr.sin_family = AF_INET;

   
   serverAddr.sin_port = htons(1100);

   
   serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);


   
   memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

   
   bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

   
   if(listen(serverSocket,50)==0)
      printf("Nasłuchiwanie\n");
   else
      printf("Błąd\n");

  pthread_t thread_id;

  while(1)
  {
        
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
        printf("Podłączono.\n");
        if( pthread_create(&thread_id, NULL, socketThread, &newSocket) != 0 )
            printf("Błąd\n");

        pthread_detach(thread_id);
        
  }
   return 0;
}