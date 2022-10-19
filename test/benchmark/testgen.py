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

def save_test (path: str, test: str, idx: int, test_fmt: str = 'test{}.dat') -> None:
    with open (os.path.join(path, test_fmt.format (idx)), 'w') as test_file:
        test_file.write(test)
    

def generate_random_test (config: dict):
    inserts_n = np.random.default_rng().integers (
        config['inserts']['min'], config['inserts']['max']
    )

    elements = random.sample(range(config["integers"]["min"], config["integers"]["max"]), inserts_n)
    inserts_n = len(elements)
    test_str = '{} '.format(inserts_n)

    for elem in elements:
        test_str += '{} '.format(elem)

    requests_n = np.random.default_rng().integers (
        config['requests']['min'], config['requests']['max']
    )
    test_str += '{} '.format(requests_n)

    elements.sort()

    for i in range (requests_n):
        l_bound = np.random.default_rng().integers(elements[0], elements[-1])
        r_bound = np.random.default_rng().integers(l_bound, elements[-1])
        test_str += '{} {} '.format(l_bound, r_bound)

    return test_str

def generate_tests (config: dict):
    # Create output directory if it does not exists
    os.makedirs(config['output_path'], exist_ok=True)
    number = config['number']

    for i in range (number):
        test = generate_random_test (config)
        save_test (config['output_path'], test, i)
    

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