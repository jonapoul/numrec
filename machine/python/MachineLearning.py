# no idea what most of this is for but it must do something
import sys
import time
import pickle
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from sklearn.metrics import classification_report, confusion_matrix
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.preprocessing import StandardScaler
from sklearn.datasets import make_moons, make_circles, make_classification
from sklearn.neural_network import MLPClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC
from sklearn import svm, datasets
from sklearn.gaussian_process import GaussianProcessClassifier
from sklearn.gaussian_process.kernels import RBF
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.discriminant_analysis import QuadraticDiscriminantAnalysis
from sklearn.decomposition import RandomizedPCA, PCA

weather = pickle.load(open('data/weather.p'))
# grab the classifcations array then separate it from the weather data
WT = weather.getFeatureData('Weather Type')
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
print 'Target names:       %s' % target_names
print 'Features:           %s' % features
#print weather.data
#print WT

start = time.time()
h = .02  # step size in the mesh
names = ["Nearest Neighbors", 
         "Linear SVM", 
         "RBF SVM", 
         "Gaussian Process", 
         "Decision Tree", 
         "Random Forest", 
         "Neural Net", 
         "AdaBoost", 
         "Naive Bayes", 
         "QDA"]
classifiers = [KNeighborsClassifier(3),
               SVC(kernel="linear", C=0.025),
               SVC(gamma=2, C=1),
               GaussianProcessClassifier(1.0 * RBF(1.0)),
               DecisionTreeClassifier(max_depth=5),
               RandomForestClassifier(max_depth=5, n_estimators=10, max_features=1),
               MLPClassifier(alpha=1),
               AdaBoostClassifier(),
               GaussianNB(),
               QuadraticDiscriminantAnalysis()]
# X = (x,y) coords of datapoint, y = classification/category of datapoint
X, y = make_classification(n_samples=100, 
                           n_features=2, 
                           n_redundant=0, 
                           n_informative=2, 
                           random_state=1, 
                           n_classes=2,
                           n_clusters_per_class=1)
rng = np.random.RandomState(2)
X += 2 * rng.uniform(size=X.shape)

linearly_separable = (X, y)
datasets = [make_moons(noise=0.3, random_state=0),
            make_circles(noise=0.2, factor=0.5, random_state=1),
            linearly_separable]
#datasets = [linearly_separable]
fig_size = (len(classifiers)*len(datasets), len(classifiers))
figure = plt.figure(figsize=fig_size)
plt.rcParams.update({'font.size': 8})
i = 1
# iterate over datasets
for ds_cnt, ds in enumerate(datasets):
   # preprocess dataset, split into training and test part
   X, y = ds
   X = StandardScaler().fit_transform(X)
   X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=.4, random_state=42)

   x_min, x_max = X[:, 0].min() - .5, X[:, 0].max() + .5
   y_min, y_max = X[:, 1].min() - .5, X[:, 1].max() + .5
   xx, yy = np.meshgrid(np.arange(x_min, x_max, h), np.arange(y_min, y_max, h))

   # just plot the dataset first
   cm = plt.cm.RdBu
   cm_bright = ListedColormap(['#FF0000', '#0000FF'])
   ax = plt.subplot(len(datasets), len(classifiers) + 1, i)
   if ds_cnt == 0:
      ax.set_title("Input data")
   # Plot the training points
   ax.scatter(X_train[:, 0], X_train[:, 1], c=y_train, cmap=cm_bright, edgecolors='k')
   # and testing points
   ax.scatter(X_test[:, 0], X_test[:, 1], c=y_test, cmap=cm_bright, alpha=0.6, edgecolors='k')
   ax.set_xlim(xx.min(), xx.max())
   ax.set_ylim(yy.min(), yy.max())
   ax.set_xticks(())
   ax.set_yticks(())
   i += 1

   # iterate over classifiers
   for name, clf in zip(names, classifiers):
      ax = plt.subplot(len(datasets), len(classifiers) + 1, i)
      clf.fit(X_train, y_train)
      score = clf.score(X_test, y_test)
      # Plot the decision boundary. For that, we will assign a color to each point in the mesh [x_min, x_max]x[y_min, y_max].
      if hasattr(clf, "decision_function"):
         Z = clf.decision_function(np.c_[xx.ravel(), yy.ravel()])
      else:
         Z = clf.predict_proba(np.c_[xx.ravel(), yy.ravel()])[:, 1]

      # Put the result into a color plot
      Z = Z.reshape(xx.shape)
      ax.contourf(xx, yy, Z, cmap=cm, alpha=.8)

      # Plot the training/testing points
      ax.scatter(X_train[:, 0], X_train[:, 1], c=y_train, cmap=cm_bright, edgecolors='k')
      ax.scatter(X_test[:, 0],  X_test[:, 1],  c=y_test,  cmap=cm_bright, edgecolors='k', alpha=0.6)

      ax.set_xlim(xx.min(), xx.max())
      ax.set_ylim(yy.min(), yy.max())
      ax.set_xticks(())
      ax.set_yticks(())
      if ds_cnt == 0:
         ax.set_title(name)
      ax.text(xx.max() - .3, yy.min() + .3, ('%.2f' % score).lstrip('0'), size=15, horizontalalignment='right')
      i += 1

