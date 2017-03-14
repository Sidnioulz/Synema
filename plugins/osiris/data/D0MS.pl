#proc getdata
	file: module_sensitivity.txt
#endproc


#proc processdata
	action: rotate
	fieldnames: folder day19 day18 day17 day16 day15 day14 day13 day12 day11 day10 day9 day8 day7 day6 day5 day4 day3 day2 day1 day0
#endproc


#proc pie
	center: 4 4
	radius: 1.5
	colors: auto
	labelmode: line+label
	labelfield: folder
	datafield: day0
	textdetails: color=black size=0.5
#endproc

#proc annotate	
	location: 2.5 2.5
//	text: DAY 0, Module Sensitivity	
#endproc 
