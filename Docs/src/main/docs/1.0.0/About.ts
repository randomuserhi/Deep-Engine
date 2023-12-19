RHU.require(new Error(), { 
    docs: "docs", rhuDocuscript: "docuscript",
}, function({
    docs, rhuDocuscript,
}) {
    docs.jit = () => docuscript<RHUDocuscript.Language, RHUDocuscript.FuncMap>((lib, include) => {
        const { 
            p, br, link, h1, i 
        } = lib;
        const { 
            cb 
        } = include({ cb: "code:block" });
       
        p(
            "This is the Docuscript document for my game engine 'Deep'. The github repo can be found ",
            link("https://github.com/randomuserhi/Enter-The-Deep", "here"), "."
        );
        br();

        h1("test");
        cb(["javascript"], `function test() { return "hello" };`);
    }, rhuDocuscript);
});