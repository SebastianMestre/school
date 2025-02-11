
mkdir -p tmp

for TEST_FILE in tests/test*
do
	# valgrind ./interprete < $TEST_FILE > tmp/salida
	./interprete < $TEST_FILE > tmp/salida

	EXPECTED_OUTPUT_FILE="${TEST_FILE/\/test/\/salida}"

	diff <(sed -e '$a\' tmp/salida | sed 's/^[> ]*//;/^$/d') $EXPECTED_OUTPUT_FILE > /dev/null
	if [ $? -ne 0 ]
	then
		echo "resultado incorrecto en $TEST_FILE"
		diff <(sed -e '$a\' tmp/salida | sed 's/^[> ]*//;/^$/d') $EXPECTED_OUTPUT_FILE --color
	else
		echo "$TEST_FILE OK"
	fi
done
