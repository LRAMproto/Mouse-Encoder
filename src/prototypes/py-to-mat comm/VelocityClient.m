classdef VelocityClient < hgsetget
    
    properties
        x = 0
        y = 0
        conn = []
    end
    
    methods
        function Connect(self)
            % The following is a script which sets up a TCP/IP connection
            % to a python program. It sends a test message to the Python
            % program once connected.
            
            % Creates a TCP/IP connection
            t=tcpip('localhost', 50008, 'NetworkRole', 'client');

            try
            % Opens the connection to allow writing to the socket.
            fopen(t);
            catch e
                error('Unable to connect. Are you sure the server is running?');
            end
            % Pauses, allowing the Python program to connect.
            pause(1);
            
            % Prints a test message.
            %fprintf(t,'Hello, Python Program!');
            self.conn = t;
        end
        
        function response = Request(self,msg)
            fprintf(self.conn,msg);
            %pause(0.001)
            data = fscanf(self.conn, '%s');
            response = data;
        end
        
        function Update(self)
            self.x = Request('x');
            self.y = Request('y');
        end
        
        function Quit(self)
            fclose(self.conn)
        end
    end
    
end

