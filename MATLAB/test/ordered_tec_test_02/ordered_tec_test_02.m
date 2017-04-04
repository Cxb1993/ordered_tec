%%
clear
close all
clc
%%
addpath('../../source')
import ORDERED_TEC.*
%%
x = 0:0.01:5;
[x,y] = meshgrid(x);
w = sin(x).*cos(y);
%%
tec_file = TEC_FILE;
tec_file.Variables = {'x','y','w'};
tec_file.Zones = TEC_ZONE;
tec_file.Zones.Data = {x,y,w};
%%
tec_file = tec_file.write_plt();
disp(tec_file.last_log)
disp(tec_file.last_log.Zones)
