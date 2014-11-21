import re
f= open('raw_data.csv','r')


for line in f:
	templine = line.split(",")
	templine[0] = re.sub('[^0-9a-zA-Z]+', '', str(templine[0]))
	templine[3] = re.sub('[^0-9a-zA-Z]+', '', str(templine[3]))
	templine[6] = re.sub('[^0-9a-zA-Z]+', '', str(templine[6]))
	templine[7] = re.sub('[^0-9a-zA-Z]+', '', str(templine[7]))
	
	print templine[0] + " , " + templine[3] + " , " + templine[6] + " , " + templine[7];
