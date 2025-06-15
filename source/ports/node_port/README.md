<div align="center">
  <a href="https://metacall.io" target="_blank"><img src="https://raw.githubusercontent.com/metacall/core/master/deploy/images/logo.png" alt="M E T A C A L L" style="max-width:100%; margin: 0 auto;" width="80" height="80">
  <p><b>M E T A C A L L</b></p></a>
  <p>A library for providing inter-language foreign function interface calls</p>
</div>

# Abstract

**metacall** is a library that allows calling functions, methods or procedures between programming languages. With **metacall** you can transparently execute code from / to any programming language, for example, call Python code from NodeJS.

# Install

Install metacall binaries first ([click here](https://github.com/metacall/install) for installing it on other platforms):
```bash
curl -sL https://raw.githubusercontent.com/metacall/install/master/install.sh | sh
```

# Example

`sum.py`
``` python
def sum(a, b):
  return a + b
```

`main.js`
``` javascript
const { sum } = require('./sum.py'); // Import Python script

sum(3, 4); // 7
```

``` sh
metacall main.js
```
