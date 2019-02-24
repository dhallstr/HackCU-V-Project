import threading
from websocket_server import WebsocketServer

class SOQ(threading.Thread):

    def __init__(self):
        threading.Thread.__init__(self)
        self.running = True
        self.server = 0

    def new_client(self, client, server):
        self.server.send_message_to_all("")

    def msg(self, m):
        self.server.send_message_to_all(m)

    def run(self):
      self.server = WebsocketServer(9001)
      self.server.set_fn_new_client(self.new_client)
      self.server.run_forever()
