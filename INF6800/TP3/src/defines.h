/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 **/



#ifndef DEFINES
#define DEFINES

#include <iostream>
#include <cstdio>


	// Color list
#define DEFAULT "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define ORANGE "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"

// Print terms
#define PRINT std::cout
#define END_PRINT DEFAULT<<std::endl


#define DEFAULT_DEBUG(Enonce)\
 	{\
 		PRINT<<BLUE<<"[DEFAULT]"<<Enonce<<END_PRINT;\
 	}


#define ERR_DEBUG(Enonce)\
 	{\
 		PRINT<<RED<<"[ERR]"<<Enonce<<END_PRINT;\
 	}

#endif