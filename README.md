The program will write 650 000 records of 9KB size to the Redis database that is running on the local-host All key values will be written on STDOUT separated by ';' these values are set as constant in redis-generate-value.c

## Install library. 

`sudo yum install epel-release` \
`sudo yum install hiredis-devel`

## build 
`make` 

## run 
All the key values are writen to stdout. \
`redis-generate-values.c > keys` 


