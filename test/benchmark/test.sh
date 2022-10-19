#!/bin/bash

base_folder="../resources"


current_folder=${2:-./}

exec 3>${current_folder}/compared.dat
exec 2>&3
exec 1>&3

for file in ${current_folder}/${base_folder}/test*.dat; do

    echo -e "Testing ${file} ..."

    # Check if an argument to executable location has been passed to the program
    if [ -z "$1" ]; then
        bin/benchmark < $file
    else
        $1 < $file
    fi
done

exit 0