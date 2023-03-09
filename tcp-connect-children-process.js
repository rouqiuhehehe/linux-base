const net = require("net");

function f(max, processId) {
    let i = 0;
    let portI = 0;
    let time = Date.now();

    function e(port) {
        if (port % 100 === 0) port = 0;
        console.log(3000 + port);
        const client = net.connect(3000 + port++, "192.168.19.128", (socket) => {
            i++;
            client.write("hello");
            if (i % 10000 === 9999) {
                const now = Date.now();
                console.log(`${i}个连接，共计${now - time}ms，进程${processId}`);
                time = now;
            }
            if (i < max) e(port);
        });
    }

    e(portI);
}

const max = process.argv[2];
const processId = process.argv[3];
f(max, processId);