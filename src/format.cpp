#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    long h, m, s;
  	string time;
  
  	h = (seconds / 60 / 60);
  	m = (seconds/ 60) % 60;
  	s = seconds % 60;
  	
	if(s < 10 && m < 10){
		time = 
		 	std::to_string(h) + ":0" + 
		  	std::to_string(m) + ":0" +
	  		std::to_string(s);
	}
	else if(s < 10){
		time = 
		 	std::to_string(h) + ":" + 
		  	std::to_string(m) + ":0" +
	  		std::to_string(s);
	}
	else if(m < 10){
		time = 
		 	std::to_string(h) + ":0" + 
		  	std::to_string(m) + ":" +
	  		std::to_string(s);
	}
	else {
  		time = 
	  		std::to_string(h) + ":" + 
	  		std::to_string(m) + ":" +
	  		std::to_string(s);
	}

    return time; 
}