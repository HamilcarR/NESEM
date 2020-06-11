#!/bin/bash

OK="OK" 

while IFS= read -r line;do
	num=$(echo $line | awk -F ' ' '{print $1}') ; 	
	mnemonic=$(echo $line | awk -F ' ' '{print $2}') ; 	
	opcode=$(echo $line | awk -F ' ' '{print $3}') ; 

	neslogline=$(sed -n "$num"p $2) ;
	neslognum=$(echo $neslogline | awk -F ' ' '{print $1}') ; 
	neslogopcode="0x$(echo $neslogline | awk -F ' ' '{print $3}')" ; 
	echo "$neslogline" ;	
	if [ $num != $neslognum ] || [ $opcode != $neslogopcode ]
	then 


		echo -e "\e[31m$neslogline\e[0m\n";	
		echo -e "             \e[31m$line\e[0m" ; 
		OK="\e[31mKO\e[0m";
		break;
	fi 

done < $1


echo -e $OK
