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
u = sin(x).*cos(y);
v = cos(x)+sin(y);
%%
tec_file_g = TEC_FILE;
tec_file_g.FileName = 'test_04_g';
tec_file_g.FileType = 1;
tec_file_g.Variables = {'x','y'};
tec_file_g.Auxiliary = {{'a1','1'},{'a2','2'}};
tec_file_g.Zones = TEC_ZONE;
tec_file_g.Zones.Data = {x,y};
%%
tec_file_g = tec_file_g.write_plt();
tec_file_g.last_log.write_echo();
tec_file_g.last_log.write_json();
tec_file_g.last_log.write_xml();
%%
tec_file_s = TEC_FILE;
tec_file_s.FileName = 'test_04_s';
tec_file_s.FileType = 2;
tec_file_s.Variables = {'u','v'};
tec_file_s.Zones = TEC_ZONE;
tec_file_s.Zones.Data = {u,v};
%%
tec_file_s = tec_file_s.write_plt();
tec_file_s.last_log.write_echo();
tec_file_s.last_log.write_json();
tec_file_s.last_log.write_xml();
