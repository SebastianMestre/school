printf "5\nroundtrip.csv\n6\nroundtrip.out.csv\n13\n" | ./app > /dev/null
diff -i -w roundtrip.csv roundtrip.out.csv > /dev/null
if [ $? -ne 0 ]
then
	echo "Resultado incorrecto"
	diff -i -w roundtrip.csv roundtrip.out.csv --color
else
	echo "OK"
fi
