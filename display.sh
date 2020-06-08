#!/bin/bash


hexline=$1
bin=$(( 16#$hexline ))

sed -n "$bin"p nestest.log 
