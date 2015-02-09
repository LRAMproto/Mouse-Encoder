function data = velocity_client_test()

% The following test opens a connection and sends a number of requests to
% measure response time.

vc = VelocityClient();
vc.Connect();

num_passes = 10000;
data = struct('x',[],'y',[]);
t1 = tic;
for k=1:num_passes
   data.x = [data.x; vc.Request('x')];
   data.y = [data.y; vc.Request('y')];   
end
t2 = toc(t1);
disp(t2);
fprintf('Time Elapsed: %.5f seconds\n',t2);
fprintf('Time per pass: %.10f seconds\n',(t2)/num_passes);

vc.Quit()

end