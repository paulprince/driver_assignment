make -C /usr/src/linux M=`pwd` clean
if [ -f `pwd`/test ]
then 
	rm test
fi

