import math
from decimal import *
getcontext().prec = 20

b = float(9992) #blue balls aka normal users
r = float(8) #red balls aka SGX users
t = float(10000) #total balls aka total users
n = 5000 #num choices aka number of nodes that don't fail
p = 1 #min number of red balls you want aka SGX users

val = [] #total prob

def choose(n, k):
    """
    A fast way to calculate binomial coefficients by Andrew Dalke (contrib).
    """
    if 0 <= k <= n:
        ntok = 1
        ktok = 1
        for t in range(1, min(k, n - k) + 1):
            ntok *= n
            ktok *= t
            n -= 1
        return ntok // ktok
    else:
        return 0

for i in range(n + 1):
    tmp1 = Decimal(choose(n, i))
    tmp2 = float(pow((b/t), (n-i)) * pow((r/t), i))
    val.append(tmp1*Decimal(tmp2))
    #print(i)
    #val += (math.factorial(n-k))/((math.factorial(i)) * (math.factorial(n-k-i))) * pow(s/n, n-k-i) * pow((n-s)/n, i)

sum = 0

for j in val[p:]:

    sum+=j

print(sum)

