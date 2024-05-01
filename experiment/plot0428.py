import matplotlib.pyplot as plt
import numpy as np
import sys
import math

# Here is the data
binarySearch_baseline = np.array([158915.455000,156546.427000,115484.951000,159051.776000,106331.825000,121030.281000,115669.082000,97066.966000,104370.276000])
kcaf_baseline = np.array([2846.391000,2712.009000,2996.755000,2786.018000,2846.206000,2768.569000,2847.253000,2922.504000,2944.499000])
checksum_baseline = np.array([59531.607000,48685.141000,40160.207000,50643.311000,54242.783000,50174.600000,65021.146000,61281.297000,41283.253000])
hash_baseline = np.array([5323.545000,5035.379000,7247.668000,3742.924000,7231.290000,4746.918000,6720.845000,5292.228000,4378.920000])

binarySeach = np.array([29449.927000,29453.508000,29451.540000,27255.954000,26352.149000,29505.293000,29504.427000,29506.573000,29549.115000])
kcaf = np.array([792.581000,826.702000,794.063000,890.674000,828.557000,844.108000,802.543000,846.548000,826.991000])
checksum = np.array([35210.680000,40769.016000,36362.865000,41765.777000,40565.371000,36908.129000,37017.304000,35557.845000,35607.982000])
hash = np.array([3112.307000,3113.594000,3113.670000,3111.341000,3261.581000,3112.713000,3111.765000,3211.723000,3062.050000])

binarySearch_advance = np.array([29448.902000,29220.395000,27155.683000,39813.511000,27559.006000,39847.008000,29996.958000,26805.870000,29451.036000])
checksum_advance = np.array([38768.020000,35315.748000,35158.487000,38616.402000,35115.396000,39767.957000,35714.464000,35665.124000,35717.218000])
hash_advance = np.array([3162.684000,3111.769000,3113.343000,3112.236000,4999.813000,3636.958000,3545.079000,3111.901000,3111.590000])
kcaf_advance = np.array([766.284000,877.768000,833.904000,860.657000,842.876000,942.063000,824.208000,992.310000,802.039000])

bs_dpu_origin = 3075.657000
checksum_dpu_origin = 1250.632000
keccak_dpu_origin = 820.762000
hashtable_dpu_origin = 286.846000

binarySearch_baseline_dpu_mean = np.mean(binarySearch_baseline)
kcaf_baseline_dpu_mean = np.mean(kcaf_baseline)
checksum_baseline_dpu_mean = np.mean(checksum_baseline)
hash_baseline_dpu_mean = np.mean(hash_baseline)

binarySearch_baseline_mean = 64.382000
kcaf_baseline_mean = 5.499000
checksum_baseline_mean = 246.612000
hash_baseline_mean = 8.968000

checksum_cc = np.mean([5.87222e+08,5.94033e+08,5.977e+08])
bs_cc = np.mean([1.49567e+08,1.42316e+08,1.46358e+08])
kecaf_cc = np.mean([9.45273e+06,9.46654e+06,9.52913e+06])
hash_cc = np.mean([1.59835e+07,2.00412e+07,7.28845e+06])
checksum_resume_cc = np.mean([2.32842e+07,1.92875e+07,2.23157e+07])
bs_resume_cc = np.mean([5.98143e+07,6.75572e+07,7.41064e+07])
kecaf_resume_cc = np.mean([2.46306e+06,2.6638e+06,2.55335e+06])
hash_resume_cc = np.mean([4.68551e-310,4.67378e-310,4.67173e-310])

def addlabels(x,y):
    for i in range(len(x)):
        plt.text(i, round(y[i], 2), round(y[i],2), ha = 'center')

