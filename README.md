### csv2json [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

csv2json is a simple command line tool that converts a csv file to a json and prints to standard output.

```
csv2json 0.1 - compiled: Jul 30 2022 01:04:32
options:
        -h, --help      prints this helpful message
        -b, --beautify  json output will be beautified
        -d, --delimiter must be followed by a single character such as ';' or the word `tab` if the input file is tab separated. default is comma (,)
        -n, --noheader  first line will not be treated as header, field names will be automatically generated
example use:
        csv2json meta/sample/points.csv --beautify
        csv2json meta/sample/points-noheader.csv --beautify --noheader
        csv2json meta/sample/points-tab-separated.csv --delimiter tab --beautify

hk@hasankaraman.devhk@hasankaraman.dev
```
Dump output to file:
```
csv2json meta/sample/points.csv --beautify > points.json
```

How to install:
```
git clone https://github.com/grizzlei/csv2json
cd csv2json
make install
source .profile
```

Thanks:

https://github.com/nlohmann/json

---

_Hasan Karaman, hk@hasankaraman.dev, 2022_