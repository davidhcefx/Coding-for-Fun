# A Birthday Paradox Experiment
#
# Calculates the probability that in a room consisting of N people, at least
#  two of them have the same birthday.

def Same_Birthday(N):
    res = 1
    for i in range(N):
        res *= (365 - i) / 365
    return 1 - res


print(Same_Birthday(20))  # 41 %
print(Same_Birthday(25))  # 56 %
print(Same_Birthday(30))  # 70 %