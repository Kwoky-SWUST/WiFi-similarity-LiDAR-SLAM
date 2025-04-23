#!/usr/local/bin/tclsh8.4

load ./distance.so


set list1 [list "hello" "there"]
set list2 [list "hlelo" "their"]

set MAX [expr [llength $list1] - 1]

proc compare {d1 d2} {
	set l1 [string length $d1]
	set l2 [string length $d2]
	return [::distance::jaccard_d $d1 $l1 $d2 $l2]
}

proc hamming {d1 d2} {
	set l1 [string length $d1]
	set l2 [string length $d2]
	return [::distance::hamming_d $d1 $l1 $d2 $l2]
}

set i 0
set j 0

foreach a $list1 {					;# i
	set one [lindex $list1 $i]
	foreach b $list1 {				;# j
		set two [lindex $list2 $j]

		# a v b == b v a ...
		puts "jaccard\t$i:$one\t $j:$two\t\t [compare $one $two]"
		puts "hamming\t$i:$one\t $j:$two\t\t [hamming $one $two]"
		if {$j < $MAX} { incr j } else { set j 0 }
	}
	if {$i < $MAX} { incr i } else { set i 0 }
}
