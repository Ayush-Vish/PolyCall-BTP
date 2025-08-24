
'use strict';
const {
     metacall,
      metacall_load_from_file 
    } =
     require('metacall');


metacall_load_from_file('py', [ 'scraping.py' ]);


const express = require('express');
const app = express();
const port = 3000;


app.get('/', (req, res) => res.send(req.query.url ?
    metacall('links', req.query.url) :
    'Invalid URL parameter'));

app.listen(port, () => console.log(`Listening on port ${port}!`));
