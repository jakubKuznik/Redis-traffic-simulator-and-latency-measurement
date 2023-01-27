// author:	 Jakub Kuznik
// execution:   ./redis-latency-measure keys 
// description: Program measure latency on redis GET querry each WAIT_SECONDS. 
// 		All latencies will be writen to stdout separated by \n. 
//
// 		also it sends GETS_P_S every second to random keys. 
// 		keys should be enter as csv file   

// ouput format: 
// Thu Jan 26 16:33:48 2023 TQS=8991 avg=45 max=2200 avgStart=47 maxStart=14154 appendOnlyAof=0 pdpDumpRdb=0 pdpRedisLog=12
// Thu Jan 26 16:33:49 2023 TQS=9142 avg=44 max=2534 avgStart=46 maxStart=14154 appendOnlyAof=0 pdpDumpRdb=0 pdpRedisLog=12

/*
  TQS ........... Querries p.s 
  avg ........... avg latency in microseconds (in last second)
  max ........... max latency in microseconds (in last second)
  avgStart ...... avg latency (in whole) 
  maxStart ...... maximal latency (in whole)
  appendOnlyAof . redis append only file size 
  pdpDumpRdb .... redis dump rdb file size 
  pdpRedisLog ... redis log file size 
*/

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <hiredis/hiredis.h>
#include <unistd.h> 
#include <fstream>
 	   
 
using namespace std;

#define GETS_P_S 6000    // how many gets will be there p.s 
#define APPEND_ONLY_FILE "/opt/pcf/pdp-redis/log/appendonly.aof"
#define PDP_DUMP "/opt/pcf/pdp-redis/log/pdp-dump.rdb"
#define REDIS_LOG "/opt/pcf/pdp-redis/log/pdp-redis.log"

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

// count latency for GET
//   @context ... redis database context 
//   @key      ... one redis database key  
long long latencyMeasure(redisContext *context, char *key){
  
  redisReply *reply;
  // start time measure 
  auto timeStart = chrono::high_resolution_clock::now();  
    
  // redis get record 
  reply = (redisReply*)redisCommand(context, "GET %s",key);

  // end time measure 
  auto timeEnd = chrono::high_resolution_clock::now();
  long long duration = std::chrono::duration_cast<std::chrono::microseconds> (timeEnd - timeStart).count();
  
  // Once an error is returned the context cannot be reused 
  //  and you should set up a new connection.
  //         
  if (reply == NULL){
    context = connectToRD("localhost", 6379);
    return duration;
  }    
  
  freeReplyObject(reply); 
  return duration;
}

// increment and count proper values 
void statistic(int latency, int *tQ, int *tQS, long long *tL, long long *tLIS, int *m, int *mS){
  // increace all counters 
  (*tQ)++;
  (*tQS)++;
  (*tL)   += latency;
  (*tLIS) += latency;  

  // if maximum set 
  if (latency > *mS)
    *mS = latency;
  if (latency > *m)
    *m = latency;
}

// count avg values 
void countAvg(int tQ, int tQS, long long tL, long long tLIS, int *avg, int *avgS){
  *avg  = (int) (tL / tQ);
  *avgS = (int) (tLIS / tQS);  
}

// set all params to 0 
void resetCounters(long long *tLIS, int *tQS  ,int *mS){
  *tLIS = 0;
  *tQS  = 0;
  *mS   = 0;
}

// return string with current time 
char *currentTime(time_t cTime){
  char *t = ctime(&cTime);
  if ( t[strlen(t)-1] == '\n') 
    t[strlen(t)-1] = '\0';
  return t;
}

// return size of file
int fileSize(const char *fileName){
   ifstream in_file(fileName, ios::binary);
   in_file.seekg(0, ios::end);
   return in_file.tellg();
}

int main(int argc, char **argv){   
  // declaring argument of time()
  time_t cTime = time(NULL);  
  time_t prevTime = cTime; 

  // redis object that represent database  
  redisContext *context = connectToRD("localhost", 6379);

  // file from arguments with all the key values 
  FILE *keys = parseArgs(argc, argv); 
 
  long long latency         = 0; 
  int indication            = 0;
  float timeWait 	    = 1 / GETS_P_S; // convert gets p. s. to interval 

  int totalQuerries         = 0;	// total querriees  
  int totalQuerriesSec      = 0;  	// total querries in one second 
  long long totalLatency    = 0; 	// total latency  
  long long totalLatencyIS  = 0; 	// total latency in second 
  int avg	            = 0;	// avg total 
  int avgS	            = 0;	// avg in second
  int max 		    = 0;  	// max total
  int maxS 		    = 0;  	// max in one second  

  char key[20]; 			// redis database key 
  

  // count latency and write it to stdout 
  // 26.1. 15:20:21  6000msg avg=1ms max=5ms avgStart=2ms maxStart=100ms 
  // 		appendonlyAof=111 pdpDumpRdb=111 pdpRedisLog=111 
  while (true){ 
    // get key from redis database 
    getKey(keys, key);
    
    // update time 
    cTime = time(NULL); 	  
    // print values each second 
    if (prevTime != cTime){
      if (totalQuerries > 0){
        // count avg times 
        countAvg(totalQuerries, totalQuerriesSec, totalLatency, totalLatencyIS, &avg, &avgS);
	cout << currentTime(cTime) << " TQS=" << totalQuerriesSec 
             << " avg=" << avgS << " max=" << maxS << " avgStart=" << avg 
             << " maxStart=" << max << " appendOnlyAof=" << fileSize(APPEND_ONLY_FILE)
	     << " pdpDumpRdb=" << fileSize(PDP_DUMP) << " pdpRedisLog=" << fileSize(REDIS_LOG)  << endl;
      
        resetCounters(&totalLatencyIS, &totalQuerriesSec, &maxS);     
      }
    }
    prevTime = cTime;
    
    // count latency for GET 
    latency = latencyMeasure(context, key);

    // increment all necessery numbers for statistic 
    statistic(latency, &totalQuerries, &totalQuerriesSec, &totalLatency, &totalLatencyIS, &max, &maxS); 
   
    // liveness indicator 
    if (indication++ % 100000 == 0) 
      cerr << ".";
    
    // do GETS_P_S gets each second  
    usleep((unsigned int )(timeWait*1000000 ));
  }  

  redisFree(context);
  return 0;
}






