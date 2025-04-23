from distutils.core import setup, Extension

setup(
	name = "pydistance",
	version = "0.2.1",
	author = "Jose Nazario",
	author_email = "jose@monkey.org",
	license = "BSD",
	long_description = \
'''pydistance provides functions to compare strings and evaluate their relative distances.
--------
''',
	ext_modules = [Extension(
		"distancemodule",
		sources = ["pydistance.c"],
		include_dirs = [".."],
		libraries = ["distance"],
		library_dirs = [".."]
		) ],
	url = "http://monkey.org/~jose/software/libdistance/",
)
