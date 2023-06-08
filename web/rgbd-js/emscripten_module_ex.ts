// Adding methods exported using EXTRA_EXPORTED_RUNTIME_METHODS
// to the EmscriptenModule interface of @types/emscripten.
// For more information, see index.d.ts of @types/emscripten.
export interface EmscriptenModuleEx extends EmscriptenModule {
  ccall: typeof ccall;
  UTF8ToString: typeof UTF8ToString;
  setValue: typeof setValue;
  canvas: HTMLCanvasElement;
}
