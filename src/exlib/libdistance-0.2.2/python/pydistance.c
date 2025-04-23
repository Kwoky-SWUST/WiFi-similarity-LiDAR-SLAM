#include <sys/types.h>
#include "Python.h"
#include "distance.h"

static PyObject *pydistance_error;         /* distance.error */

struct nw_matrix {
	PyObject_HEAD
	struct matrix _m;
} nw_matrix_t;

static char 	pydistance__doc__[] =
"String distance comparison functions\n"
"\nThe distance library is used to compare pieces of data for similarity.\n"
"Specifically, it contains a number of methods to find the 'edit distance'\n"
"between inputs, or the number of differences between them. These\n" 
"differences are calculated using various mechanisms.\n"
"\n"
"Each fuction returns the calculated distance between the two inputs. A\n"
"distance of 0 indicates that the strings are the same. A distance less\n"
"than 0 indicates that an error has occurred. For the Hamming and Jaccard\n"
"distances, this is because the two inputs are of different sizes.\n"
"\nLorenzo Seidenari wrote the Levenshtein distance implementation.\n"
"\nAUTHORS\n\n"
"Lorenzo Seidenari wrote the Levenshtein distance implementation.\n\n"
"Jose Nazario <jose@monkey.org> wrote the implementations of the Damerau,\n"
"Hamming and Jaccard distance algorithms along with the Needleman-Wunsch\n"
"Levenshtein distance implementation here.\n\n"
"Evan Cooke adapted the adler32 routine from Jean-loup Gailly and Mark\n"
"Adler used in the bloom distance function, which he also wrote.\n"
"\nSEE ALSO\n\n"
"V. I. Levenshtein, 'Binary codes capable of correcting deletions,\n"
"insertions and reversals', Doklady Akademii Nauk SSSR, 4, 163, 845-848,\n"
"1965.\n"
"\n"
"Burton Bloom, 'Space/time trade-offs in hash coding with allowable\n"
"errors', Communications of the ACM, 7, 13, 422-426, July 1970.\n"
"\n"
"Fred J. Damerau, 'A technique for computer detection and correction of\n"
"spelling errors', Communications of the ACM, 3, 7, 171-176, March 1964.\n"
"\n"
"Burton Bloom, 'Space/time trade-offs in hash coding with allowable\n"
"errors', Communications of the ACM, 7, 13, 422-426, July 1970.\n"
"\n"
"R. W. Hamming, 'Error Detecting and Error Correcting Codes', Bell System\n"
"Tech Journal, 9, 147-160, April 1950.\n"
"\n"
"P. Jaccard, 'Etude comparative de la distribution florale dans une\n"
"portion des Alpes et du Jura', Bull Soc Vaudoise Sci Nat, 44, 223-270,\n"
"1908.\n";

static PyObject *
raisePydistanceError(char *errmsg)
{
        PyErr_SetString(pydistance_error, errmsg);
        return NULL;
}

static char	pydistance__levenshtein__doc__[] =
"levenshtein(string1, string2)\n\n"
"Levenshtein distance (LD) is a measure of the similarity between two\n"
"inputs, which we will refer to as the source s and the target input t.\n"
"The distance is the number of deletions, insertions, or substitutions\n"
"required to transform s into t.  For example,\n"
"\n"
"If s is 'test' and t is 'test', then LD(s,t) = 0, because no \n"
"transformations are needed. The strings are already identical.\n"
"\n"
"If s is 'test' and t is 'tent', then LD(s,t) = 1, because one \n"
"substitution (change 's' to 'n') is sufficient to transform s into t.\n"
"\n"
"The greater the Levenshtein distance, the more different the inputs are.\n"
"\n"
"Levenshtein distance is named after the Russian scientist Vladimir \n"
"Levenshtein, who devised the algorithm in 1965. If you can't spell or \n"
"pronounce Levenshtein, the metric is also sometimes called edit distance.";

static PyObject *
pydistance_levenshtein(PyObject *na, PyObject *args)
{
	char *str1, *str2;
	int ret;

	if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
	ret = levenshtein_d(str1, strlen(str1), str2, strlen(str2));
	return PyInt_FromLong((long) ret);
}

