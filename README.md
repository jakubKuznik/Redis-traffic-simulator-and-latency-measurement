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
The `redis-latency-measure/redis-latency` measure latency on GET command. And also send W GETS p.s.

## Install library. 

`sudo yum install epel-release` \
`sudo yum install hiredis-devel`

## build 
`make` 
`cd redis-latency-measure`
`make`

## run 
All the key values are writen to stdout. \
`./redis-generate-values.c > keys` filling the database \
`./redis-write-keys keys` simmulate traffic \
`./redis-latency keys` measure latency 
