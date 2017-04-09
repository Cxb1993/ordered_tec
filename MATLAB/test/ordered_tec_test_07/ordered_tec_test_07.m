%%
clear
close all
clc
%%
addpath('../../source')
import ORDERED_TEC.*
%%
disp(size(TEC_FILE))
disp(size(TEC_FILE(4)))
disp(size(TEC_FILE([1,5])))
try
    disp(size(TEC_FILE([])))
catch ME
    disp(ME.message);
end
try
    disp(size(TEC_FILE([1,2.5])))
catch ME
    disp(ME.message);
end
try
    disp(size(TEC_FILE('test')))
catch ME
    disp(ME.message);
end
try
    disp(size(TEC_FILE(1,2)))
catch ME
    disp(ME.message);
end
%%
disp(size(TEC_ZONE))
disp(size(TEC_ZONE(4)))
disp(size(TEC_ZONE([1,5])))
try
    disp(size(TEC_ZONE([])))
catch ME
    disp(ME.message);
end
try
    disp(size(TEC_ZONE([1,2.5])))
catch ME
    disp(ME.message);
end
try
    disp(size(TEC_ZONE('test')))
catch ME
    disp(ME.message);
end
try
    disp(size(TEC_ZONE(1,2)))
catch ME
    disp(ME.message);
end
%%
disp(size(TEC_FILE_LOG))
disp(size(TEC_FILE_LOG(TEC_FILE)))
disp(size(TEC_FILE_LOG(TEC_FILE([2,5]))))
disp(size(TEC_FILE_LOG(4)))
disp(size(TEC_FILE_LOG([1,5])))
try
    disp(size(TEC_FILE_LOG([])))
catch ME
    disp(ME.message);
end
try
    disp(size(TEC_FILE_LOG([1,2.5])))
catch ME
    disp(ME.message);
end
try
    disp(size(TEC_FILE_LOG('test')))
catch ME
    disp(ME.message);
end
try
    disp(size(TEC_FILE_LOG(1,2)))
catch ME
    disp(ME.message);
end
%%
disp(size(TEC_ZONE_LOG))
disp(size(TEC_ZONE_LOG(TEC_ZONE)))
disp(size(TEC_ZONE_LOG(TEC_ZONE([2,5]))))
disp(size(TEC_ZONE_LOG(4)))
disp(size(TEC_ZONE_LOG([1,5])))
try
    disp(size(TEC_ZONE_LOG([])))
catch ME
    disp(ME.message);
end
try
    disp(size(TEC_ZONE_LOG([1,2.5])))
catch ME
    disp(ME.message);
end
try
    disp(size(TEC_ZONE_LOG('test')))
catch ME
    disp(ME.message);
end
try
    disp(size(TEC_ZONE_LOG(1,2)))
catch ME
    disp(ME.message);
end
%%
clear ME
