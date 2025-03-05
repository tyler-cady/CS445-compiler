#!/bin/bash

# run all the tests in the tests directory

# run all the tests in the tests directory in the background

# t1-1a.p: (* ERROR: variable redeclared *)
# t1-1b.p: (* ERROR: variable redeclared *)
# t1-2.p: (* LEGAL: local names should hide non-local names *)
# t1-3.p: (* LEGAL: non-local names visible from inner scopes *)
# t1-4.p: (* LEGAL: scope of the name of subprograms *)
# t1-5.p: (* ERROR: local objects not defined outside their scopes *)
# t2-2.p: (* ERROR: objects must be declared before used in expressions *)
# t2-3.p: (* ERROR: objects of different types appear in the same expression *)
# t3-2.p: (* ERROR: test expressions in IF/WHILE statements must be Boolean *)
# t3-3.p: (* dangling ELSE binds to closest IF *)
# t3-4.p: (* ERROR: mismatched type for index variable in FOR loops *)
# t4-1.p: (* ERROR: non-integer type for array index *)
# t5-2.p: (* ERROR: function missing return statement *)
# t5-3.p: (* ERROR: function passed wrong number/type of arguments *)
# t5-4.p: (* ERROR: function updating non-local variable *)
# t6-1.p: (* ERROR: procedures don't return values *)
# t6-2.p: (* ERROR: procedure passed wrong number/type of arguments *)


# run all the tests in the tests directory in the background
# output pass or error for each test
make all
for file in test_files/*.p
do
    echo "Running $file"
    ./parser < $file
    if [ $? -eq 0 ]
    then
        echo "PASS"
    else
        echo "ERROR"
    fi
done