static char	pydistance__damerau__doc__[] =
"damerau(string1, string2)\n\n"
"The Damerau distance is almost identical to the Levenshtein distance but\n"
"can tolerate adjascent characters that have been swapped, a common\n"
"spelling mistake or typographic error. For example, if s is 'abcd' and t\n"
"is 'acbd', then DD(s,t) is 0 because of the transposition of 'b' and 'c'.\n"
"Other costs found in the Levenshtein distance are identical.";

static PyObject *
pydistance_damerau(PyObject *na, PyObject *args)
{
	char *str1, *str2;
	int ret;

	if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
	ret = damerau_d(str1, strlen(str1), str2, strlen(str2));
	return PyInt_FromLong((long) ret);
}

static char 	pydistance__hamming__doc__[] =
"hamming(string1, string2)\n\n"
"The Hamming distance H is defined only for inputs of the same length.\n"
"For two inputs s and t, H(s, t) is the number of places in which the two\n"
"string differ, i.e., have different characters.";

static PyObject *
pydistance_hamming(PyObject *na, PyObject *args)
{
	char *str1, *str2;
	int ret;

	if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
	ret = hamming_d(str1, strlen(str1), str2, strlen(str2));
	return PyInt_FromLong((long) ret);
}

static char	pydistance__jaccard__doc__[] =
"jaccard(string1, string2)\n\n"
"The Jaccard similarity between two strings is the ratio of the size of\n"
"their intersection to the size of their union. This distance is 1 minus\n"
"this similarity sscore. Indentical strings have a Jacard distance of 0,\n"
"completely dissimilar strings have a score of 1. The two inputs must be\n"
"of the same size.";

static PyObject *
pydistance_jaccard(PyObject *na, PyObject *args)
{
	char *str1, *str2;
	float ret;

	if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
	ret = jaccard_d(str1, strlen(str1), str2, strlen(str2));
	return PyFloat_FromDouble((long) ret);
}

static char	pydistance__minkowski__doc__[] =
"minkowski(string1, string2, power)\n\n"
"The Minkowski distance between two strings is the geometric distance\n"
"between two inputs and uses a variable scaling factor, power.  When this\n"
"value is 1 the Minkowski distance is equal to the Manhattan distance.\n"
"When power is 2 it yields the Euclidian distance between two strings.\n"
"When this value grows, the value of the Minkowski distance tends towards\n"
"a Chebyshev result. Therefore by inceasing power, one can place more\n"
"numerical value on the largest distance (in terms of elements in the two\n"
"vectors in question). \n"
"\n"
"A disadvantage of the Minkowski method is that if one element in the \n"
"vectors has a wider range than the other elements then that large range\n"
"may 'dilute' the distances of the small-range elements.";

static PyObject *
pydistance_minkowski(PyObject *na, PyObject *args)
{
	char *str1, *str2;
	int power;
	double ret;

	if (!PyArg_ParseTuple(args, "ssi", &str1, &str2, &power)) return NULL;
	ret = minkowski_d(str1, strlen(str1), str2, strlen(str2), power);
	return PyFloat_FromDouble((long) ret);
}

static char	pydistance__manhattan__doc__[] = 
"manhattan(string1, string2)\n\n"
"The Manhattan distance is a special case of the Minkowski distance where\n"
"the 'power' is set to 1.";

static PyObject *
pydistance_manhattan(PyObject *na, PyObject *args)
{
	char *str1, *str2;
	double ret;

	if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
	ret = MANHATTAN_D(str1, strlen(str1), str2, strlen(str2));
	return PyFloat_FromDouble((long) ret);
}

static char	pydistance__euclid__doc__[] = 
"euclid(string1, string2)\n\n"
"The Euclid distance is a special case of the Minkowski distance where\n"
"the 'power' is set to 2.";

static PyObject *
pydistance_euclid(PyObject *na, PyObject *args)
{
	char *str1, *str2;
	double ret;

	if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
	ret = EUCLID_D(str1, strlen(str1), str2, strlen(str2));
	return PyFloat_FromDouble((long) ret);
}

