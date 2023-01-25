// author: Jakub Kuznik
// Program will write 650 000 records of 9KB size to Redis database that is running on local-host 
// All key-values will be writen on STDOUT separated by ';'


#include <stdio.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>

#define RECORD_SIZE_BYTES 9216 // 9KB
#define RECORDS 650000 // 650 000 

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

// generate 9KB long string using rand 
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

int main(){   
 
  // redis object that represent database  
  redisContext *context = connectToRD("localhost", 6379);
  // string of size RECORD_SIZE_BYTES
  char *recordValue = generate9Kstring();
  // start on some random big value then increment 
  long long unsigned recordkey = 900000000;
  char strRecordKey[50];
  // init random number 
  srand (123); 

  // generate 
  for (int i = 0; i < RECORDS; i++){
    sprintf(strRecordKey, "%llu", recordkey); // convert to string key value  
    redisCommand(context, "SET %s %b", strRecordKey, recordValue, RECORD_SIZE_BYTES); 
    printf("%s;",strRecordKey); 
    recordkey += rand(); // adding random value to key
  }

  free(recordValue); 
  redisFree(context);
  return 0;
}

