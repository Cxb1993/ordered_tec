# Ordered_tec for C++

Ordered_tec is a simple pack writing ordered data (structural data) to a binary tecplot file (.plt). Ordered_tec for C++ is written in C++. This package does not need any dependency, and it directly write binary files according to Tecplot binary data format.

## Features

### Basic Features
* Support 1, 2 and 3 dimensional data
* Support multi-zone file
* Support file types including grid, solution and full
* Support solution time and strand Id for zone
* Support auxiliary data for file and zone

### Hightlight Features
1. Auto recognition the data type, supporting five types of data
2. Support exclude data with `Begin`, `End` and `Skip`
3. Free setting of echo mode
4. Complete log system including log in xml and json

## Usage

### Download and Configuration
You only need to download the two files in the folder `source` for using this package.

Before using it in your project, you need to copy the two files into your project directory or somewhere else to make sure that the complier could find it. 

### Coding
The Tecplot file consists of zones and data are storage in zone. Zones describe different set of data in space or in time. All the numbers of data in different zones in one file are same. Auxiliary data is extra data attached to file or zone which can be seen and used in Tecplot.

There are six classes usually used included in the `ORDERED_TEC` namespace. They are `TEC_FILE`, `TEC_ZONE`, `TEC_DATA` and correspondingly `TEC_FILE_LOG`, `TEC_ZONE_LOG`, `TEC_DATA_LOG` used to contain log.

To use the package, first you need some including and namespace declaration.
```c++
# include "ordered_tec.h"
using namespace ORDERED_TEC;
```

Then you need to declare a `TEC_FILE` object and set its properties. The property `Variables` is a string vector containing the variables' name which is required.
```c++
TEC_FILE tec_file;
tec_file.Variables.push_back("x");
tec_file.Variables.push_back("y");
tec_file.Variables.push_back("z");
```
You can also set some other properties optionally.
```c++
tec_file.FileName = "Test";
tec_file.Title = "Test";
tec_file.add_auxiliary_data("a_data_1","test01");
tec_file.add_auxiliary_data("a_data_2",0.5);
```

Then attach a `TEC_ZONE` object to the file and set its properties.
```c++
tec_file.Zones.push_back(TEC_ZONE());
//x, y, z are the point to the data, the size of all data is 10 by 30
tec_file.Zones[0].Max[0] = 10;
tec_file.Zones[0].Max[1] = 30;
tec_file.Zones[0].Data.push_back(TEC_DATA(x)); //auto recognition point type
tec_file.Zones[0].Data.push_back(TEC_DATA(y));
tec_file.Zones[0].Data.push_back(TEC_DATA(z));
```

You can also set some other properties optionally.
```c++
tec_file.Zones[0].ZoneName = "zone_1";
tec_file.Zones[0].StrandId = 0;
tec_file.Zones[0].SolutionTime = 0.1;
tec_file.Zones[0].Begin[1] = 1; //begin of data offset 1 in second dimension
tec_file.Zones[0].End[0] = 2; //end of data offset 2 in first dimension
tec_file.Zones[0].Skip[0] = 2; //skip to write data by 2 in first dimension
tec_file.Zones[0].add_auxiliary_data("a_data_1","test01");
tec_file.Zones[0].add_auxiliary_data("a_data_2",0.5);
```

Before write data, you can set echo mode optionally.
```c++
tec_file.set_echo_mode("full","full");
```

And write data.
```c++
tec_file.write_plt();
```

Now you can get log in `tec_file`'s property `last_log`. Also you can write the log in a xml file.
```c++
tec_file.last_log.write_xml();
```

### Compile
The file functions are based on 32 bit, so it is not supported to write big file over 2G. Try to split data into small files.

## Contribution
You can just comment in issues or contact me by [email](mailto:luan_ming_yi@126.com) to talk about your idea or demand. Thank you.

## License
[MIT License](https://opensource.org/licenses/MIT)
