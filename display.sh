#!/bin/bash

#just a script finding a line in a file using hex numbers
hexline=$1
bin=$(( 16#$hexline ))

sed -n "$bin"p $2 
