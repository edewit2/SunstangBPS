%% Discharge Profile Data Logger

%% Connect to Arduino Serial Port
%
clear all
clc

instrreset;     
arduino=serial('COM11','BaudRate',9600);

fopen(arduino);
     
formatSpec = '%f';

%% Record and plot 10 seconds of data
%
voltage = zeros(1e4,1);
current = zeros(1e4,1);
temp = zeros(1e4,1);
t = zeros(1e4,1);

i=0;

tic
while toc < 10    
    i = i + 1;
    for j=1:3
    A(i,j) = fscanf(arduino,formatSpec);
    end
    t(i) = toc;
end

fclose(arduino);

t = t(1:i);
V = A(:,1);
I = A(:,2);
T = A(:,3);

disp('making plot..')
% Plot temperature versus time
figure
plot(t,V)
xlabel('Elapsed time (sec)')
ylabel('Voltage (V)')
title('Ten Seconds of Voltage Data')
set(gca,'xlim',[t(1) t(i)])