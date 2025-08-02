
<div align="center">
  <h1><b>PolyCall Polyglot Runtime</b></h1>
</div>

**PolyCall** allows calling functions, methods or procedures between multiple programming languages.

`sum.py`
``` python
def sum(a, b):
  return a + b
```

`main.js`
``` javascript
const { sum } = require('./sum.py');

sum(3, 4); // 7
```

`shell`
``` sh
polycall main.js
```

**PolyCall** is an extensible, embeddable, and interoperable cross-platform polyglot runtime. It supports NodeJS, Vanilla JavaScript, Python,  Java, Go, C, C++.


For more information about other install methodologies and platforms or Docker, check the install documentation.
