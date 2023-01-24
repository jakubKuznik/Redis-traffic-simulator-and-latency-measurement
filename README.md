Program will write 650 000 records of 9KB size to Redis database that is running on local-host 
All key-values will be writen on STDOUT separated by ';'
these values are set as constant in redis-value-generator.c

## Install library. 

`sudo yum install epel-release` \
`sudo yum install hiredis-devel`

## build 
`make` 

## run 
All the key values are writen to stdout. \
`redis-generate-values.c > keys` 


