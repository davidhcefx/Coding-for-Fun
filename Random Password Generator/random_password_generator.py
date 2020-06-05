from os import urandom
from random import shuffle

# Written by davidhcefx, 2020.3.16.

print('===============================================================================')
print('||                     Offline Random Password Generator                     ||')
print('===============================================================================')
print('')
ln = input('> Password Length: (strong: >= 16) ').strip()
length = 16 if ln == '' else int(ln)
print('length = %d\n' % length)

print('Choose Character Set: [default = 12345]')
print('[1] Lowercase + Uppercase')
print('[2] Digits')
print('[3] Symbols')
print('[4] EXCLUDE Similar Characters: I l 1 O 0')
print('[5] EXCLUDE Ambiguous Characters: {}[]()<>/\\\'\"`;:,.')
choice = input('> ').strip()
if choice == '':
    choice = '12345'

charset = []
exclude_alnum = ''
exclude_symbol = ''

# Exclude similar
if '4' in choice:
    exclude_alnum = 'Il1O0'

# Exclude ambiguous
if '5' in choice:
    exclude_symbol = '{}[]()<>/\\\'\"`;:,.'

# Lower + Upper
if '1' in choice:
    alpha = [chr(i) for i in range(ord('a'), ord('z') + 1)] \
        + [chr(i) for i in range(ord('A'), ord('Z') + 1)]
    for a in alpha:
        if a not in exclude_alnum:
            charset.append(a)

# Digits
if '2' in choice:
    for i in range(ord('0'), ord('9') + 1):
        if chr(i) not in exclude_alnum:
            charset.append(chr(i))

# Symbols
if '3' in choice:
    all_symbols = [chr(i) for i in range(0x21, 0x2f + 1)] \
        + [chr(i) for i in range(0x3a, 0x40 + 1)] \
        + [chr(i) for i in range(0x5b, 0x60 + 1)] \
        + [chr(i) for i in range(0x7b, 0x7e + 1)]
    for s in all_symbols:
        if s not in exclude_symbol:
            charset.append(s)

print('charset:', ''.join(charset))
shuffle(charset)  # mitigate the probability bias in [0,255] % len(charset)

passwd = []
for i in urandom(length):
    passwd.append(charset[i % len(charset)])

print('\n> Here is your generated password: ', ''.join(passwd))
