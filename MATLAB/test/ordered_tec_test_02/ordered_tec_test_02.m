%%
clear
close all
clc
%%
addpath('../../source')
%%
x = 0:0.01:5;
[x,y] = meshgrid(x);
w = sin(x).*cos(y);
%%
tec_file = ORDERED_TEC.TEC_FILE;
tec_file.Variables = {'x','y','w'};
tec_file.Auxiliary = {{'test_01','t01'},{'test_001','t001'}};
tec_file.Zones = ORDERED_TEC.TEC_ZONE;
tec_file.Zones.Data = {x,y,w};
%%
tec_file = tec_file.write_plt();
