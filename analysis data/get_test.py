from random import randrange


f= open('get_test.txt','w+')

for i in range(0,500):
	j = randrange(100)
	f.write("countries,"+str(j)+"\n")