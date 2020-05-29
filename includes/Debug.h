#ifndef DEBUG_H
#define DEBUG_H

#include "Constants.h"
#include "CPU.h" 









class Debug{
public:

	virtual void print_registers() const = 0 ; 
	virtual void loop() const = 0 ; 

	
};



class CPUDEBUG : public Debug{
public:
	CPUDEBUG(CPU* c){ cpu = c ; } 
	virtual ~CPUDEBUG(){}
	virtual void print_registers() const ; 
	virtual void loop() const ; 
	virtual void current_instruction() const ; 

	CPU* cpu ; 





};
































#endif
