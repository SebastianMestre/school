
mkdir -p tmp

for TEST_FILE in tests/{memory_,}test*
do
	echo "=== corriendo $TEST_FILE ==="
	valgrind ./interprete < $TEST_FILE > /dev/null
	echo ""
done
