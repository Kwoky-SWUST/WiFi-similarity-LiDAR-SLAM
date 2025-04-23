import distance
m = distance.nw_matrix(insertion=1,conversion=1)
s1 = "hay"
s2 = "h@y"
print distance.needleman_wunsch(s1, s2, m)
m.setConversion("a", "@", 0.1)
s1 = "hay"
s2 = "h@y"
print distance.needleman_wunsch(s1, s2, m)
s1 = "buy vi@gr@ from us!!!"
s2 = "buy your viagra from us!!!"
print distance.needleman_wunsch(s1, s2, m)
print distance.levenshtein(s1, s2)

v = ['Vi--agra',
'Vi-ag-ra',
'VIAGR @',
'Via-gra',
'Vi.agr-a',
'vi__@gr@',
'Via-gra',
'Vi--agra',
'Viagr,a',
'Viaqra']

for x in v:
    for y in v:
        print "%10s -> %10s: %f" % (x, y, distance.needleman_wunsch(x, y, m))

del(m)
