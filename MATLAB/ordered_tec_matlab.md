# Ordered_tec for MATLAB

Ordered_tec is a simple pack writing ordered data (structural data) to a binary tecplot file (.plt). Ordered_tec for MATLAB is written in MATLAB OOP. This package does not need any dependency, and it directly write binary files according to Tecplot binary data format.

## Features

### Basic Features
* Support 1, 2 and 3 dimensional data
* Support multi-zone file
* Support file types including grid, solution and full
* Support solution time and strand Id for zone
* Support auxiliary data for file and zone

### Hightlight Features
1. Auto recognition the data type, supporting six types of data
2. Support exclude data with `Begin`, `EEnd` and `Skip`
3. Free setting of echo mode
4. Complete log system including log in xml and json

## Usage

### Download and Configuration
You only need to download the the folder `+ORDERED_TEC` and put it on the search path of MATLAB. Maybe we will distribute the package as a toolbox in the future.

### Coding
The Tecplot file consists of zones and data are storage in zone. Zones describe different set of data in space or in time. All the numbers of data in different zones in one file are same. Auxiliary data is extra data attached to file or zone which can be seen and used in Tecplot.

There are four classes usually used included in the `ORDERED_TEC` namespace. They are `TEC_FILE`, `TEC_ZONE` and correspondingly `TEC_FILE_LOG`, `TEC_ZONE_LOG` used to contain log.

To use the package, first you need some  namespace declaration.
```matlab
import ORDERED_TEC.*
```

Then you need to declare a `TEC_FILE` object and set its properties. The property `Variables` is a cell of string containing the variables' name which is required.
```matlab
tec_file = TEC_FILE;
tec_file.Variables = {'x','y','w'};
```
You can also set some other properties optionally.
```matlab
tec_file.FileName = 'Test';
tec_file.Title = 'Test';
tec_file.Auxiliary = {{'a1','1'},{'a2','2'}};
```

Then attach a `TEC_ZONE` object to the file and set its properties.
```matlab
tec_file.Zones = TEC_ZONE;
% x, y, z are the data
tec_file.Zones.Data = {x,y,w};
```

You can also set some other properties optionally.
```matlab
tec_file.Zones.ZoneName = 'zone_1';
tec_file.Zones.StrandId = 0;
tec_file.Zones.SolutionTime = 0.1;
tec_file.Zones.Begin = [1,2,1]; % begin of data from 2 in second dimension
tec_file.Zones.EEnd = [2,0,0]; % end of data offset 2 in first dimension
tec_file.Zones.Skip = [2,1,1]; % skip to write data by 2 in first dimension
tec_file.Zones.Auxiliary = {{'a1','1'},{'a2','2'}};
```

Before write data, you can set echo mode optionally.**These classes are value class, always remember to use the form of obj = obj.set_echo_mode(...)**
```matlab
tec_file = tec_file.set_echo_mode('full','full');
```

And write data.**These classes are value class, always remember to use the form of obj = obj.write_plt(...)**
```matlab
tec_file = tec_file.write_plt();
```

Now you can get log in `tec_file`'s property `last_log`. Also you can write the log in a xml file.
```matlab
tec_file.last_log.write_xml();
```

## Contribution
You can just comment in issues or contact me by [email](mailto:luan_ming_yi@126.com) to talk about your idea or demand. Thank you.

## License
[MIT License](https://opensource.org/licenses/MIT)
