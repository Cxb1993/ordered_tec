%%
clear
close all
clc
%%
addpath('../../source')
import ORDERED_TEC.*
%%
x = 0:0.1:2*pi;
%%
tec_file = TEC_FILE;
tec_file.Variables = {'x','y'};
tec_file.Zones  = TEC_ZONE(2);
for kk = 1:numel(tec_file.Zones)
    tec_file.Zones(kk).Data = {x,sin(x+kk)};
end
%%
file_mode_code = logical(eye(7));
zone_mode_code = logical(eye(9));
for ii = file_mode_code
    for jj = zone_mode_code
        tec_file = tec_file.set_echo_mode(ii,jj);
        disp(tec_file.Echo_Mode)
        disp(tec_file.Zones(2).Echo_Mode)
        tec_file = tec_file.write_plt();
    end
end
%%
try
    tec_file = tec_file.set_echo_mode(logical([1,1]),logical([1,1]));
catch ME
    disp(ME.message)
end
try
    tec_file = tec_file.set_echo_mode(true(1,7),logical([1,1]));
catch ME
    disp(ME.message)
end
%%
mode_string = {'brief','full','simple','none','leave'};
for ii = mode_string
    for jj = mode_string
        tec_file = tec_file.set_echo_mode(ii{1},jj{1});
        disp(ii{1})
        disp(tec_file.Echo_Mode)
        disp(jj{1})
        disp(tec_file.Zones(2).Echo_Mode)
        tec_file = tec_file.write_plt();
    end
end
%%
try
    tec_file = tec_file.set_echo_mode('','');
catch ME
    disp(ME.message)
end
try
    tec_file = tec_file.set_echo_mode('simple','');
catch ME
    disp(ME.message)
end
%%
try
    tec_file = tec_file.set_echo_mode([1,1],true(1,7));
catch ME
    disp(ME.message)
end
try
    tec_file = tec_file.set_echo_mode(true(1,7),{});
catch ME
    disp(ME.message)
end
%%
clear ii jj kk ME