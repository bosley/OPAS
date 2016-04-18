import sql

def handle(request):
    info = []

    #
    #		NEW JOB REQUESTED
    #
    if request[0] == 'n':
        print "Got a request for a new job"
	data = sql.fetch("SELECT * from openjobs WHERE jobtype > -1 AND tid = (SELECT max(tid));")[0]

	# Build the query to move job to pending table
	pending_q = ("INSERT INTO pendingjobs(tid, jobtype, jobdesc, jobdata) VALUES (%s, %s, '%s', '%s');" %
                (data[0], data[1], data[2], data[3]))

	# Build info with delimeters for transit
	for el in data:
		el = str(el)
		info.append(el)
		info.append("^")
	info.pop(-1)	# Remove the last item, as its a spare delimeter

	# Move job to pending table
	print "> Moving job with table ID [" + str(info[0]) + "] to the pending jobs table"
	res = sql.update(pending_q)	
	if res == -1:
		print ">> There was an error moving job to the pending table! Changes reverted"
	elif res == 1:
		print ">> Job moved to pending table"

	# Remove job from open table
	print "> Removing job with table ID [" + str(info[0]) + "] from open jobs";
	res1 = sql.update("delete from openjobs where tid = %s;" % str(info[0]))
	if res1 == -1:
		print ">> There was an error removing the job from the open table! Changes reverted"
	elif res == 1:
		print ">> Job removed from open table"

    #
    #		UPDATE TO JOB REQUESTED
    #
    if request[0] == 'u':
	# EXPECTING  :  u, tid, type, desc, data
	# If sorting : data = item,item,item;ORIGINAL_DESC;ORIGINAL_DATA
	# Insert data into closed
	print "Got a request to update job"
	print "> Moving job with table ID [" + str(request[1]) + "] to the closed table"
	sql.update("INSERT INTO closedjobs(tid, jobtype, jobdesc, jobdata) VALUES (%s, %s, '%s', '%s');" % 
			(request[1], request[2], request[3], request[4]))

	# Remove item from pending
        print "> Removing job with table ID [" + str(request[1]) + "] from the pending jobs";
        res1 = sql.update("delete from pendingjobs where tid = %s;" % str(request[1]))
        if res1 == -1:
                print ">> There was an error removing the job from the pending table! Changes reverted"
        elif res1 == 1:
                print ">> Job removed from pending table"

	# Thank the client
	info.append("skynet thanks you for your service")

    #
    #		REPORT REQUESTED
    #
    if request[0] == 'r':
        info.append("REQ FOR REPORT")
        print "Got a request for a report"

    #
    #		JOIN AND SEND DATA
    #
    reply = ''.join(info).replace('\t', ' ')
    reply += '\t'
    return reply
