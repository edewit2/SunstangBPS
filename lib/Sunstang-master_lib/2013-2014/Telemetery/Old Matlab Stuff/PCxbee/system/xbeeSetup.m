function [ s ] = xbeeSetup( serialName )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

 s = serial(serialName);
 s.Terminator = hex2dec('69');
 s.BytesAvailableFcn = @framedCallbackHandler;
 fopen(s);

end

