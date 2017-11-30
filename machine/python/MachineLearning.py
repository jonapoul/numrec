# no idea what most of this is for but it must do something
import sys
import time
import pickle
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from mpl_toolkits.mplot3d import Axes3D
from sklearn.metrics import classification_report, confusion_matrix, accuracy_score
from sklearn.model_selection import train_test_split, cross_val_score, GridSearchCV
from sklearn.preprocessing import StandardScaler
from sklearn.datasets import make_moons, make_circles, make_classification
from sklearn.neural_network import MLPClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC, LinearSVC
from sklearn import svm, datasets, tree
from sklearn.multiclass import OneVsRestClassifier
from sklearn.gaussian_process import GaussianProcessClassifier
from sklearn.gaussian_process.kernels import RBF
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.discriminant_analysis import QuadraticDiscriminantAnalysis
from sklearn.decomposition import RandomizedPCA, PCA
from sklearn.linear_model import RidgeClassifier

start = time.time()
np.set_printoptions(threshold=np.nan)
weather = pickle.load(open('data/weather.p'))

# grab the classifcations array then separate it from the weather data
classifications = weather.getFeatureData('Weather Type')
weather.delete('Weather Type')
N_data       = weather.getNrEntries()
N_targets    = weather.getNrTargets()
N_features   = weather.getNrFeatures()
N_stations   = weather.getNrStations()
target_names = weather.getTargetNames()
targets      = np.arange(0, N_targets, 1)
features     = weather.getFeatures()
print 'Number of entries:  %s' % N_data
print 'Number of targets:  %s' % N_targets
print 'Number of stations: %s' % N_stations
print 'Number of features: %s' % N_features
print 'Target names and frequencies:'
index = len(weather.data[0])-1
count = np.zeros(N_targets)
for c in classifications:
   count[int(c)] += 1
print 'Weather Types and frequencies:'
for i in range(len(count)):
   print '{0:2d} = {1:13s} = {2:5.0f}'.format(i, target_names[i], count[i])

print 'Features:         \n%s' % features

allfeatures = [ # just for reference
   'Station ID',
   'Station Name',
   'Elevation',
   'Latitude',
   'Longitude',
   'Date',
   'Day Number',
   'Time since midnight',
   'Gust',
   'Temperature',
   'Visibility',
   'Wind Direction',
   'Wind Speed',
   'Pressure',
   'Pressure Trend',
   'Dew Point',
   'Humidity',
   'Weather Type',
   'Wind North',
   'Wind West',
   'Nearest Station Weather Type'
]
names = [
   'Nearest\nNeighbours',
   'Decision\nTree',
   'Random\nForest',
   'AdaBoost\nRFC',
   'Neural\nNet',
   'Ridge\nClassifier',
]
classifiers = [
   KNeighborsClassifier(n_jobs=-1, n_neighbors=1, p=1),
   DecisionTreeClassifier(max_depth=10),
   RandomForestClassifier(n_estimators=50, n_jobs=-1),
   AdaBoostClassifier(base_estimator=RandomForestClassifier(n_estimators=50, n_jobs=-1), n_estimators=1),
   MLPClassifier(activation='logistic', learning_rate='invscaling'),
   RidgeClassifier(fit_intercept=True),
]

print 'Chosen Classifiers:'
print names

nn_all = []
nn_02  = []
N = 20
for j in range(N):
   iter_start = time.time()
   print 'iteration {0:2d}/{1}:       '.format(j, N),
   for i in range(len(classifiers)):
      if j == 0:
         nn_all.append([])
         nn_02.append([])

      t = time.time()
      data_train,data_test,class_train,class_test = train_test_split(weather.data, classifications, test_size=0.5)
      actual = class_test
      clf = classifiers[i]
      clf.fit(data_train, class_train)
      # if names[i] == "Decision Tree":
      #    tree.export_graphviz(clf, out_file='tree.dot')  # dot -Tpng tree.dot -o tree.png

      predicted = clf.predict(data_test)
      data_test  = data_test[class_test != 1]  # remove all test elements for class=1
      class_test = class_test[class_test != 1]
      score = clf.score(data_test, class_test)
      score_all = accuracy_score(actual,     predicted)
      score_02  = accuracy_score(class_test, clf.predict(data_test))
      nn_all[i].append(score_all)
      nn_02[i].append(score_02)
      print '{0}={1:.2f}s '.format(i, time.time()-t),
   print '       total={0:.3f} secs'.format(time.time()-iter_start)

print '\nall:'
print 'mean   = {0}'.format(np.mean(nn_all))
print 'var    = {0}'.format(np.var(nn_all))
print 'min    = {0}'.format(np.min(nn_all))
print 'max    = {0}'.format(np.max(nn_all))
print 'median = {0}'.format(np.median(nn_all))

print '\n0/2:'
print 'mean   = {0}'.format(np.mean(nn_02))
print 'var    = {0}'.format(np.var(nn_02))
print 'min    = {0}'.format(np.min(nn_02))
print 'max    = {0}'.format(np.max(nn_02))
print 'median = {0}'.format(np.median(nn_02))

print '\ntime = {0}'.format(time.time()-start)

plt.figure()
plt.boxplot(nn_all, 0, '')
plt.title("Accuracy score over all weather types")
plt.ylim(0, 1)
plt.xticks([i for i in range(len(names)+1)], [" "] + names, rotation=25)
plt.show()

plt.figure()
plt.boxplot(nn_02, 0, '')
plt.title("Accuracy score for weather types 0 and 2")
plt.ylim(0, 1)
plt.xticks([i for i in range(len(names)+1)], [" "] + names, rotation=25)
plt.show()
