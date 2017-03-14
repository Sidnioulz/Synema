#proc getdata
	file: list_module_unknown.txt
#endproc

#proc processdata
	fieldnames: 1 names
#endproc


#proc pie
	center: 4 4
	radius: 1.5
	colors: auto
	labelmode: line+label
	labelfield: names
	datafield: 1
	textdetails: color=black size=0.5
#endproc

#proc annotate	
	location: 2.5 2
//	text: List of modules (UNKNOWN)
#endproc 

