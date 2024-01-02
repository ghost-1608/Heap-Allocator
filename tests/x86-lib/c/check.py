#!/usr/bin/python3

import sys
import os
from ast import literal_eval
from pwn import process, log

# sys.tracebacklimit = 0

keyw = ['in', 'out']

def parse(line):
    if comment := line.find('//') == -1:
        return False
    if openq := line.find('`', comment + 1) == -1:
        return False
    if closeq := line.find('`', openq + 1) == -1:
        return False

    line = line.replace('//', '').replace('`', '')

    for i in keyw:
        line = line.replace(i, f"'{i}'")

    return literal_eval(line)

if len(sys.argv) == 1:
    file = input('Enter file: ')
else:
    file = sys.argv[1]

if not os.path.exists(f'{file}.c'):
    log.error(f'Error: File "{file}.c" not found!')
    sys.exit(-1)

if not os.path.exists(f'{file}'):
    log.error(f'Error: Executable file "{file}" not found!')
    sys.exit(-2)
    
with open(file + '.c', 'r') as f:
    header = f.readline()
    header = parse(header)
    
p = process(f'./{file}')
    
if header['in']:
    for i in list(header['in']):
        p.sendline(i.encode())

output = p.clean().decode().split('\n')
for i in range(output.count('')):
    output.remove('')

out_list = header['out'] if isinstance(header['out'], list) else [header['out']]
for i in range(len(out_list)):
    if not isinstance(out_list[i], str):
        out_list[i] = str(out_list[i])
        
if output == out_list:
    log.success('Test PASSED!')
    sys.exit(0)
else:
    log.failure('Test FAILED!')
    sys.exit(1)
