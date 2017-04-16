# Ordered_tec

Ordered_tec is a simple pack writing ordered data (structural data) to a binary tecplot file (.plt). Now this pack contains C++ and Matlab version. This package does not need any dependency, and it directly write binary files according to Tecplot binary data format.

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
More details seen in [C++ version](./C++/ordered_tec_cpp.md) and [MATLAB version](./MATLAB/ordered_tec_matlab.md)

## Contribution
You can just comment in issues or contact me by [email](mailto:luan_ming_yi@126.com) to talk about your idea or demand. Thank you.

## License
[MIT License](https://opensource.org/licenses/MIT)
