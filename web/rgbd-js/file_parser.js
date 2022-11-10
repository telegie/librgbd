import { NativeFile } from "./file.js";
import { PointerByReference } from "./pointer_by_reference.js";

export class NativeFileParser {
  constructor(wasmModule, data) {
    this.wasmModule = wasmModule;
    const parserPtrRef = new PointerByReference(wasmModule);
    const dataPtr = wasmModule._malloc(data.byteLength);
    wasmModule.HEAPU8.set(data, dataPtr);
    const result = wasmModule.ccall("rgbd_file_parser_ctor_from_data", "number", ["number", "number", "number"], [parserPtrRef.ptr, dataPtr, data.byteLength]);

    const parserPtr = parserPtrRef.getValue();
    parserPtrRef.close();

    if (result < 0)
      throw new Error("Failed to createFileParserFromData");

    this.ptr = parserPtr;
    // Need to keep this dataPtr alive to read the data using the parser.
    this.dataPtr = dataPtr;
  }

  close() {
    this.wasmModule.ccall("rgbd_file_parser_dtor", null, ["number"], [this.ptr]);
    this.wasmModule._free(this.dataPtr);
  }

  parseNoFrames() {
    const filePtr = this.wasmModule.ccall('rgbd_file_parser_parse_no_frames', "number", ["number"], [this.ptr]);
    return new NativeFile(this.wasmModule, filePtr);
  }

  parseAllFrames() {
    const filePtr = this.wasmModule.ccall('rgbd_file_parser_parse_all_frames', "number", ["number"], [this.ptr]);
    return new NativeFile(this.wasmModule, filePtr);
  }
}
