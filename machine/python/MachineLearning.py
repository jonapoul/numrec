## Step 1 - Import and Data loading
import pickle
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from sklearn.metrics import classification_report, confusion_matrix
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.datasets import make_moons, make_circles, make_classification
from sklearn.neural_network import MLPClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC
from sklearn.gaussian_process import GaussianProcessClassifier
from sklearn.gaussian_process.kernels import RBF
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.discriminant_analysis import QuadraticDiscriminantAnalysis

# Load the weather data you created by FeatureExtraction.py
print 'Loading pickle file...'
weather = pickle.load(open('data/mldata.p'))
print 'Done!'

# Confirm that the data has loaded correctly by inspecting the data attributes in the `weather` object.
print 'Number of entries: \n\t%s\n' % (weather.getNrEntries())
print 'Number of targets: \n\t%s\n' % (weather.getNrTargets())
print 'Target names: \n\t%s\n' % (weather.getTargetNames())
print 'Number of features: \n\t%s\n' % (weather.getNrFeatures())
print 'Features: \n\t%s\n' % (weather.getFeatures())
print 'Number of weather stations: \n\t%s\n' % (weather.getNrStations())
print 'Stations (ID, Name, Latitude, Longitude)'
stations = weather.getStationData('all')
print '\t%s' % stations[0]
print '\t...'
print '\t%s' % stations[len(stations)-1]
print 'Temperature data: \n\t%s\n' % (weather.getFeatureData('Temperature'))

N = 100
x = np.arange(0, N, 1)
y = [np.random.random() for _ in xrange(N)]
#y = np.random.randint(0, N)
plt.plot(x, y, 'k-')
plt.show()

## Step 2 - Define the training and testing sample
#
# Divide the weather data into a suitable training and testing sample.
# Start with a 50/50 split but make this easily adaptable for futher fitting evaluation.
#
# *Examples*:
# [`sklearn.model_selection.train_test_split`]
# http://scikit-learn.org/stable/modules/generated/sklearn.model_selection.train_test_split.html

# DEFINE TRAINING AND TESTING SAMPLES AND TARGETS HERE

## Step 3 - Define the classification method

# This can be any of the estimators provided by Sklearn.
# I suggest you start with a *white box* method
# to better understand the process before trying something more advanced.

# DEFINE CLASSIFIER HERE
# e.g for a Decision tree: clf = DecisionTreeClassifier()

## Step 4 - Fit the training data

# Run the `fit` method of your chosen estimator using the training data (and corresponding targets) as input

# RUN FIT METHOD HERE

## Step 5 - Define the expected and predicted datasets

# Define `expected` as your *test* target values (i.e. **not** your *training* target values)
# and run the `predict` method on your chosen estimator using your *test data*

# DEFINE EXPECTED AND PREDICTED VALUES HERE

## Step 6 - Prediction Evaluation

# Use the `sklearn.metrics` module to compare the results using the expected and predicted datasets.

# Examples:
# - [Sklearn Model Evaluation](http://scikit-learn.org/stable/modules/model_evaluation.html#)
# - [Handwritten Digits example](http://scikit-learn.org/stable/auto_examples/classification/plot_digits_classification.html#sphx-glr-auto-examples-classification-plot-digits-classification-py)

# RUN PREDICTION EVALUATION METHODS HERE
