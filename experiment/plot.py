import matplotlib.pyplot as plt
import numpy as np
import sys
import math

def addlabels(x,y):
    for i in range(len(x)):
        plt.text(i, round(y[i], 2), round(y[i],2), ha = 'center')

def experiment():
    binarySearch_baseline = np.array([184027.330000, 102906.642000, 169225.910000, 84219.442000])
    kcaf_baseline = np.array([3829.906000, 4287.390000, 3880.161000, 3968.756000, 3779.139000, 3946.843000])
    checksum_baseline = np.array([48685.141000, 47635.832000, 55428.951000])
    hash_baseline = np.array([4420.082000, 5500.902000, 8313.516000, 3742.116000, 5562.839000, 7061.582000, 6221.687000, 5478.900000])
    binarySeach = np.array([29430.589000, 28885.665000, 29383.287000, 30387.469000])
    kcaf = np.array([1018.316000, 1042.276000, 1036.025000, 963.081000, 964.831000, 975.554000])
    checksum = np.array([40769.016000, 36962.737000, 35460.751000])
    hash = np.array([3163.440000, 3113.150000, 3213.164000, 3269.456000, 3514.593000, 3265.822000, 3412.536000, 3213.592000])

    binarySearch_baseline_mean = np.mean(binarySearch_baseline)
    kcaf_baseline_mean = np.mean(kcaf_baseline)
    checksum_baseline_mean = np.mean(checksum_baseline)
    hash_baseline_mean = np.mean(hash_baseline)
    binarySeach_mean = np.mean(binarySeach)
    kcaf_mean = np.mean(kcaf)
    checksum_mean = np.mean(checksum)
    hash_mean = np.mean(hash)

    X = ["binary Search", "keccakf", "checksum", "hashtable"]
    y_base = [1, 1, 1]
    y_new = [binarySearch_baseline_mean/binarySeach_mean, kcaf_baseline_mean/kcaf_mean, checksum_baseline_mean/checksum_mean, hash_baseline_mean/hash_mean]
    X_axis = np.arange(len(X)) 

    plt.bar(X_axis - 0.2, y_base, 0.4, label = 'without preemption') 
    plt.bar(X_axis + 0.2, y_new, 0.4, label = 'preemption') 

    plt.xticks(X_axis, X) 
    plt.xlabel("usecase") 
    plt.ylabel("speedup (%)") 
    plt.title("Preemption Speedup Compared to Baseline") 
    plt.legend() 
    plt.show() 

    return
    
def sensitivity_execution():
    X = ['2%', '10%', '20%']
    checksum = [48685.141000/40769.016000, 50174.600000/36908.129000, 54242.783000/40565.371000]
    kcaf = [3829.906000/1018.316000, 3968.756000/963.081000, 3946.843000/975.554000]
    bs = [113981.222000/32037.580000, 124072.810000/29784.955000, 97726.694000/29234.542000]
    hash = [5562.839000/3269.456000, 4350.885000/3262.554000, 5374.452000/3263.666000]

    X_axis = np.arange(len(X)) 
    plt.bar(X_axis - 0.1, checksum, 0.1, label = 'checksum') 
    plt.bar(X_axis, kcaf, 0.1, label = 'keccakf')
    plt.bar(X_axis + 0.1, bs, 0.1, label = 'binary search')
    plt.bar(X_axis + 0.2, hash, 0.1, label= 'hash table')

    plt.xticks(X_axis, X) 
    plt.xlabel("execution time window") 
    plt.ylabel("speedup (%)") 
    plt.title("Preemption Speedup with Different Execution Time Window") 
    plt.legend() 
    plt.show() 

    return

def sensitivity_threshold():
    X = ['60%', '40%', '20%']
    checksum = [48685.141000/40769.016000, 50643.311000/41765.777000, 54242.783000/40565.371000]
    kcaf = [3829.906000/1018.316000, 4287.390000/1042.276000, 3880.161000/1036.025000]
    bs = [113981.222000/32037.580000, 87768.426000/30333.815000, 93428.493000/29634.704000]
    hash = [5562.839000/3269.456000, 6431.236000/3512.193000, 6401.939000/3465.461000]

    X_axis = np.arange(len(X)) 
    plt.bar(X_axis - 0.1, checksum, 0.1, label = 'checksum') 
    plt.bar(X_axis, kcaf, 0.1, label = 'keccakf')
    plt.bar(X_axis + 0.1, bs, 0.1, label = 'binary search')
    plt.bar(X_axis + 0.2, hash, 0.1, label= 'hash table')

    plt.xticks(X_axis, X) 
    
    # addlabels(X, checksum)
    # addlabels(X, kcaf)
    # addlabels(X, bs)

    plt.xlabel("abortion percentage") 
    plt.ylabel("speedup (%)") 
    plt.title("Preemption Speedup with Different Abortion Percentage") 
    plt.legend() 
    plt.show() 

    return

if __name__ == "__main__":
    # experiment()
    sensitivity_execution()
    #sensitivity_threshold()