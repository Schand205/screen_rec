#include "functions.h"

bool isShiny (std::string name) {
	bool shiny_found = false;
	std::string wtd = "Shiny";
	for(int i = 0; i < name.length(); i++) {
		if(name[i] == 's' || name[i] == 'S') {
			for(int j = 1; j < wtd.length()-1; j++) {
				shiny_found = true;
				if(wtd[j] != name[i+j]) {
					shiny_found = false;
					break;
				}
			}
			if(shiny_found) return true;
		}
	}
	return false;
}
	
