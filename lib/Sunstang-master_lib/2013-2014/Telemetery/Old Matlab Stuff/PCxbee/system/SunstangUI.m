function varargout = SunstangUI(varargin)
% SUNSTANGUI MATLAB code for SunstangUI.fig
%      SUNSTANGUI, by itself, creates a new SUNSTANGUI or raises the existing
%      singleton*.
%
%      H = SUNSTANGUI returns the handle to a new SUNSTANGUI or the handle to
%      the existing singleton*.
%
%      SUNSTANGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SUNSTANGUI.M with the given input arguments.
%
%      SUNSTANGUI('Property','Value',...) creates a new SUNSTANGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before SunstangUI_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to SunstangUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help SunstangUI

% Last Modified by GUIDE v2.5 01-Apr-2014 15:19:21

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @SunstangUI_OpeningFcn, ...
                   'gui_OutputFcn',  @SunstangUI_OutputFcn, ...
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


% --- Executes just before SunstangUI is made visible.
function SunstangUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to SunstangUI (see VARARGIN)

%%assign the battery-current plot to the one being displayed
%axes(handles.battery_current);
a = [];
b = [];
c = [];
d = [];
e = [];
f = [];
g = [];
h = [];
i = [];


%y_array = zeros(1,100000);

%%generate random y values
for i = 1:100
    r1 = times(rand,5);
    r2 = times(rand,5);
    r3 = times(rand,5);
    r4 = times(rand,5);
    r5 = times(rand,5);
    r6 = times(rand,5);
    r7 = times(rand,5);
    r8 = times(rand,5);
    
    a(end + 1) = r1;
    b(end + 1) = r2;
    c(end + 1) = r3;
    d(end + 1) = r4;
    e(end + 1) = r5;
    f(end + 1) = r6
    g(end + 1) = r7;
    h(end + 1) = r8;
    %a(end + 1) = r1;
    %a(end + 1) = r1;

    drawnow;
end

updateBatteryVoltage(a, handles);
updateBatteryCurrent(b, handles);
updateBatteryTemperature(c, handles);
updateSolarArrayCurrent(d, handles);
updateSolarArrayVoltage(e, handles);
updateMotorSpeed(f, handles);
updateMotorTemperature(g, handles);
updateCabinTemperature(h, handles);

% Choose default command line output for SunstangUI
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes SunstangUI wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = SunstangUI_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;




function [] = updateBatteryVoltage(most_updated_plot_points, handles) 
    axes(handles.battery_voltage);
    plot(most_updated_plot_points, '-.dk','linewidth',1.5); 

function [] = updateBatteryCurrent(most_updated_plot_points, handles) 
    axes(handles.battery_current);
    plot(most_updated_plot_points, '-.dk','linewidth',1.5); 
    
function [] = updateBatteryTemperature(most_updated_plot_points, handles) 
    axes(handles.battery_temperature);
    plot(most_updated_plot_points, '-.dk','linewidth',1.5); 

function [] = updateSolarArrayCurrent(most_updated_plot_points, handles) 
    axes(handles.solar_array_current);
    plot(most_updated_plot_points, '-.dk','linewidth',1.5); 

function [] = updateSolarArrayVoltage(most_updated_plot_points, handles) 
    axes(handles.solar_array_voltage);
    plot(most_updated_plot_points, '-.dk','linewidth',1.5); 
    
function [] = updateMotorSpeed(most_updated_plot_points, handles) 
    axes(handles.motor_speed);
    plot(most_updated_plot_points, '-.dk','linewidth',1.5); 
    
function [] = updateMotorTemperature(most_updated_plot_points, handles) 
    axes(handles.motor_temperature);
    plot(most_updated_plot_points, '-.dk','linewidth',1.5); 
    
function [] = updateCabinTemperature(most_updated_plot_points, handles) 
    axes(handles.cabin_temperature);
    plot(most_updated_plot_points, '-.dk','linewidth',1.5); 




