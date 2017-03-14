#proc getdata
fieldnameheader: no
file: 2weeksdistance.txt

#proc areadef
rectangle: 1 1 6.4 5
xrange: 0 7
yautorange: datafield=2 lowfix=0

#proc xaxis
stubs: datafields=1
//stubs: text
//Windows
//Linux
//Mac
//Others
label: Distance

#proc yaxis
label: Number of users
stubs: inc auto
minortics: yes
minorticinc: 5

#proc bars
lenfield: 2
 
barwidth: 0.2
