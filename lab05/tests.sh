#!/bin/bash

#############################################################################

echo "TEST 1: Verify that all inputs were correctly sorted"
# generate some data, put in a file called "mydata"
./gen 100 R > mydata
# count the number of lines in the data (should be 100)
wc -l mydata
# sort the data using sortA, put the result in "sortedA"
./sortA < mydata > sortedA
# sort the data using sortB, put the result in "sortedB"
./sortB < mydata > sortedB
# count the number of lines in "sortedA" (should be 100)
wc -l sortedA
# count the number of lines in "sortedB" (should be 100)
wc -l sortedB
# sort the data using Unix sort
sort -n < mydata > sorted
# check that the sortA and sortB programs actually sorted
diff sorted sortedA   # should show no diffs
diff sorted sortedB   # should show no diffs
# remove generated files
rm mydata sorted sortedA sortedB

#############################################################################

echo -e "\nTEST 2: Compare timing for different orders"
# take an average of the three times that will be output

echo "sortA timing // 1,000,000 elements"
dataA1=`seq 1000000`
echo "- ascending: "
echo $dataA1 | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA1 | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA1 | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo "- descending: "
echo $dataA1 | sort -nr | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA1 | sort -nr | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA1 | sort -nr | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo "- random: "
echo $dataA1 | sort -R | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA1 | sort -R | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA1 | sort -R | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo "sortA timing // 2,500,000 elements"
dataA2=`seq 2500000`
echo "- ascending: "
echo $dataA2 | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA2 | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA2 | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo "- descending: "
echo $dataA2 | sort -nr | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA2 | sort -nr | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA2 | sort -nr | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo "- random: "
echo $dataA2 | sort -R | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA2 | sort -R | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA2 | sort -R | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo "sortA timing // 5,000,000 elements"
dataA3=`seq 5000000`
echo "- ascending: "
echo $dataA3 | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA3 | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA3 | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo "- descending: "
echo $dataA3 | sort -nr | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA3 | sort -nr | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA3 | sort -nr | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo "- random: "
echo $dataA3 | sort -R | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA3 | sort -R | /usr/bin/time --format="%U seconds" ./sortA > /dev/null
echo $dataA3 | sort -R | /usr/bin/time --format="%U seconds" ./sortA > /dev/null

echo "sortB timing // 1,000,000 elements"
dataB1=`seq 1000000`
echo "- ascending: "
echo $dataB1 | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB1 | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB1 | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo "- descending: "
echo $dataB1 | sort -nr | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB1 | sort -nr | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB1 | sort -nr | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo "- random: "
echo $dataB1 | sort -R | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB1 | sort -R | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB1 | sort -R | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo "sortB timing // 2,500,000 elements"
dataB2=`seq 2500000`
echo "- ascending: "
echo $dataB2 | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB2 | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB2 | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo "- descending: "
echo $dataB2 | sort -nr | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB2 | sort -nr | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB2 | sort -nr | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo "- random: "
echo $dataB2 | sort -R | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB2 | sort -R | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB2 | sort -R | /usr/bin/time --format="%U seconds" ./sortB > /dev/null

echo "sortB timing // 5,000,000 elements"
dataB3=`seq 5000000`
echo "- ascending: "
echo $dataB3 | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB3 | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB3 | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo "- descending: "
echo $dataB3 | sort -nr | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB3 | sort -nr | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB3 | sort -nr | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo "- random: "
echo $dataB3 | sort -R | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB3 | sort -R | /usr/bin/time --format="%U seconds" ./sortB > /dev/null
echo $dataB3 | sort -R | /usr/bin/time --format="%U seconds" ./sortB > /dev/null

#############################################################################

echo -e "\nTest 3: Check stability"
seqList=`seq 20`
genList=`./gen 20 R`
combined=`echo -e "$seqList\n$genList"`
echo -e "--- Original combined seq/gen ---\n$combined"
echo -e "\n------ sortA ------"
echo "$combined" | ./sortA
echo -e "\n------ sortB ------"
echo "$combined" | ./sortB

#############################################################################

echo -e "\n***** ALL TESTS COMPLETED *****\n"

#############################################################################

# rm data files after lab finished
