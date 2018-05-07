import scipy.io
import numpy as np
data = scipy.io.loadmat('Workspace_SL.mat')
# print(list(data.keys()))

for key in data.keys():
	if type(data[key]) != type("") and type(data[key]) != type([]):
		print(key + " : " + str(type(data[key])) + " : " + str(data[key].shape))
		if (key == "Sources"):
			np.savetxt('Sources.txt', data[key], delimiter=' ', fmt="%d")
		if (key == "FM_EEG_522"):
			np.savetxt('FM_EEG_1378.txt', data[key], delimiter=' ', fmt="%lf")
		if (key == "GridLoc"):
			np.savetxt('GridLoc.txt', data[key], delimiter=' ', fmt="%lf")



