#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
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