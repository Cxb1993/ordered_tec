%%
clear
close all
clc
%%
addpath('../../source')
import ORDERED_TEC.*
%%
x = 0:0.05:2*pi;
[x,y] = meshgrid(x);
M = 2;
N = 3;
for nn = 1:N
    for mm = 1:M
        u{mm,nn} = sin(x+mm).*cos(y+nn);
        v{mm,nn} = cos(x+mm)+sin(y+nn);
    end
end
%%
tec_file = TEC_FILE;
tec_file.Variables = {'x','u';'y','v'};
tec_file.Zones = TEC_ZONE([M,N]);
for nn = 1:N
    for mm = 1:M
        tec_file.Zones(mm,nn).Data = {x,u{mm,nn};y,v{mm,nn}};
    end
end
%%
tec_file = tec_file.write_plt();
tec_file.last_log.write_echo();
tec_file.last_log.write_json();
tec_file.last_log.write_xml();
