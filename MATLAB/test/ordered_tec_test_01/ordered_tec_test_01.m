%%
clear
close all
clc
%%
addpath('../../source')
%%
tec_file = ORDERED_TEC.TEC_FILE;
tec_file.Variables = {'x','y','w'};
tec_file.Auxiliary = {'test_01','t01';'test_001','t001'};
tec_file.Zones = ORDERED_TEC.TEC_ZONE(3);
disp(tec_file)
disp(tec_file.Zones(2))
%%
tec_file.last_log = ORDERED_TEC.TEC_FILE_LOG(tec_file);
tec_file.last_log.Zones = ORDERED_TEC.TEC_ZONE_LOG(tec_file.Zones);
disp(tec_file.last_log)
disp(tec_file.last_log.Zones(2))