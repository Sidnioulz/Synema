
#proc getdata
file: 2monthsOSgeneral.txt

#proc pie
firstslice: 10
explode 0 .05 .1 .2
datafield: 2
labelfield: 1
labelmode: line+label
center: 4 4
radius: 1
colors: auto
labelfarout: 1.05

#proc page
titledetails: textsize=20
//scale: 1.5 1.5