def experiment():

    binarySearch_baseline_mean = np.mean(binarySearch_baseline)
    kcaf_baseline_mean = np.mean(kcaf_baseline)
    checksum_baseline_mean = np.mean(checksum_baseline)
    hash_baseline_mean = np.mean(hash_baseline)
    binarySeach_mean = np.mean(binarySeach)
    kcaf_mean = np.mean(kcaf)
    checksum_mean = np.mean(checksum)
    hash_mean = np.mean(hash)
    binarySearch_advance_mean = np.mean(binarySearch_advance)
    kcaf_advance_mean = np.mean(kcaf_advance)
    checksum_advance_mean = np.mean(checksum_advance)
    hash_advance_mean = np.mean(hash_advance)

    X = ["binary Search", "keccakf", "checksum", "hashtable"]
    y_base = [1, 1, 1, 1]
    y_new = [binarySearch_baseline_mean/binarySeach_mean, kcaf_baseline_mean/kcaf_mean, checksum_baseline_mean/checksum_mean, hash_baseline_mean/hash_mean]
    y_advance = [binarySearch_baseline_mean/binarySearch_advance_mean, kcaf_baseline_mean/kcaf_advance_mean, checksum_baseline_mean/checksum_advance_mean, hash_baseline_mean/hash_advance_mean]
    print(y_new)
    X_axis = np.arange(len(X)) 

    plt.bar(X_axis - 0.2, y_base, 0.2, label = 'without preemption') 
    plt.bar(X_axis, y_new, 0.2, label = 'preemption')
    plt.bar(X_axis + 0.2, y_advance, 0.2, label = 'advance mode') 

    plt.xticks(X_axis, X) 
    plt.xlabel("usecase") 
    plt.ylabel("speedup (%)") 
    plt.title("Preemption Speedup Compared to Baseline") 
    plt.legend() 
    plt.show() 

    return

def CPU_baseline():

    binarySeach_mean = np.mean(binarySeach)
    kcaf_mean = np.mean(kcaf)
    checksum_mean = np.mean(checksum)
    hash_mean = np.mean(hash)
    
    print(checksum_mean)

    X = ["binary Search", "keccakf", "checksum", "hashtable"]
    y_base = [1, 1, 1, 1]
    y_new = [binarySeach_mean / binarySearch_baseline_mean , kcaf_mean/kcaf_baseline_mean, checksum_mean/checksum_baseline_mean, hash_mean/hash_baseline_mean]
    
    print(y_new)
    X_axis = np.arange(len(X)) 

    plt.bar(X_axis - 0.2, y_new, 0.4, label = 'CPU baseline', color = 'blue') 
    plt.bar(X_axis + 0.2, y_base, 0.4, label = 'lazy preemption', color = 'green') 

    plt.xticks(X_axis, X) 
    plt.xlabel("usecase") 
    plt.ylabel("speedup (%)") 
    plt.title("CPU Speedup Compared to preemption") 
    plt.legend() 
    plt.show() 

    return

def CPU_scaled_baseline():

    binarySeach_mean = np.mean(binarySeach)
    kcaf_mean = np.mean(kcaf)
    checksum_mean = np.mean(checksum)
    hash_mean = np.mean(hash)

    X = ["binary Search", "keccakf", "checksum", "hashtable"]
    y_base = [1, 1, 1, 1]
    y_new = [(binarySeach_mean * bs_dpu_origin)/(binarySearch_baseline_mean * binarySearch_baseline_dpu_mean),
             (kcaf_mean * keccak_dpu_origin)/(kcaf_baseline_mean * kcaf_baseline_dpu_mean), 
             (checksum_mean * checksum_dpu_origin)/(checksum_baseline_mean * checksum_baseline_dpu_mean),
             (hash_mean * hashtable_dpu_origin)/(hash_baseline_mean * hash_baseline_dpu_mean)]
    print(y_new)
    X_axis = np.arange(len(X)) 

    plt.bar(X_axis - 0.2, y_new, 0.4, label = 'scaled lazy preemption', color = 'green') 
    plt.bar(X_axis + 0.2, y_base, 0.4, label = 'CPU baseline', color = 'blue') 

    plt.xticks(X_axis, X) 
    plt.xlabel("usecase") 
    plt.ylabel("speedup (%)") 
    plt.title("Scaled CPU Speedup Compared to preemption") 
    plt.legend() 
    plt.show() 

    return

