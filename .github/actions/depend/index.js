const os = require("os");
const fs = require("fs");
const path = require("path");
const proc = require("child_process");
const git = require("nodegit");
const action = require("@actions/core");
const cache = require("@actions/cache");

async function run() {
    const working_directory = path.join(os.tmpdir(), "VolgaOS", "deps");
    if (fs.existsSync(working_directory)) {
        fs.rmSync(working_directory, { recursive: true });
    }

    //const nasm = action.getBooleanInput("install_nasm");
}

run();