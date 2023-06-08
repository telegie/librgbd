WebAssembly.compileStreaming(fetch("rgbd-wasm.wasm"))
.then(module => {
  const imports = WebAssembly.Module.imports(module);
  console.log(imports[0]);
  console.log(imports[1]);
});