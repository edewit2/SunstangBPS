function [ out ] = TestSend( s )
%UNTITLED Test the sending of 80 'a' 0xfff and SEX(It's innoccent... its
%the speacial charaters :P )
%   Detailed explanation goes here
    fopen(s);
    pause(3);
    in = fread(s, s.BytesAvailable)';
    fclose(s);
    c = [83 0 80 97 255 255 49 69 83 0 88 83 88 69 0 88 88 78 69];
    
    out = in  == c;

end

