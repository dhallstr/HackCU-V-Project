from bottle import route, run, static_file, request
import soq

s = soq.SOQ()
s.daemon = True
s.start()

@route("/set")
def test():
	s.msg(request.query.c)
	return "Received."

@route("/")
def sendFile(filepath="index.html"):
	return static_file(filepath, root='')

run(host='localhost', port=8080, debug=True)
