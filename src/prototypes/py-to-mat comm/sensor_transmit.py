# SENSOR_TRANSMIT.PY
# Written by David Rebhuhn (rebhuhnd@onid.oregonstate.edu / rebhuhnd@gmail.com)
#
# Dummy Python-based Server Program that transmits data
# upon request of a client program.
#
# This is based off of the simple server program described in
#
#      https://docs.python.org/2/library/socket.html
#
# Note that the following implementation uses the IPV4-compatible version
# only.

import socket

def CONN_LOOP():
    print "Starting up connection loop...."
    HOST = ''                 # Symbolic name meaning all available interfaces
    PORT = 50008              # Arbitrary non-privileged port
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((HOST, PORT))
    print "Awaiting client program..."
    s.listen(1)
    conn, addr = s.accept()
    print 'Connected by', addr
    while 1:
        data = conn.recv(1024)
        #print "request recieve", len(data)
        if data == "x\n":
            #print "Sending X Velocity"
            conn.sendall('1')
        elif data == "y\n":
            conn.sendall('4')
            #print data
        if not data: break
        conn.sendall(data)
        
    print "Bye, Client Program!"
    conn.close()

def SERVER():
    while(1):
        response = raw_input('Start a new connection? y-n')
        if (response == 'y'):
            CONN_LOOP()
        if (response == 'n'):
            break;


SERVER()

# DEPRECATED:


#raw_input("Press any key to begin...")

#HOST = ''                 # Symbolic name meaning all available interfaces
#PORT = 50008              # Arbitrary non-privileged port
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.bind((HOST, PORT))
#s.listen(1)
#conn, addr = s.accept()
#print 'Connected by', addr
#while 1:
#    data = conn.recv(1024)
#    print "request recieve", len(data)
#    if data == "x\n":
#        print "Sending X Velocity"
#        conn.send('1')
#    else:
#        conn.send('4')
#        print data
#    if not data: break
#    conn.sendall(data)
#print "Bye, matlab program!"
#conn.close()
#raw_input("Press any key to exit...")

