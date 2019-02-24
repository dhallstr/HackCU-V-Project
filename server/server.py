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
@route('/<filepath:path>')
def sendFile(filepath="index.html"):
	return static_file(filepath, root='')

run(host='192.168.1.3', port=8080, debug=True)
