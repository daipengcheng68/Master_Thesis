const express = require('express')
const expressWs = require('express-ws');
const app = express()
const web_port = 8080

const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')
const port = new SerialPort("/dev/ttyACM0", { baudRate: 9600 })
    // var stdin = process.openStdin();

var g_ws = null;

const parser = new Readline()
port.pipe(parser)

parser.on('data', (line) => {
    if (g_ws) {
        g_ws.send(line);
    }
    // console.log("> " + line)
})

// stdin.addListener("data", function(data) {
//     var str = data.toString().trim();
//     console.log("send->: " + str);
//     port.write(str)
// });



app.use(express.static('public'));

expressWs(app);

app.ws('/ws', function(ws, req) {
    if (g_ws) {
        g_ws.close();
    }
    g_ws = ws;
    ws.on('message', function(msg) {
        port.write(msg);
        // ws.send(msg);
    });
});

app.listen(web_port, () => {
    console.log(`server listening at http://localhost:${web_port}`)
})