print '{0:.2f} seconds'.format(time.time()-start)
plt.tight_layout()
plt.show()


# classifiers = [#KNeighborsClassifier(3), # t=0.05s, score=0.736
#                #SVC(kernel="linear", C=0.025), # t=78.4s, score=who cares
#                #SVC(gamma=2, C=1), # t=0.43s, score=0.775
#                #GaussianProcessClassifier(1.0 * RBF(1.0)), # t=140.02s, score=who cares
#                DecisionTreeClassifier(max_depth=5), # t=0.06s, score=0.787
#                RandomForestClassifier(max_depth=5, n_estimators=10, max_features=1), # t=0.06s, score=0.787
#                #MLPClassifier(alpha=1), # t=0.12s, score=0.675
#                #AdaBoostClassifier(), # t=0.35s, score=0.725
#                #GaussianNB(), # t=0.01s, score=0.722
#                QuadraticDiscriminantAnalysis()] # t=0.08s, score=0.731

# for i in range(len(classifiers)):
#    average = 0
#    iterations = 100
#    for j in range(iterations):
#       start = time.time()
#       #print 'splitting...'
#       testsize = 0.8
#       data_train, data_test, result_train, result_test = train_test_split(weather.data, WT, test_size=testsize)
#       expected = result_test
#       clf = classifiers[i]
#       #print 'fitting {0:.0f} datapoints...'.format((1-testsize)*N_data)
#       clf.fit(data_train, result_train)
#       #print 'predicting {0:.0f} datapoints...'.format(testsize*N_data)
#       predicted = clf.predict(data_test)
#       #print 'scoring...'
#       score = clf.score(data_test, result_test)
#       #print 'score = {0}'.format(score)
#       #print("Classification report for classifier {0}:\n{1}".format(i, classification_report(expected, predicted)))
#       #print("Confusion matrix:\n%s" % confusion_matrix(expected, predicted))
#       #print '{0:.2f} seconds'.format(time.time()-start)
#       #print '\n'
#       average += score/iterations
#    print average

# ## Step 6 - Prediction Evaluation
# # Use the `sklearn.metrics` module to compare the results using the expected and predicted datasets.
# # Examples:
# # - [Sklearn Model Evaluation](http://scikit-learn.org/stable/modules/model_evaluation.html#)
# # - [Handwritten Digits example] (http://scikit-learn.org/stable/auto_examples/classification/plot_digits_classification.html#sphx-glr-auto-examples-classification-plot-digits-classification-py)

# sys.exit()