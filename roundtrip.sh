printf "5\ninteractive_test.csv\n6\ninteractive_test.out.csv\n13\n" | ./app > /dev/null
diff interactive_test.csv interactive_test.out.csv > /dev/null
if [ $? -ne 0 ]
then
	echo "Resultado incorrecto"
	diff interactive_test.csv interactive_test.out.csv --color
else
	echo "OK"
fi
