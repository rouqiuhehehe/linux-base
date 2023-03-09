const net = require("net");

for (let i = 0; i < 100; i++) {
    net.createServer((socket) => {
    }).listen(3000 + i);
}