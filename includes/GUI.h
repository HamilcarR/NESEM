#ifndef GUI_H
#define GUI_H

#include "Constants.h" 
#include <Graphics.hpp>
#include "Debug.h"

class Debug ; 
class CPUDEBUG; 

//debug class
class Window : public sf::RenderWindow{
public:
	struct SCREENDIV{
		sf::Rect<int> memory;
		sf::Rect<int> zeropage;
		sf::Rect<int> registers_state ;
		sf::Rect<int> current_instruction;


	};


	Window() : sf::RenderWindow(sf::VideoMode(900 , 600) , " ") 
	{
		width = 900 ; 
		height = 600 ;
		memory_page = 0x00; 
		initFont();
		setupFont();
		initScreen();
		setFramerateLimit(60); 
	}

	Window(int w , int h ):sf::RenderWindow(sf::VideoMode(w , h) , " ")  
	{
		width = w ; 
		memory_page = 0x00; 
		height = h ; 
		initFont();
		setupFont();
		initScreen();
		setFramerateLimit(60); 
	}

	
	virtual ~Window(){

	}

	void loop(const Debug* debug);

private:
	void process(sf::Keyboard::Key , const Debug*) ;  
	void setupFont() ;
	void setText(std::string , const sf::Rect<int>&);
	void initFont() ;
	void initScreen();
	sf::Font font ; 		
	sf::Text text ;
	SCREENDIV screen ;

	uint8_t memory_page ; 
	int width ; 
	int height ;


	
};














#endif
