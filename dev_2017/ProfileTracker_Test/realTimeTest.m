%% Discharge Profile Data Logger

%% Initialization
%
clear all
clc

disp('Discharge Profile Data Logger');

%% Connect to Arduino Serial Port
%
disp('Connecting to serial port...');

instrreset;     
arduino=serial('COM11','BaudRate',9600);

fopen(arduino); 
formatSpec = '%f';

%{
UIdone = false;
while ~UIdone
mode = input('Enter the discharge mode: ','s');
switch mode
    case 'current'
        fprintf(arduino,'%i',3);
        UIdone = true;
    case 'power'
        fprintf(arduino,'%i',5);
        UIdone = true;
    case 'resistance'
        fprintf(arduino,'%i',9);
        UIdone = true;
    case 'pulse'
        fprintf(arduino,'%i',17);
        UIdone = true;
    otherwise
        disp('incorrect mode');
end
end
%}

%% Acquire and Display Real-time Data
%
disp('Running data acquisition');

figure
h = animatedline;
ax = gca;
ax.YGrid = 'on';

k = 0;
stop = false;
startTime = datetime('now');
while ~stop
    k = k + 1;
    % Read sensors
    for j=1:3
        A(j,k) = fscanf(arduino,formatSpec); 
    end
    power(k) = A(1,k).*A(2,k);
    resistance(k) = A(1,k)./A(2,k);
    % Get current time
    t =  datetime('now') - startTime;
    % Add points to animation
    addpoints(h,datenum(t),A(1,k))
    % Update axes
    ax.XLim = datenum([t-seconds(15) t]);
    datetick('x','keeplimits')
    drawnow
    % Display data on terminal
    fprintf('******** Display data ********\n');
    fprintf('Voltage: %f\n',A(1,k));
    fprintf('Temperature: %f\n',A(3,k));
    fprintf('Current: %f\n',A(2,k));
    fprintf('Power: %f\n',power(k));
    fprintf('Resistance: %f\n',resistance(k));
    % Check stop condition
    if A(1,k) < 3
        stop = true;
    end
end   

fclose(arduino);

%% Plot the recorded data
%
[timeLogs,voltageLogs] = getpoints(h);
timeSecs = (timeLogs-timeLogs(1))*24*3600;
smoothedVoltage = smooth(voltageLogs,25);
%smoothedCurrent = smooth(A(2,:),25);
%smoothedTemp = smooth(A(3,:),25);
%smoothedPower = smoothedVoltage.*smoothedCurrent;
%smoothedResistance = smoothedVoltage./smoothedCurrent;
figure
plot(timeSecs,voltageLogs,timeSecs,smoothedVoltage)
xlabel('Elapsed time (sec)')
ylabel('Voltage (V)')

%% Save results to a file

T = table(timeSecs',voltageLogs',A(2,:)',A(3,:)',power',resistance',...
    'VariableNames',{'Time_sec','Voltage_V','Current_A','Temp_C',...
    'Power_W','Resistance_ohm'});
filename = 'DischargeProfile_Data.xlsx';
% Write table to file 
writetable(T,filename)
% Print confirmation to command line
fprintf('Results table with %g profile measurements saved to file %s\n',...
    length(timeSecs),filename)