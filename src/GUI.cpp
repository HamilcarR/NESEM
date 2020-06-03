#include "../includes/GUI.h"









void Window::initScreen(){
	screen.zeropage = sf::Rect(0 , 0 , width/2 , height/3) ; 
	screen.memory = sf::Rect(0 , (height/2)+50 , width/2 , height/3) ; 
	screen.registers_state = sf::Rect(width - 130 , 0 , width/3 , height/5) ; 
	screen.current_instruction = sf::Rect(width - 130 , (height/2)+50 , width/3 , height/3) ;


}













void Window::process(sf::Keyboard::Key key , const Debug *debug){
using a = sf::Keyboard::Key ;

		const CPUDEBUG *cdebug = static_cast<const CPUDEBUG*>(debug) ; 
		switch (key) {
		
			case a::C :
				cdebug->cpu->clock(); 
			break;
			
			case a::Add :
				memory_page++ ; 
			break;

			case a::Subtract :
				memory_page--;
			break ; 

			case a::Escape : 
				close();
			break;
			

	  }

	



}









void Window::loop(const Debug *debug){

	const CPUDEBUG *cdebug = static_cast<const CPUDEBUG*>(debug) ; 
	std::ostringstream oss(std::ostringstream::out) ; 
	while(isOpen()){
		clear();
		sf::Event event ; 
		while(pollEvent(event)){
			switch(event.type){
				case sf::Event::Closed :
					close();
				break;

				case sf::Event::KeyPressed :
					process(event.key.code , debug) ; 
				break;

			}

		}
		oss << cdebug->print_zeropage();
		setText(oss.str() , screen.zeropage);
		oss.str(std::string());
		draw(text);

		oss << cdebug->print_memory(memory_page); 
		setText(oss.str() , screen.memory);
		oss.str(std::string());
		draw(text);

		oss << cdebug->print_registers(); 
		setText(oss.str() , screen.registers_state);
		oss.str(std::string());	
		draw(text);	

		oss << cdebug->current_instruction();
		setText(oss.str() , screen.current_instruction);
		oss.str(std::string());
		draw(text);	

		display();
	while(static_cast<const CPUDEBUG*>(debug)->cpu->ticks > 0) 
		static_cast<const CPUDEBUG*>(debug)->cpu->clock(); 


	}




}


void Window::setText(std::string t , const sf::Rect<int>& position){
	text.setString(t);
	text.setPosition(position.left , position.top);
}


void Window::setupFont() {
		
	text.setFont(font);
	text.setCharacterSize(10); 
	text.setFillColor(sf::Color::Yellow);
	





}
	



void Window::initFont(){
	if(!font.loadFromFile("font/font.ttf"))
		std::cerr << " error loading font" << "\n" ; 
	
	
}















