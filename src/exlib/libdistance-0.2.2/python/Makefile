all: distancemodule.so

distancemodule.so: pydistance.c
	python setup.py build
	# cp build/lib.darwin-7.9.0-Power_Macintosh-2.3/distancemodule.so .

install:
	python setup.py install

clean:
	rm -rf build/ distancemodule.so *.core
