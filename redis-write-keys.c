// author: Jakub Kuznik
//    ./redis-write-keys FILE 
//            FILE should containt redis key values separated by ';'  
// Program will randomly change redis records with 9KB long string  

#include <stdio.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>
#include <unistd.h> 

#define RECORD_SIZE_BYTES 9216 // 9KB

// Connect to redis database
// If error exit program 
redisContext * connectToRD(char *hostname, int port){
  
  redisContext *c = redisConnect(hostname, port);
  if (c != NULL && c->err) {
    printf("Error: %s\n", c->errstr);
    exit(1);
  } 
  
  fprintf(stderr, "Connected to Redis\n");
  return c; 
}

// generate 9KB long string using 
char * generate9Kstring(){
  char * out = malloc(RECORD_SIZE_BYTES);
  if (out == NULL){
    fprintf(stderr, "Malloc failed\n");
    exit(1);
  }
  
  for (int i = 0; i < RECORD_SIZE_BYTES; i++){
    out[i] = (rand() % 127);
  }
  return out;
}

// parse args: there should be cvs file with all redis keys 
void *parseArgs(int argc, char **argv){
  FILE *f = NULL; 
  
  if ( argc != 2){
    fprintf(stderr, "Error keys file missing in argument\n");
    exit(0);
  }  
  
  f = fopen(argv[1], "r");
  if (f == NULL){
    fprintf(stderr, "Error can't open file\n");
    exit(0);
  }
  
  return f;

}

int main(int argc, char **argv){   
  // file from arguments with all the key values 
  FILE *keys = parseArgs(argc, argv); 

  // init random number 
  srand (123);
  
  // redis object that represent database  
  redisContext *context = connectToRD("localhost", 6379);
  // string of size RECORD_SIZE_BYTES
  char *recordValue = generate9Kstring();
  // start on some random big value then increment 
  
 
  char c;
  int i = 0;
  int indicator = 0;
  char key[20];
  loop:
    while ((c = fgetc(keys)) != EOF){
      if (c == ';'){
        key[i] = '\0';
        c = fgetc(keys);
        i = 0;
        redisCommand(context, "SET %s %b", key, recordValue, RECORD_SIZE_BYTES);
        sleep(0.0001); // about 7000 p.s
      
        // program liveness indication 
        if (indicator++ % 100 == 0)
          printf("."); 
      } 
      key[i++] = c; 
    }
  fseek(keys, 0, SEEK_SET); // set file pointer on begining 
  goto loop;

  
  //redisCommand(context, "SET %s %b", strRecordKey, recordValue, RECORD_SIZE_BYTES); 

  free(recordValue); 
  redisFree(context);
  return 0;
}






