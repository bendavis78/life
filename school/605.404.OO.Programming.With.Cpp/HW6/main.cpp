#include <iostream>
using std::cout;
using std::endl;
#include <fstream>
#include <vector>
#include <string>
#include <locale>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/cstdint.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>

#include "elevator.h"
#include "building.h"
#include "passenger.h"
#include "floor.h"

int main()
{
   std::list<Passenger> passengers;
   Building* building = Building::getInstance();
   // Open the file
   std::ifstream myfile("HW6-Elevators.csv");
   std::string line;
   if(myfile.is_open())
   {
      while(!myfile.eof())
      {
         std::getline(myfile, line);
         // Simple validation
         // Line must have characters
         // and first ones need to be numbers
         // Short-circuits to avoid dereferencing line[0] when invalid
         if(line.length() <= 1 ||
            !std::isdigit(line[0])
           )
         {
            continue;
         }
         // Trim the line
         boost::trim(line);
         // Use Boost.Tokenizer to parse the line
         boost::tokenizer< boost::escaped_list_separator< char > > tok(line);
         boost::tokenizer< boost::escaped_list_separator< char > >::iterator
            iter = tok.begin();
         // Set the start time
         uint16_t time = boost::lexical_cast<uint16_t>(*iter++);
         // Next is the starting floor
         uint16_t start_floor = boost::lexical_cast<uint16_t>(*iter++);
         // Finally, set the ending floor
         uint16_t end_floor = boost::lexical_cast<uint16_t>(*iter++);
         // Add it to the list
         Passenger p(time, start_floor, end_floor);
         passengers.push_back(p);
      }
   }

   // Start ticker
   uint64_t ticker = 0;
   while(!passengers.empty())
   {
      if(passengers.front().getTime() == ticker)
      {
         // Process until all passengers for this timestamp are through
         while(passengers.front().getTime() == ticker)
         {
            cout << "Processing " << passengers.front() << endl;
            Floor& floor = building->getFloor(passengers.front().getStartFloor());
            floor.addPassenger(passengers.front());
            // When we add a passenger, trigger an alert to the elevators
            // Each one will then respond to the request
            building->alertElevators(floor.getNumber());
            passengers.pop_front();
         }
      }
      // Step the elevators
      building->stepElevators();
      ++ticker;
   }
}
