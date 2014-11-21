
for i in range (1,6):
	for j in range(1,6):
		config = 'config' + str(i)+ str(j)
		f1 = open(config, 'w+')
		f= open('config.txt','r')
		count =0
		for line in f:
			if count == 0:
				f1.write("PAGE_SIZE "+ str(i*50) +"\n")
			elif count == 1:
				f1.write("NUM_PAGES "+ str(j*2) +"\n")
			else:
				f1.write(line)
			count +=1
		f1.close()
		f.close