static char	pydistance__bloom__doc__[] =
"bloom(string1, string2)\n\n"
"A Bloom Filter (BF) is a method of encoding a variable length piece of\n"
"input data to a fixed length signature.  The basic idea is to map\n"
"attributes of the data to specific positions in a binary vector.  First,\n"
"the result vector is initialized to all 0's.  Next, a series of hash\n"
"functions are applied to the data to map some part of the input data\n"
"to a position in the result vector.  Thus, if a is the input data,\n"
"and m is length of the result vector, then hash(a) produces a value in the\n"
"range 0...m-1.\n"
"\n"
"A similar method encoding data into a fixed length result vector can be\n"
"accomplished using a crytographic digest.  A digest function like MD5\n"
"is designed such that the difference between any two digests does not in\n"
"any way correlate to similar differences in input messages.  This is \n"
"an important property in certain applications however it may also be \n"
"desirable that the difference between digests corresponds to\n"
"differences in the input messages.  Because a Bloom filter is derived from\n"
"attributes of the input message, the difference between any two Bloom\n"
"Filter digests corresponds to differences in the input data.\n"
"\n"
"This Bloom filter implementation is meant take any byte sequence as input\n"
"so a generic hash function is used.  The input stream is divided into\n"
"chunks and hashed using a 32 bit adler modulo m.  In order to get the most\n"
"complete coverage, the chunk N+1 includes all but one of the bytes in\n"
"chunk N and one byte not included in chunk N.  (i.e. the chunk window\n"
"slides to right one byte per hash.)\n"
"\n"
"The difference between any two Bloom filter digests is computed by taking\n"
"the logical and of the two digests and computing the number of bit of\n"
"similarity.  The similar count is then divided by maximum of the total\n"
"number of bit in either digest.  This acts to normalize the bit count for\n"
"large and small signatures.  The similarity is then turned into a distance\n"
"by take 1 - normalized bit count.\n";

static PyObject *
pydistance_bloom(PyObject *na, PyObject *args)
{
        char *str1, *str2, *d1, *d2;
	double ret;
	int bits;

        if (!PyArg_ParseTuple(args, "ssi", &str1, &str2, &bits)) return NULL;
	d1 = (char *)malloc(bits);
	d2 = (char *)malloc(bits);
	if (!d1 || !d2)
		raisePydistanceError("Couldn't allocate memory.");

        bloom_create(str1, strlen(str1), d1, bits);
        bloom_create(str2, strlen(str2), d2, bits);
	ret = bloom_d(d1, d1, bits);
	free(d1);
	free(d2);
	return PyFloat_FromDouble((long) ret);
}

static char	pydistance__needleman_wunsch__doc__[] = 
"The Levenshtein distance algorithm assumes that the cost of all insertions \n"
"or conversions is equal. However, in some scenarios this may not be\n"
"desirable and may mask the acceptable distances between inputs.\n"
"\n"
"A modified Levenshtein distance algorithm, also known as the Needleman-\n"
"Wunsch distance algorithm, accepts a cost matrix as an extra input. This\n"
"matrix object (distance.nw_matrix) contains two cost matricies for each \n"
"pair of characters to convert from and to. The costs of inserting this \n"
"character and converting between characters is listed in this matrix.\n";

static PyObject *
pydistance_needleman_wunsch(PyObject *na, PyObject *args)
{
        char *str1, *str2;
	float ret;
	struct nw_matrix *nw_m;
	struct matrix *m;
	PyObject *tmp;

        if (!PyArg_ParseTuple(args, "ssO", &str1, &str2, &nw_m, &tmp)) 
		return NULL;
	m = (struct matrix *)(&nw_m->_m);
	ret = needleman_wunsch_d(str1, strlen(str1), str2, strlen(str2), m);
	return PyFloat_FromDouble(ret);
}

static PyObject *
NwMatrix_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	struct nw_matrix *self;
	self = (struct nw_matrix *)type->tp_alloc(type, 0);
	Py_INCREF(self);
	return (PyObject *)self;
}