def CPU_dpu_compare():

    binarySeach_mean = np.mean(binarySeach)
    kcaf_mean = np.mean(kcaf)
    checksum_mean = np.mean(checksum)
    hash_mean = np.mean(hash)
    
    print(checksum_mean)

    X = ["binary Search", "keccakf", "checksum", "hashtable"]
    y_base = [1, 1, 1, 1]
    y_new = [bs_dpu_origin / binarySearch_baseline_mean, keccak_dpu_origin / kcaf_baseline_mean, checksum_dpu_origin / checksum_baseline_mean, hashtable_dpu_origin / hash_baseline_mean]
    
    print(y_new)
    X_axis = np.arange(len(X)) 

    plt.bar(X_axis - 0.2, y_new, 0.4, label = 'CPU baseline', color = 'blue') 
    plt.bar(X_axis + 0.2, y_base, 0.4, label = 'PIM baseline', color = 'green') 

    plt.xticks(X_axis, X) 
    plt.xlabel("usecase") 
    plt.ylabel("speedup (%)") 
    plt.title("CPU Speedup Compared to PIM baseline") 
    plt.legend() 
    plt.show() 

    return    
    
def CPU_resume_cc_compare():
    categories = ['checksum', 'bs', 'keccaf', 'hash']
    cc_values = [checksum_cc, bs_cc, kecaf_cc, hash_cc]
    resume_cc_values = [checksum_resume_cc, bs_resume_cc, kecaf_resume_cc, hash_resume_cc]

    normalized_cc = [cc / (cc + resume_cc) for cc, resume_cc in zip(cc_values, resume_cc_values)]
    normalized_resume_cc = [resume_cc / (cc + resume_cc) for cc, resume_cc in zip(cc_values, resume_cc_values)]

    x = np.arange(len(categories))
    width = 0.35

    fig, ax = plt.subplots()
    rects1 = ax.bar(x - width/2, normalized_cc, width, label='CPU only clock cycle')
    rects2 = ax.bar(x + width/2, normalized_resume_cc, width, label='Preempt CPU resume function clock cycle')

    ax.set_ylabel('Normalized Clock Cycle')
    ax.set_title('Comparison of CPU only cc and preempt resume function cc')
    ax.set_xticks(x)
    ax.set_xticklabels(categories)
    ax.legend()

    plt.show()

def sensitivity_execution():
    X = ['1%','2%','5%', '10%', '20%']
    indices = [6, 1, 7, 5, 8]
    checksum_window = np.array([checksum_baseline[index] / checksum[index] for index in indices])
    kcaf_window = np.array([kcaf_baseline[index] / kcaf[index] for index in indices])
    bs_window = np.array([binarySearch_baseline[index] / binarySeach[index] for index in indices])
    hash_window = np.array([hash_baseline[index] / hash[index] for index in indices])

    X_axis = np.arange(len(X)) 
    plt.bar(X_axis - 0.1, checksum_window, 0.1, label = 'checksum') 
    plt.bar(X_axis, kcaf_window, 0.1, label = 'keccakf')
    plt.bar(X_axis + 0.1, bs_window, 0.1, label = 'binary search')
    plt.bar(X_axis + 0.2, hash_window, 0.1, label= 'hash table')

    plt.xticks(X_axis, X) 
    plt.xlabel("execution time window") 
    plt.ylabel("speedup (%)") 
    plt.title("Preemption Speedup with Different Execution Time Window") 
    plt.legend() 
    plt.show() 

    return

def sensitivity_threshold():
    X = ['80%','60%','50%', '40%', '20%']
    indices = [0, 1, 2, 3, 4]
    checksum_threshold = np.array([checksum_baseline[index] / checksum[index] for index in indices])
    kcaf_threshold = np.array([kcaf_baseline[index] / kcaf[index] for index in indices])
    bs_threshold = np.array([binarySearch_baseline[index] / binarySeach[index] for index in indices])
    hash_threshold = np.array([hash_baseline[index] / hash[index] for index in indices])
    X_axis = np.arange(len(X)) 
    plt.bar(X_axis - 0.1, checksum_threshold, 0.1, label = 'checksum') 
    plt.bar(X_axis, kcaf_threshold, 0.1, label = 'keccakf')
    plt.bar(X_axis + 0.1, bs_threshold, 0.1, label = 'binary search')
    plt.bar(X_axis + 0.2, hash_threshold, 0.1, label= 'hash table')

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
    #experiment()
    #sensitivity_execution()
    #sensitivity_threshold()
    #CPU_baseline()
    #CPU_scaled_baseline()
    #CPU_dpu_compare()
    CPU_resume_cc_compare()