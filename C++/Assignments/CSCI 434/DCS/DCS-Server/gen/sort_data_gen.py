#
#	Josh Bosley
#	Generate random numbers for DCS sort tests
#

import sql
import random

# Get number of data sets to be generated
x = raw_input("How many jobs? :")

# List, and gen params
RAND_DATA = []
RAND_LOW  = 1
RAND_HIGH = 10000

# Generate each data set
for j in range(0, int(x)):

	# Generate data size
	RAND_DATA_SIZE = random.randint(100, 10000)

	# Generate random numbers
	for i in range(0, RAND_DATA_SIZE):
		RAND_DATA.append(random.randint(RAND_LOW, RAND_HIGH))

	# Build description string
	DS = "DATA_SIZE:" + str(RAND_DATA_SIZE)
	print DS

	# Build data string
	DD = ""
	for el in RAND_DATA:
		DD += str(el)
		DD += "\n"
	DD = DD[:-1]

	# Add to database
	sql.update("INSERT INTO openjobs(tid, jobtype, jobdesc, jobdata) VALUES (default, 1, '%s', '%s');" % (DS, DD))

	# Signal that the job is completed
	print "> Completed [" + str(j) + "]"
