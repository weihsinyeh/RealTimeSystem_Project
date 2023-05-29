echo 1 2 3 4 | xargs -n 1 >> args
rm -r outputData
mkdir outputData
./main ./inputData/example1.json > ./outputData/example_output_1
./main ./inputData/example2.json > ./outputData/example_output_2
./main ./inputData/example3.json > ./outputData/example_output_3




