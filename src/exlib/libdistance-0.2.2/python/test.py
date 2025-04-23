#!/usr/bin/env python

import string, unittest

import distance

class DistanceTest(unittest.TestCase):
    """A simple and incomplete test case for the distance functions"""

    def testLD(self):
        str1 = "hello my name is jose"
        str2 = "hello m yname is jose"
        str3 = "hlelo my name is jose"
        self.assertEquals(distance.levenshtein(str1, str2), 2)
        self.assertEquals(distance.levenshtein(str1, str3), 2)
        self.assertEquals(distance.levenshtein(str1, str2), \
                          distance.levenshtein(str2, str1))

    def testHD(self):
        str1 = "hello my name is jose"
        str2 = "hello m yname is jose"
        str3 = "hlelo my name is jose"
        self.assertEquals(distance.hamming(str1, str2), 2)
        self.assertEquals(distance.hamming(str1, str3), 2)

    def testDD(self):
        str1 = "hello my name is jose"
        str2 = "hello m yname is jose"
        str3 = "hlelo my name is jose"
        self.assertEquals(distance.damerau(str1, str2), 0)
        self.assertEquals(distance.damerau(str1, str3), 0)

    def testJD(self):
        s1 = 'i like to party'
        s2 = 'this party is started'
        s3 = 'this parte is started'
        s4 = 'this party wasstarted'
        self.assertEquals(distance.jaccard(s1, s1), 0)
        self.assertEquals(distance.jaccard(s1, s2), -1)
        # print distance.jaccard(s2, s4)

    def testMD(self):
        str1 = "hello my name is jose"
        str2 = "hello m yname is jose"
        str3 = "hlelo my name is jose"
        self.assertEquals(distance.minkowski(str1, str2, 1), 23)
        self.assertEquals(distance.minkowski(str1, str2, 2), 265)
        self.assertEquals(distance.minkowski(str1, str2, 3), 3059)

    def testNW(self):
        m = distance.nw_matrix(insertion = 0.1, conversion = 1.0)
        str1 = "hello my name is jose"
        str2 = "hello m yname is jose"
        str3 = "hlelo my name is jose"
        self.failUnlessAlmostEqual(distance.needleman_wunsch(str1, str2, m), 0.2, 2)
        self.failUnlessAlmostEqual(distance.needleman_wunsch(str2, str3, m), 0.4, 2)
        self.failUnlessAlmostEqual(distance.needleman_wunsch(str1, str3, m), 0.2, 2)
        del(m)

        m = distance.nw_matrix()
        str1 = "hello my name is jose"
        str2 = "hello m yname is jose"
        str3 = "hlelo my name is jose"
        self.failUnlessAlmostEqual(distance.needleman_wunsch(str1, str2, m), 0.2, 2)
        self.failUnlessAlmostEqual(distance.needleman_wunsch(str2, str3, m), 0.4, 2)
        self.failUnlessAlmostEqual(distance.needleman_wunsch(str1, str3, m), 0.2, 2)
        del(m)

if __name__ == '__main__':
    # When this module is executed from the command-line, run all its tests
    unittest.main()
