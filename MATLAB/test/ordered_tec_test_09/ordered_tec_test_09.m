%%
clear
close all
clc
%%
addpath('../../source')
import ORDERED_TEC.*
%%
x = 0:0.1:2*pi;
y = 32*sin(x);
%%
tec_file = TEC_FILE;
tec_file.FileName = 'test_09';
tec_file.Variables = {'x','single','double','int32','int16','int8'};
tec_file.Zones = TEC_ZONE;
tec_file.Zones.Data = {x,single(y),double(y),int32(y),int16(y),int8(y)};
tec_file = tec_file.write_plt();
tec_file.last_log.write_echo();
tec_file.last_log.write_json();
tec_file.last_log.write_xml();
%%
tec_file.Zones.Data = {x,single(y),double(y),int32(y),int16(y),char(y)};
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
%%
clear ME
