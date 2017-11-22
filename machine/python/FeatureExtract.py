from Weather import *

# load class
weatherFile = '../data/basic.txt'
fileSlice = 0
weather = Weather(weatherFile, fileSlice)

# zero any null gust measurements
newG = ['0' if g == '-99999' else g for g in weather.getFeatureData('Gust')]
weather.modify('Gust', newG)

# After recovering any data ensure you run the `discard()` method to remove any data with remaining null observations.
weather.discard()

# enumerate pressure trends
# F = Forward = +1, S = Steady = 0, R = Reversing(?) = -1
pTType = ['F', 'S', 'R']
new_PT = [str(pTType.index(p) - 1) for p in weather.getFeatureData('Pressure Trend') ]
weather.modify('Pressure Trend', new_PT)

# enumerate wind direction
compassRose = ['NNE', 'NE', 'ENE', 'E', 'ESE', 'SE', 'SSE', 'S', 'SSW', 'SW', 'WSW', 'W', 'WNW', 'NW', 'NNW', 'N']
new_WD = [compassRose.index(w) for w in weather.getFeatureData('Wind Direction')]
weather.modify('Wind Direction', new_WD)

# set relative direction (assume 16 points)
points = 16
north = [abs((points/2) - int(w))%(points / 2) for w in weather.getFeatureData('Wind Direction')]
west = [abs((points/2) - int(w) - (points / 4))%(points / 2) for w in weather.getFeatureData('Wind Direction')]
weather.append('Wind Relative North', north)
weather.append('Wind Relative West', west)

# select features
weather.delete('Station Name')
weather.delete('Station ID')
weather.delete('Latitude')
weather.delete('Longitude')
weather.delete('Date')
weather.delete('Wind Direction')

allfeats = ['Station ID', 'Station Name', 'Elevation', 'Latitude', 'Longitude', 'Date', 'Time since midnight', 'Gust', 'Temperature', 'Visibility', 'Wind Direction', 'Wind Speed', 'Pressure', 'Pressure Trend', 'Dew Point', 'Humidity', 'Weather Type']

features = ['Elevation', 'Temperature', 'Visibility', 'Pressure', 'Pressure Trend', 'Humidity']
weather.select(features)
print 'Selected features:'
print weather.featureNames

# export to file
filename = 'data/weather.p'
weather.export(filename)
print '\nData exported to %s' % filename
