#include <string>
#include <fstream>
#include <iostream>

#include "../global.h"
#include "Weather.h"
#include "Date.h"

Weather::Weather(const std::string& file,
                 const int lines) 
         : filename(file), 
           lines_to_read(lines) {
   /* load data from file and make sure it's valid */
   ASSERT( this->load() );

   /* add data descriptions */
   this->set_target_names();
   this->set_feature_names();
   this->set_station_data();

   /* set member data defaults */
   this->observation_start = Date(2017, 10, 23);
}

int Weather::number_of_entries() const {
   return 1;
}

bool Weather::load() {
   /*
    # open file
   weatherData = []
   with open(self.wfile) as myfile:
       if (self.lines > 0):
           weatherData = [next(myfile) for x in xrange(self.lines + header)]
       else:
           weatherData = myfile.readlines()

   # get data stream from first line
   streamHeader = weatherData.pop(0).rstrip()
   if (streamHeader == 'FULL'):
       self.dataStream = 0
   elif (streamHeader == 'ADVANCED'):
       self.dataStream = 1
   elif (streamHeader == 'BASIC'):
       self.dataStream = 2
   else:
       print "Error: unecognised data stream from file %s" % (self.wfile)
       return -1

   # read data
   inputData = []
   for line in weatherData:
        entries = line.split()
        inputData.append(entries)

   # copy all into np array
   self.data = np.array(inputData)
   */
   const int non_data_header_lines = 1;
   std::ifstream filestream(this->filename);
   ASSERT( filestream.is_open() );

   std::string line;
   while (getline(filestream, line)) {

   }
   return true;
}  

void Weather::set_target_names() {

}

void Weather::set_station_data() {

}

void Weather::set_feature_names() {

}