const childrenProcess = require("child_process");

const maxProcess = 3;
const maxClient = 999999;
const processMaxClient = maxClient / maxProcess;
for (let i = 0; i < 3; i++) {
    childrenProcess.fork("./tcp-connect-children-process.js", [processMaxClient, i], {
        silent: false
    });
}