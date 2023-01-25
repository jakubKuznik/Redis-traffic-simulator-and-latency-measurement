The `redis-generate-values` will write 650 000 records of 9KB size to the Redis database that is running on the local-host All key values will be written on STDOUT separated by ';' these values are set as constant in redis-generate-values.c. The `redis-write-keys` will do about 6000 rewrites in the database in an infinite loop. Each database write will set a 9KB long string to existing key. `redis-write-keys` needs an input csv file with Redis keys.
## Install library. 

`sudo yum install epel-release` \
`sudo yum install hiredis-devel`

## build 
`make` 

## run 
All the key values are writen to stdout. \
`./redis-generate-values.c > keys` filling the database \
`./redis-write-keys keys` simmulate traffic 

