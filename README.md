## Motivation 
redis-benchmark utility does provide the same functionality as programs in this repository. But redis-benchmark doesn't behave as typical database traffic! 

Here you can specify how many SETs/GETs (p.s.) and how big string you want to write into the database. Also, this repository provides a tool for latency measurement.  

## Functionality
Everything can be set up using program constants. 

### redis-generate-values 
The `redis-generate-values` will write records of X KB size to the Redis database that is running on the local-host All key values will be written on STDOUT separated by ';'

### redis-write-keys 
The `redis-write-keys` will do about Y rewrites in the database in an infinite loop. Each database write will set a Z KB long string to existing key.

### redis-latency-measurement 
The `redis-latency-measure/redis-latency` measure latency on GET command. And also send W GETS p.s. It also gets information about size of important redis files. 
#### output format: 
```
Thu Jan 26 16:33:48 2023 TQS=8991 avg=45 max=2200 avgStart=47 maxStart=14154 appendOnlyAof=0 pdpDumpRdb=0 pdpRedisLog=12
Thu Jan 26 16:33:49 2023 TQS=9142 avg=44 max=2534 avgStart=46 maxStart=14154 appendOnlyAof=0 pdpDumpRdb=0 pdpRedisLog=12
```
```
TQS ........... Querries p.s 
avg ........... avg latency in microseconds (in last second)
max ........... max latency in microseconds (in last second)
avgStart ...... avg latency (in whole) 
maxStart ...... maximal latency (in whole)
appendOnlyAof . redis append only file size 
pdpDumpRdb .... redis dump rdb file size 
pdpRedisLog ... redis log file size 
```
## Install library. 


`sudo yum install epel-release` \
`sudo yum install hiredis-devel`

## build 
`make` \
`cd redis-latency-measure` \
`make`

## run 
All the key values are writen to stdout. \
`./redis-generate-values.c > keys` filling the database \
`./redis-write-keys keys` simmulate SET traffic \
`./redis-latency keys` measure latency and simulate GET traffic 
