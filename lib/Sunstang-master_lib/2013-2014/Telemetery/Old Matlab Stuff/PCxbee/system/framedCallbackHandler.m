function [ output_args ] = framedCallbackHandler(s, evt )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    disp('Fucking here')
    startByte = hex2dec('A2'); %ASCII S
    endByte = hex2dec('69');   %E
    
    global fig;
    persistent buff;
        
    if s.BytesAvailable > 0 % deal with concuruancy issues >.<
        buff = horzcat(buff, fread(s, s.BytesAvailable)');
        
        %Mark where in the buffer each packet starts and ens
        packStarts = buff == startByte; 
        packEnds = buff == endByte;

        %We should always have a start character at the begining, if
        %not something bad happend and we just chuck out the data
        %before the first packet
            
        if packStarts(1) == 0
            sIndex = find(packStarts, 1);
            buff(1:sIndex-1) = [];
            packStarts(1:sIndex-1) = [];
            packEnds(1:sIndex-1) = [];          
        end
            
            
        %start parsing the data
        startIndex = find(packStarts==1);
        endIndex = find(packEnds == 1);
            
        numFrames = length(endIndex);
        done = false; 
        start = 1;
        
        while ~done
            subSystemID = buff(start + 1);
            packetID = buff(start +2 );
            lengthFromStream = buff(start+3);
            
            %first check if we know what that packet is, and weather it's
            %the rigt length
            
            import javax.xml.xpath.*
            factory = XPathFactory.newInstance;
            xpath = factory.newXPath;
            xDoc = xmlread('C:\Users\Peter\Documents\SunStang\Telemetery\PCxbee\system\dataConfig.XML');

            
            numberBytesExpression = xpath.compile(['solarcar/subsystem[@id=''' dec2hex(subSystemID) ''']/packet[@id=''' num2str(packetID) ''']/data/@bytes']);
            bytes = numberBytesExpression.evaluate(xDoc, XPathConstants.NODELIST);
            numDataPoints = bytes.getLength;
            lengthFromFile =0 ;
            
            for d = 0:numDataPoints -1
               lengthFromFile = lengthFromFile + str2double(byte.item(d).getTextContent);
            end
            
            if lengthFromFile == lengthFromStream
               data = (start+4 : start+4+lengthFromFile); 
            end
            
        end
    end
end

