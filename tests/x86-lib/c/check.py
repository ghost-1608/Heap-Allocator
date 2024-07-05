#!/usr/bin/python3

import sys
import os
from ast import literal_eval
from pwn import process, log, exception

# Keywords for test config header conversion to Python
keyw = ['in', 'out']

def parse(line):
    '''
    Parser to extract test config header
    '''

    # Check syntax
    if (comment := line.find('//')) == -1:
        return False
    if (openq := line.find('`', comment + 1)) == -1:
        return False
    if (closeq := line.find('`', openq + 1)) == -1:
        return False

    # Extract config
    line = line[openq + 1: closeq]

    # Preprocess
    for i in keyw:
        line = line.replace(i, f"'{i}'")

    # Convert to python dict and return
    return literal_eval(line)

# Parse commandline arguments
file = ''
enableLog = False
if '-l' in sys.argv:
    enableLog = True
for i in sys.argv[1:]:
    if not i.startswith('-'):
        file = i

# In case file not provided in commandline args
if not file:
    file = input('Enter test name: ')

# Check if test files exist
ret = 0
if not os.path.exists(f'{file}.c'):
    try:
        log.error(f'Error: File "{file}.c" not found!')
    except exception.PwnlibException:
        pass
    ret = -1
if not os.path.exists(f'{file}'):
    try:
        log.error(f'Error: Executable file "{file}" not found!')
    except exception.PwnlibException:
        pass
    ret = -2
if ret < 0:
    sys.exit(ret)

# Load source file to process
with open(file + '.c', 'r') as f:
    header = f.readline()
    header = parse(header)

# Config header error check
if not header:
    try:
        log.error(f'No test config found in "./{file}.c"!')
    except exception.PwnlibException:
        pass
    sys.exit(-3)

# Load executable
p = process(f'./{file}')

# Send input to executable
if header['in']:
    for i in list(header['in']):
        p.sendline(i.encode())

# Receive output from executable
output = p.clean().decode().split('\n')
for i in range(output.count('')):
    output.remove('')

# Process output received
out_list = header['out'] if isinstance(header['out'], list) else [header['out']]
for i in range(len(out_list)):
    if not isinstance(out_list[i], str):
        out_list[i] = str(out_list[i])

# Logging, if enabled
if enableLog:
    log.info(f'Produced output: {output}')
    log.info(f'Expected output: {out_list}')

# Publish test result
if output == out_list:
    log.success('Test PASSED!')
    sys.exit(0)
else:
    log.failure('Test FAILED!')
    sys.exit(1)
