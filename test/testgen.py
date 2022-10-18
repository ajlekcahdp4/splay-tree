#!/usr/bin/python
##
# ----------------------------------------------------------------------------
# "THE BEER-WARE LICENSE" (Revision 42):
# <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
# can do whatever you want with this stuff. If we meet some day, and you think
# this stuff is worth it, you can buy me a beer in return.       Alex Romanov
# ----------------------------------------------------------------------------
##

import argparse
from cgi import test
import os
import json
import numpy as np
import random

usage_string = "gentest.py -n <num> -o output"

def number_less_then (elements: list, num: int):
    cnt = 0
    for i in elements:
        if (i < num):
            cnt += 1
        else:
            break
    return cnt

def save_test (path: str, test: str, ans: str, idx: int, test_fmt: str = 'test{}.dat', ans_fmt: str = 'test{}.dat.ans') -> None:
    with open (os.path.join(path, test_fmt.format (idx)), 'w') as test_file:
        test_file.write(test)
    with open (os.path.join(path, ans_fmt.format (idx)), 'w') as test_file:
        test_file.write(ans)
    

def generate_random_test (config: dict):
    length = np.random.default_rng().integers (
        config['elements']['min'], config['elements']['max']
    )

    elements = random.sample(range(config["integers"]["min"], config["integers"]["max"]), length)

    length = len(elements)

    test_str = ''

    for i in elements:
        test_str += ' k {}'.format(i)
    
    elements.sort()

    queries_kth_cnt = np.random.default_rng().integers (
        config['kth_queries']['min'], config['kth_queries']['max']
    )

    ans_str = ''
    for i in range (queries_kth_cnt):
        index = np.random.default_rng().integers(1, length)
        ans_str += '{} '.format(elements[index - 1])
        test_str += 'm {} '.format(index)
    
    queries_less_cnt = np.random.default_rng().integers (
        config['less_queries']['min'], config['less_queries']['max']
    )

    for i in range (queries_kth_cnt):
        key = np.random.default_rng().integers(elements[0], elements[-1])
        ans_str += '{} '.format(number_less_then (elements, key))
        test_str += 'n {} '.format(key)

    return [test_str, ans_str]



def generate_tests (config: dict):
    # Create output directory if it does not exists
    os.makedirs(config['output_path'], exist_ok=True)
    number = config['number']

    for i in range (number):
        test = generate_random_test (config)
        save_test (config['output_path'], test[0], test[1], i)
    

def main ():
    parser = argparse.ArgumentParser(description='Generate queries end-to-end tests.')
    parser.add_argument('--config_path', dest='path', type=str, help='Path to config file.')
    args = parser.parse_args()

    config_path = args.path
    if (not os.path.isfile(config_path)):
        raise Exception('Wrong config file path.')

    with open(config_path) as config_file:
        config_settings = json.load(config_file)
    
    generate_tests (config_settings)

if __name__ == '__main__':
    main()