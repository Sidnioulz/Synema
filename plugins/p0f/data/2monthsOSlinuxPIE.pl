
#proc getdata
file: 2monthsOSlinux.txt

#proc pie
firstslice: 10
explode 0 .05 .1 .2
datafield: 2
labelfield: 1
labelmode: line+label
center: 4 4
radius: 0.7
colors: auto
labelfarout: 0
labelfmtstring: @@1 @@PCT%
#proc page
titledetails: textsize=10
//scale: 1.5 1.5
