function [ pack ] = testPackGen( SubsystemID, packID,  data)
%UNTITLED Generates a packet that looks like something coming from the master 
    pack(1) = hex2dec('A2');
    pack(2) = SubsystemID;
    pack(3) = packID;
    pack(4) = length(data);
    pack = [pack data hex2dec('69')];
   
    %c%c%s963C', SubsystemID, packID, data));
end

