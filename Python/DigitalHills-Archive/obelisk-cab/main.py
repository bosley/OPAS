#   Josh A. Bosley
#   Example of using the connector object to 
#   send and receive data

from connector import connection

def hci_test(conn, mode):
    try:
        while True:
            x = input("connector-xmnlib > ")

            '''
                Send one message
            '''
            if x == "sone":
                
                # Due to xmnlib using (to,message) format for each send, we need to setup the send
                # to match the mode. IP sends raw data, not structured
                if mode == 'xmnlib':
                    y = input("TO (Node address) : ")
                    z = input("MESSAGE: ")
                    conn.send_one((y,z))

                elif mode == 'ip':
                    m = input("Data to send: ")
                    conn.send_one(m)

            '''
                Send one to outgoing queue
            '''
            if x == "sadd":
                
                # Due to xmnlib using (to,message) format for each send, we need to setup the send
                # to match the mode. IP sends raw data, not structured
                if mode == 'xmnlib':
                    y = input("TO (Node address) : ")
                    z = input("MESSAGE: ")
                    conn.send_add_to_queue((y,z))

                elif mode == 'ip':
                    m = input("Data to send: ")
                    conn.send_add_to_queue(m)

            '''
                Push all in outgoing queue
            '''
            if x == "squeue":
                conn.send_pour_queue()

            '''
                Get messages
            '''
            # 1 Message
            if x == "get1":
                m = conn.get_one()
                print(m)

            # All messages
            if x == "geta":
                m = conn.get_all()
                print(m)

    except KeyboardInterrupt:
        return
    

'''
    XNM Connection
'''
def xmnlib_connection():
    jb_conn = connection('obelisk', '<xmn-pillar>@<ip>', port=<port>, identifier="local.node.001")
    hci_test(jb_conn, 'xmnlib')

'''
    IP Connection 
'''
def ip_connection():
    print("NYD")
    jb_conn = connection('ip', '<ip>', port=<port>)
    hci_test(jb_conn, 'ip')

#print("xmnlib connection =>\n\n")
#xmnlib_connection()

print("tcp connection => \n\n")
ip_connection()
