%% Getting Start
% This is a simple example for using ordered_tec.
%% clear
clear
close all
clc
%% use namespace
import ORDERED_TEC.*
%% prepare data
x = 0:0.01:5;
[x,y] = meshgrid(x);
w = sin(x).*cos(y);
%% pack data in TEC classes
tec_file = TEC_FILE;
tec_file.Variables = {'x','y','w'};
tec_file.Zones = TEC_ZONE;
tec_file.Zones.Data = {x,y,w};
%% write data
tec_file = tec_file.write_plt();
