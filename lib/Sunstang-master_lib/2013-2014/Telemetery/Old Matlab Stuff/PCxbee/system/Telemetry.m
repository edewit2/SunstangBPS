function varargout = Telemetry(varargin)
% TELEMETRY MATLAB code for Telemetry.fig
%      TELEMETRY, by itself, creates a new TELEMETRY or raises the existing
%      singleton*.
%
%      H = TELEMETRY returns the handle to a new TELEMETRY or the handle to
%      the existing singleton*.
%
%      TELEMETRY('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in TELEMETRY.M with the given input arguments.
%
%      TELEMETRY('Property','Value',...) creates a new TELEMETRY or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before Telemetry_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to Telemetry_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help Telemetry

% Last Modified by GUIDE v2.5 06-May-2014 05:59:47

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @Telemetry_OpeningFcn, ...
                   'gui_OutputFcn',  @Telemetry_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before Telemetry is made visible.
function Telemetry_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to Telemetry (see VARARGIN)

% Choose default command line output for Telemetry
handles.output = hObject;
serials = instrhwinfo('serial');
set(handles.selectCOMs, 'String' ,  serials.AvailableSerialPorts);
handles.driver = struct;
handles.driver.accel = [];
handles.driver.temp = [];

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes Telemetry wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = Telemetry_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on selection change in selectCOMs.
function selectCOMs_Callback(hObject, eventdata, handles)
% hObject    handle to selectCOMs (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns selectCOMs contents as cell array
%        contents{get(hObject,'Value')} returns selected item from selectCOMs


% --- Executes during object creation, after setting all properties.
function selectCOMs_CreateFcn(hObject, eventdata, handles)
% hObject    handle to selectCOMs (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in connectButton.
function connectButton_Callback(hObject, eventdata, handles)
% hObject    handle to connectButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if strcmp(get(hObject, 'String'),  'Connect') 
    list=get(handles.selectCOMs,'String');
    val=get(handles.selectCOMs,'Value');
    set(handles.selectCOMs,'Enable', 'off');
    
    s = serial(list{val});
    s.Terminator = 'i';
    s.BaudRate = 9600;
    s.BytesAvailableFcn = {@framedCallbackHandler, handles};
    fopen(s);

    handles.s = s;
    
    set(hObject, 'string', 'Disconnect');
    guidata(hObject, handles);
else
    fclose(handles.s);
    set(hObject, 'string', 'Connect');
    set(handles.selectCOMs,'Enable', 'on');

    
end


% --- Executes when uipanel1 is resized.
function uipanel1_ResizeFcn(hObject, eventdata, handles)
% hObject    handle to uipanel1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in leftSignal.
function leftSignal_Callback(hObject, eventdata, handles)
% hObject    handle to leftSignal (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of leftSignal


% --- Executes on button press in rightSignal.
function rightSignal_Callback(hObject, eventdata, handles)
% hObject    handle to rightSignal (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of rightSignal


function [ output_args ] = framedCallbackHandler(s, evt, handles )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    
    startByte = hex2dec('A2'); %These value come are from the Spec
    endByte = hex2dec('69');

    persistent buff;
    persistent accel;
    persistent temp;
        
    if s.BytesAvailable > 0 % deal with concuruancy issues >.<
        buff = horzcat(buff, fread(s, s.BytesAvailable)');
    
        packStarts = buff == startByte;  %This will inclde the escaped values
        packEnds = buff == endByte;

        %We should always have a start character at the begining, if
        %not something bad happend and we just chuck out the data
        %before the first s in an attempt to recover
            
        if packStarts(1) == 0
            sIndex = find(packStarts, 1);
            buff(1:sIndex-1) = [];
            packStarts(1:sIndex-1) = [];
            packEnds(1:sIndex-1) = [];
            packEscapes(1:sIndex-1) = [];
            escapeMask(1:sIndex-1) = [];       
        end
        
        %start parsing the data
        startIndex = find(packStarts==1);
        endIndex = find(packEnds == 1);
        
        numFrames = length(endIndex);
        
        disp(['Number: ' num2str(numFrames)]);
        clear buff
            %{
            for f = 1:numFrames            
                frame = buff(startIndex(f)+2:endIndex(f)-2); %Get all the data between the start (excluding the type) and stop bits(excluding the check byte)
                frame = frame(~packEscapes(startIndex(f)+2:endIndex(f)-2));

                packType = buff(startIndex(f)+1); %get the packet type (so we know how to process it)

                try
                switch(packType(1))
                    case 0
                        disp('Test Packet')
                        disp(frame);
                    case 1
                        %disp('Driver Interface')
                        [leftBlinker, rightBlinker, horn, brake, hazards, a, t] = HandleDriverInterface(frame);
                        accel(end+1) = a;
                        temp(end+1) = (t/1024 * 500)/10;
                        
                        plot(handles.pedalPositionPlot, accel);
                        plot(handles.TempPlot, temp);
                                               
                        set(handles.leftSignal, 'Value', leftBlinker);
                        set(handles.rightSignal, 'Value', rightBlinker);
                        set(handles.horn, 'Value', horn);
                        set(handles.brakes, 'Value', brake);
                        set(handles.hazards, 'Value', hazards);

                        %disp(frame);
                    otherwise
                        warn('Unknown type');
                end
                catch 
                   disp('Well shit'); 
                end

                %disp(buff);
                %disp(packStarts);
                %disp(packEnds);
                %disp(pack);          
            end
        %}
           % buff(1:endIndex(f)) = [];
    end


% --- Executes on button press in horn.
function horn_Callback(hObject, eventdata, handles)
% hObject    handle to horn (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of horn


% --- Executes during object deletion, before destroying properties.
function figure1_DeleteFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


if isfield(handles, 's') %Close the connection is it's open
    fclose(handles.s);
    delete(handles.s);
end


% --- Executes on button press in brakes.
function brakes_Callback(hObject, eventdata, handles)
% hObject    handle to brakes (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of brakes


% --- Executes on button press in hazards.
function hazards_Callback(hObject, eventdata, handles)
% hObject    handle to hazards (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of hazards
