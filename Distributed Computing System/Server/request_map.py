import sql

def handle(request):
	info = []

	if request[0] == 'n':
		info.append("REQ FOR NEW JOB")
		print "Got a request for a new job"

	if request[0] == 'u':
		info.append("REQ FOR UPDATE")
		print "Got a request for an update"

	if request[0] == 'r':
		info.append("REQ FOR REPORT")
		print "Got a request for a report"

	reply = ''.join(info).replace('\n', ' ')
	reply += '\n'
	return reply
