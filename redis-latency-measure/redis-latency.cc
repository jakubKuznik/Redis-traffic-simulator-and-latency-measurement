// author:	 Jakub Kuznik
// execution:   ./redis-latency-measure keys 
// description: Program measure latency on redis GET querry each WAIT_SECONDS. 
// 		All latencies will be writen to stdout separated by \n. 
//
// 		also it sends GETS_P_S every second to random keys. 
// 		keys should be enter as csv file   

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <hiredis/hiredis.h>
#include <unistd.h> 

using namespace std;

#define WAIT_SECONDS 0.1 // WAIT 0.1 BETWEEN EACH LATENCY MEASURE 
#define GETS_P_S 6000    // how many gets will be there p.s 

// Connect to redis database
// If error exit program 
redisContext * connectToRD(const char *hostname, const int port){
  
  redisContext *c = redisConnect(hostname, port);
  if (c != NULL && c->err) {
    cerr << "Error: " << c->errstr << endl;
    exit(1);
  } 
  
  fprintf(stderr, "Connected to Redis\n");
  return c; 
}

// parse args: there should be cvs file with all redis keys 
FILE *parseArgs(int argc, char **argv){
  FILE *f = NULL; 
  
  if ( argc != 2){
    cerr << "Error keys file missing in arguments\n";
    exit(0);
  }  
  
  f = fopen(argv[1], "r");
  if (f == NULL){
    cerr << "Error can't open file\n"; 
    exit(0);
  }
  
  return f;
}

// return key from FILE store it to key variable  
// file has to be csv 
void getKey(FILE *f, char *key){
  char c;
  int i = 0;
  int indicator = 0;
  
  while (c = fgetc(f)){
    if (c == EOF){
      fseek(f, 0, SEEK_SET); // set file pointer to begin     
    }
    if (c == ';'){
      key[i] = '\0';
      return; 
    }
    key[i++] = c;
  }
} 


int main(int argc, char **argv){   
  
  // redis object that represent database  
  redisContext *context = connectToRD("localhost", 6379);

  // file from arguments with all the key values 
  FILE *keys = parseArgs(argc, argv); 
 
  redisReply *reply;
  int indication 	= 0;
  int measure 		= GETS_P_S * WAIT_SECONDS; // how often there will be a latency measure 
  float timeWait 	= 1 / GETS_P_S;            // convert gets p. s. to interval 
  int measureCount 	= 0;			   // latency indicator 
  
  char key[20]; // redis database key 
  
  // every 0.1 second count latency and write it to stdout 
  while (true){ 
    // get key from redis database 
    getKey(keys, key);

    // count latency each WAIT_SECONS      
    if (measureCount++ % measure == 0){
      // start time measure 
      auto timeStart = chrono::high_resolution_clock::now();  
    
      // redis get record 
      redisAppendCommand(context,"GET %s",key);
      redisGetReply(context,(void**)&reply); // reply for SET
    
      // end time measure 
      auto timeEnd = chrono::high_resolution_clock::now();
      long long duration = std::chrono::duration_cast<std::chrono::microseconds> (timeEnd - timeStart).count();
 
      // log latency in microseconds 
      cout << duration << endl;  
    }
    
    // liveness indicator 
    if (indication++ % 100000 == 0) 
    	cerr << ".";
    
    // do GETS_P_S gets each second  
    usleep((timeWait*1000000));
  }  

  redisFree(context);
  return 0;
}