static void
NwMatrix_dealloc(struct nw_matrix *self)
{
	Py_DECREF(self);
	self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
NwMatrix_init(struct nw_matrix *self, PyObject *args, PyObject *kwds)
{
	static char *kwlist[] = {"insertion", "conversion", NULL};
	double ins = 1.0, conv = 1.0;
	int x, y;

	if (! PyArg_ParseTupleAndKeywords(args, kwds, "|dd", kwlist, 
					  &ins, &conv))
		return NULL;
	/* initialize the matrix ... */
        for (x = 0; x < 255; x++) {
                for (y = 0; y < 255; y++) {
                        self->_m.conversion[x][y] = conv;
                        self->_m.insertion[x][y] = ins;
                }
        }
	Py_INCREF(self);
	return 0;
}

static char pydistance__nw_matrix__setConversion__doc__[] = 
	"setConversion(source, dest, cost)\n"
	"set the cost of a conversion";

static PyObject *
pydistance_nw_matrix_setConversion(struct nw_matrix * self, PyObject *args)
{
        char *s, *d;
	int from, to;
        double val;

        if (!PyArg_ParseTuple(args, "ssd", &s, &d, &val)) 
		return NULL;

	from = s[0];
	to = d[0];
	Py_DECREF(self);
	Py_INCREF(self);
	self->_m.conversion[from][to] = val;
	return PyFloat_FromDouble(0.0);
}

static char pydistance__nw_matrix__setInsertion__doc__[] = 
	"setInsertion(source, dest, cost)\n"
	"set the cost of an insertion";

static PyObject *
pydistance_nw_matrix_setInsertion(struct nw_matrix * self, PyObject *args)
{
        char *s, *d;
	int from, to;
        double val;

        if (!PyArg_ParseTuple(args, "ssd", &s, &d, &val)) 
		return NULL;

	from = s[0];
	to = d[0];
	Py_DECREF(self);
	Py_INCREF(self);
	self->_m.insertion[from][to] = val;
	return PyFloat_FromDouble(0.0);
}

#define mkMethod(x)                                             \
    {#x, pydistance_##x, METH_VARARGS, pydistance__##x##__doc__}

static PyMethodDef pydistance_methods[] = {
	mkMethod(levenshtein),
	mkMethod(hamming),
	mkMethod(damerau),
	mkMethod(jaccard),
	mkMethod(minkowski),
	mkMethod(manhattan),
	mkMethod(euclid),
	mkMethod(bloom),
	mkMethod(needleman_wunsch),
	{NULL, NULL}
};

#define mkNwMethod(x)                                             \
    {#x, pydistance_nw_matrix_##x, METH_VARARGS, pydistance__nw_matrix__##x##__doc__}

static PyMethodDef nw_matrix_methods[] = {
        mkNwMethod(setConversion),
        mkNwMethod(setInsertion),
        {NULL, NULL}
};

/* see http://www.python.org/doc/2.3/ext/node22.html */

static PyTypeObject distance_MatrixType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "distance.nw_matrix",      /*tp_name*/
    sizeof(nw_matrix_t),         /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)NwMatrix_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */ 
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/ 
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Needleman Wunsch cost matrix",           /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    nw_matrix_methods,         /* tp_methods */
    0,				/* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)NwMatrix_init,   /* tp_init */
    0,                         /* tp_alloc */
    NwMatrix_new,                 /* tp_new */
};


#if 0
static PyObject *
pydistance_init(PyObject *na, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ":init")) return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}
#endif 	/* 0 */

DL_EXPORT(void)
initdistance(void)
{
	PyObject *m;
	distance_MatrixType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&distance_MatrixType) < 0)
		return;
	m = Py_InitModule3("distance", pydistance_methods, pydistance__doc__);
	pydistance_error = PyErr_NewException("distance.error", NULL, NULL);
	Py_INCREF(pydistance_error);
	Py_INCREF(&distance_MatrixType);
	PyModule_AddObject(m, "error", pydistance_error);
	PyModule_AddObject(m, "nw_matrix", (PyObject *)&distance_MatrixType);
	PyModule_AddStringConstant(m, "__version__", "0.2.1");
}
