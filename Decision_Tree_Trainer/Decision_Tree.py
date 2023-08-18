from sklearn import tree
import pandas as pd
import numpy as np

def read_data(name:str()) -> np.ndarray:#读取数据
    tmp_file = pd.read_csv("%s.csv"%name, header=0)
    tmp_series = pd.Series(tmp_file['blink'])
    tmp_result = tmp_series.to_numpy()
    tmp_feature = tmp_file.to_numpy()[:,1:6]
    return tmp_feature,tmp_result

X, Y = read_data("feature")
clf = tree.DecisionTreeClassifier()
clf = clf.fit(X, Y)
tree.plot_tree(clf)