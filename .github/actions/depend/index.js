const os = require("os");
const fs = require("fs");
const http = require("isomorphic-git/http/node");
const path = require("path");
const proc = require("child_process");
const git = require("isomorphic-git");
const action = require("@actions/core");
const cache = require("@actions/cache");
const io = require("@actions/io");

async function install_nasm(working_directory) {
    const remote = "https://github.com/netwide-assembler/nasm.git";
    const src = path.join(working_directory, "src");
    const bin = path.join(working_directory, "bin");
    await io.mkdirP(src);
    await io.mkdirP(bin);

    try {
        
        const refs = await git.listServerRefs({
            http,
            url: remote,
            prefix: "HEAD",
            symrefs: true
        });
        const head_ref = refs.find(ref => ref.ref == "HEAD");
        
        const cache_key = "nasm-" + head_ref.oid;

        if(cache.isFeatureAvailable && await cache.restoreCache([ bin ], "nasm-" + head_ref.oid) != undefined) {
            action.info("Restored from cache");
            action.addPath(bin);
        } else {
            action.info("Cached version is outdated");
            action.info("Cloning NASM...");
            await git.clone({
                fs,
                http,
                dir: src,
                url: remote,
                singleBranch: true,
                depth: 1
            });

            action.info("Building NASM");
            proc.spawnSync(path.join(src, "autogen.sh"), { 
                cwd: src,
                stdio: "inherit",
                shell: true
            });
            proc.spawnSync(path.join(src, "configure"), {
                cwd: src,
                stdio: "inherit",
                shell: true
            });
            proc.spawnSync("make", [ "-j", os.cpus().length.toString() ], {
                cwd: src,
                stdio: "inherit"
            });

            await io.mv(path.join(src, "nasm"), path.join(bin, "nasm"), { recursive: true, force: true });

            if(cache.isFeatureAvailable()) {
                action.info("Caching NASM");
                await cache.saveCache([ bin ], cache_key);
            }

            action.addPath(bin);
        }
    } catch (e) {
        action.error(e);
        return false;
    }

    return true;
}

async function run() {
    const working_directory = path.join(process.cwd(), "deps");
    await io.mkdirP(working_directory);

    const nasm = action.getBooleanInput("install_nasm");
    if (nasm) {
        action.info("Installing NASM");
        if(!await install_nasm(path.join(working_directory, "nasm"))) {
            action.error("Failed to install NASM");
            action.setFailed("Failed to install dependencies");
        }
    }
}

run();