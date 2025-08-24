
'use strict';
const {
     polycall,
      polycall_load_from_file 
    } =
     require('polycall');


polycall_load_from_file('py', [ 'scraping.py' ]);


const express = require('express');
const app = express();
const port = 3000;


app.get('/', (req, res) => res.send(req.query.url ?
    polycall('links', req.query.url) :
    'Invalid URL parameter'));

app.listen(port, () => console.log(`Listening on port ${port}!`));
