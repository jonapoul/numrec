import time
import sys
import datetime as dt
from dateutil.parser import parse
import pandas as pd
from Weather import *

# file choices
file_choices = [
   'data/basic.txt',
   'data/advanced.txt'
]
weatherFile = file_choices[0]
print 'Loading {0}'.format(weatherFile)
fileSlice = 0 # load all lines
weather = Weather(weatherFile, fileSlice)

# fill in the Relative Time field for all datapoints
print 'Setting relative times'
weather.setRelTime()

# zero any null gust measurements
print 'Modifying Gust'
newG = ['0' if g == '-99999' else g for g in weather.getFeatureData('Gust')]
weather.modify('Gust', newG)

# find the weather type at each datapoint's closest weather station (might be cheating?)
print 'Finding closest stations and their weather types'
all_ids = weather.getFeatureData('Station ID')
# list of all station id numbers
unique_ids = np.unique(all_ids).tolist()
# relative time value at every datapoint
times = weather.getFeatureData('Relative Time').astype(np.float)
feats = ['Relative Time', 'Weather Type']
# id number of each station's nearest station
closest_ids = [weather.findStations(coords=(s[2],s[3]),minThreshold=2,maxThreshold=300)[0][0] for s in weather.getStationData('all')]
# relative time and weather type of each datapoint
unique_closest_obs = []
# weather type at the closest weather station at the epoch of each datapoint
nearest_station_weather_type = []
for ID in closest_ids:
   obs = weather.getObservations(stationId=ID, features=feats).astype(np.float)
   unique_closest_obs.append(obs)

for i in range(weather.getNrEntries()):
   index = unique_ids.index(all_ids[i])
   nearest_station_id = closest_ids[index]
   nearest_obs = np.where( np.abs(unique_closest_obs[index][0] - times[i]) < 400)
   closest_obs = []
   closest_time = 1e10
   for n in nearest_obs:
      timediff = abs(n[0] - times[i])
      if timediff < closest_time:
         closest_time = timediff
         closest_obs = n
   nearest_station_weather_type.append(closest_obs[1])
weather.append('Nearest Station Weather Type', nearest_station_weather_type)

# After recovering any data ensure you run the `discard()` method to remove any data with remaining null observations.
print 'Discarding invalid records'
weather.discard()

# enumerate pressure trends
# F = Falling = -1, S = Steady = 0, R = Rising = +1
print 'Enumerating Pressure Trend'
pTType = ['F', 'S', 'R']
new_PT = [str(pTType.index(p) - 1) for p in weather.getFeatureData('Pressure Trend') ]
weather.modify('Pressure Trend', new_PT)

# enumerate wind direction, and set relative direction
print 'Enumerating Wind Direction'
compass = ['NNE','NE','ENE','E','ESE','SE','SSE','S','SSW','SW','WSW','W','WNW','NW','NNW','N']
new_WD     = [compass.index(w) for w in weather.getFeatureData('Wind Direction')]
weather.modify('Wind Direction', new_WD)

# westerliness ranges from -4 at East up to +4 at West
# northerliness from -4 at South, +4 at North
# each of these numbers lines up the corresponding index in compass[] list above
print 'Enumerating wind west/northerliness'
westerliness  = [-1, -2, -3, -4, -3, -2, -1,  0,  1,  2,  3, 4, 3, 2, 1, 0]
northerliness = [ 3,  2,  4,  0, -1, -2, -3, -4, -3, -2, -1, 0, 1, 2, 3, 4]
wind_west  = [westerliness[wd]  for wd in new_WD]
wind_north = [northerliness[wd] for wd in new_WD]
weather.append('Wind West',  wind_west)
weather.append('Wind North', wind_north)

# calculate and print the net wind direction
net_w = 0.
net_n = 0.
speed = weather.getFeatureData('Wind Speed')
for i in range(0, len(wind_west)):
   net_w += wind_west[i] * float(speed[i])
   net_n += wind_north[i] * float(speed[i])
print 'Net Wind West    = {0}'.format(net_w/len(wind_west))
print 'Net Wind North   = {0}'.format(net_n/len(wind_north))
phi = -(math.atan2(net_w, net_n)*180./math.pi) % 360
print 'Net Wind Bearing = {0} degrees, about WSW'.format(phi)

# days since Jan 1st (enumerating the date)
print 'Enumerating date'
day_numbers = [dt.datetime.strptime(d, '%Y-%m-%d').timetuple().tm_yday for d in weather.getFeatureData('Date')]
weather.append('Day Number', day_numbers)

# select features
allfeatures = ['Station ID','Station Name','Elevation','Latitude','Longitude','Date','Day Number','Time since midnight','Gust','Temperature','Visibility','Wind Direction','Wind Speed','Pressure','Pressure Trend','Dew Point','Humidity','Weather Type','Relative Time','Wind North','Wind West','Nearest Station Weather Type']
features = [
   #'Station ID',
   #'Station Name',
   'Elevation', 
   #'Latitude', 
   #'Longitude',
   #'Date',
   #'Day Number',
   'Time since midnight',
   'Gust',
   'Temperature', 
   'Visibility', 
   #'Wind Direction',
   'Wind Speed',
   'Pressure',
   'Pressure Trend',
   'Dew Point',
   'Humidity',
   'Weather Type',
   'Relative Time',
   'Wind North', 
   'Wind West',
   'Nearest Station Weather Type'
]

weather.select(features)
print 'Selected features:'
print weather.featureNames

print '\nDatapoint count = {0}'.format(len(weather.data))

# for adding a descriptive filename to any boxplots
features_format = ''
for f in allfeatures:
   if f in features:
      features_format += '1'
   else:
      features_format += '0'
print 'Features format = {0}'.format(features_format)

# export to file
filename = 'data/weather.p'
weather.export(filename)
print 'Data exported to %s' % filename